// Copyright 1997-2021 by Jon Dart. All Rights Reserved.
//
#include "protocol.h"

#include "attacks.h"
#include "bench.h"
#include "bitprobe.h"
#include "boardio.h"
#include "calctime.h"
#include "chessio.h"
#include "eco.h"
#include "globals.h"
#include "learn.h"
#include "legal.h"
#include "log.h"
#include "movearr.h"
#include "movegen.h"
#include "notation.h"
#include "scoring.h"
#ifdef SYZYGY_TBS
#include "syzygy.h"
#endif
#include "tester.h"
#include "threadp.h"
#ifdef TUNE
#include "tune.h"
#endif
#ifdef UNIT_TESTS
#include "unit.h"
#endif

#include <cstddef>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <regex>
#include <unordered_set>

const char * Protocol::UCI_DEBUG_PREFIX = "info ";
const char * Protocol::CECP_DEBUG_PREFIX = "# ";

using namespace std::placeholders;

Protocol::Protocol(const Board &board, bool traceOn, bool icsMode, bool cpus_set, bool memory_set)
    : verbose(false),
      post(false),
      searcher(nullptr),
      last_move(NullMove),
      last_move_image("(null)"),
      last_computer_move(NullMove),
      game_file(nullptr),
      time_left(0),
      opp_time(0),
      minutes(5.0),
      incr(0),
      winc(0),
      binc(0),
      computer(false),
      computer_plays_white(false),
      ics(icsMode),
      forceMode(false),
      analyzeMode(false),
      editMode(false),
      side(White),
      moves(0),
      ponder_board(new Board()),
      main_board(new Board(board)),
      ponder_status(PonderStatus::None),
      predicted_move(NullMove),
      ponder_move(NullMove),
      best_move(NullMove),
      time_target(0),
      last_time_target(INFINITE_TIME),
      computer_rating(0),
      opponent_rating(0),
      doTrace(traceOn),
      easy(false),
      game_end(false),
      result_pending(false),
      last_score(Constants::MATE),
      ecoCoder(nullptr),
      xboard42(false),
      srctype(TimeLimit),
      time_limit(INFINITE_TIME),
      ply_limit(Constants::MaxPly),
      uci(false),
      movestogo(0),
      ponderhit(false),
      uciWaitState(false),
      cpusSet(cpus_set),
      memorySet(memory_set)
{
    ecoCoder = new ECO();
    searcher = new SearchController();
    searcher->registerPostFunction(std::bind(&Protocol::post_output,this,_1));
    searcher->registerMonitorFunction(std::bind(&Protocol::monitor,this,_1,_2));

    if (options.store_games) {
        if (options.game_pathname == "") {
            game_pathname = derivePath("games.pgn");
        }
        else {
            game_pathname = options.game_pathname;
        }
        game_file = new ofstream(game_pathname.c_str(),ios::out | ios::app);
        if (!game_file->good()) {
            cerr << "Warning: cannot open game file. Games will not be saved." << endl;
            delete game_file;
            game_file = nullptr;
        }
    }
}

Protocol::~Protocol()
{
    delete main_board;
    delete ponder_board;
    delete ecoCoder;
    delete searcher;

    if (game_file) {
        game_file->close();
        delete game_file;
    }
}

void Protocol::poll(bool &polling_terminated)
{
    while(!polling_terminated) {
        if (inputSem.wait()) {
            break;
        }
        while (!polling_terminated) {
            Lock(input_lock);
            string cmd;
            if (hasPending()) {
                cmd = pending.front();
                pending.erase(pending.begin());
            } else {
                Unlock(input_lock);
                break;
            }
            if (doTrace) {
                cout << debugPrefix() << "got cmd (main): "  << cmd << endl;
            }
            Unlock(input_lock);
            if (doTrace) cout << debugPrefix() << "calling do_command(main):" << cmd << (flush) << endl;
            if (!do_command(cmd,*main_board)) {
                if (doTrace) cout << debugPrefix() << "exiting polling loop" << endl;
                polling_terminated = true;
            }
        }
    }
    // handle termination.
    save_game();
    if (doTrace) {
        cout << debugPrefix() << "terminating" << endl;
    }
}

void Protocol::add_pending(const string &cmd) {
    Lock(input_lock);
    pending.push_back(cmd);
    Unlock(input_lock);
}

void Protocol::split_cmd(const string &cmd, string &cmd_word, string &cmd_args) {
   size_t space = cmd.find_first_of(' ');
   cmd_word = cmd.substr(0,space);
   cmd_args = cmd.substr(cmd_word.length());
   size_t start = cmd_args.find_first_not_of(' ');
   if (start != string::npos) {
      cmd_args.erase(0,start);
   }
}

Move Protocol::text_to_move(const Board &board, const string &input) {
   // Try SAN
   Move m = Notation::value(board,board.sideToMove(),Notation::InputFormat::SAN,input);
   if (!IsNull(m)) return m;
   if (input.length() >= 4 && isalpha(input[0]) && isdigit(input[1]) &&
       isalpha(input[2]) && isdigit(input[3])) {
      // This appears to be "old" coordinate style notation, used in
      // Winboard before 4.2
       return Notation::value(board,board.sideToMove(),Notation::InputFormat::WB,input);
   } else {
      return NullMove;
   }
}

Move Protocol::get_move(const string &cmd_word, const string &cmd_args) {
    string move;
    if (cmd_word == "usermove") {
        // new for Winboard 4.2
        move = cmd_args;
    } else {
        move = cmd_word;
    }
    // see if it could be a move
    auto it = move.begin();
    while (it != move.end() && !isalpha(*it)) it++;
    move.erase(move.begin(),it);
    if (doTrace) {
        cout << debugPrefix() << "move text = " << move << endl;
    }
    return text_to_move(*main_board,move);
}

Protocol::AllPendingStatus Protocol::do_all_pending(Board &board)
{
    AllPendingStatus retVal = AllPendingStatus::Nothing;
    if (doTrace) cout << debugPrefix() << "in do_all_pending" << endl;
    while (true) {
        Lock(input_lock);
        if (pending.empty()) {
           Unlock(input_lock);
           break;
        }
        string cmd(pending.front());
        pending.erase(pending.begin());
        Unlock(input_lock);
        if (doTrace) {
            cout << debugPrefix() << "pending command(a): " << cmd << endl;
        }
        if (cmd == "quit") {
            retVal = AllPendingStatus::Quit;
            break;
        }
        do_command(cmd,board);
    }
    if (doTrace) {
        cout << debugPrefix() << "out of do_all_pending, list size=" << pending.size() << endl;
    }
    return retVal;
}

Protocol::PendingStatus Protocol::check_pending(Board &board) {
    if (doTrace) cout << debugPrefix() << "in check_pending" << endl;
    PendingStatus retVal = PendingStatus::Nothing;
    Lock(input_lock);
    while (!pending.empty()) {
        const string cmd(pending.front());
        string cmd_word, cmd_args;
        split_cmd(cmd,cmd_word,cmd_args);
        if (cmd_word == "result" ||
            cmd == "new" ||
            cmd == "quit" ||
            cmd == "resign") {
                if (doTrace) cout << debugPrefix() << "game end signal in pending stack" << endl;
                retVal = PendingStatus::GameEnd;
                break;
        }
        else if (cmd_word == "usermove" || text_to_move(board,cmd) != NullMove) {
            if (doTrace) cout << debugPrefix() << "move in pending stack" << endl;
            retVal = PendingStatus::Move;
            break;
        }
        else {  // might as well execute this
            if (doTrace) {
                cout << debugPrefix() << "calling do_command from check_pending" << (flush) << endl;
            }
            // remove command from pending stack
            pending.erase(pending.begin());
            // execute command
            do_command(cmd,board);
        }
    }
    Unlock(input_lock);
    return retVal;
}

void Protocol::parseLevel(const string &cmd, int &moves, float &minutes, int &incr)
{
    // not exact for decimal values, so must still check for validity
    const auto pattern = std::regex("^(\\d+)\\s+(\\d+)(\\:([\\d\\.]+))?\\s+([\\d\\.]+)$");

    std::smatch match;
    string movesStr, minutesStr, secondsStr, incStr;
    if (std::regex_match(cmd,match,pattern)) {
        // first result is the whole string matched
        auto it = match.begin()+1;
        movesStr = *it++;
        minutesStr = *it++;
        if (match.size() > 4) {
            it++; // outer group
            secondsStr = *it++; // inner group has seconds
        }
        incStr = *it;
    } else {
        cerr << "error parsing level command." << endl;
        return;
    }

    float time1, time2, floatincr;

    stringstream nums(minutesStr);
    nums >> time1;
    if (nums.bad()) {
       cerr << "error in time field" << endl;
       return;
    }
    else if (secondsStr.size()) {
        stringstream nums2(secondsStr);
        nums2 >> time2;
        if (nums2.bad()) {
            cerr << "error in time field" << endl;
            return;
        } else {
            minutes = time1 + time2/60;
        }
    } else {
        minutes = time1;
    }
    stringstream movesStream(movesStr);
    movesStream >> moves;
    if (movesStream.bad()) {
        cerr << "error in move field" << endl;
        return;
    }

    stringstream incStream(incStr);
    incStream >> floatincr;
    if (incStream.bad()) {
        cerr << "error in increment field" << endl;
    } else {
        // Winboard increment is in seconds, convert to our
        // internal value (milliseconds).
        incr = int(1000*floatincr);
    }
}

void Protocol::dispatchCmd(const string &cmd) {
    if (uciWaitState) {
        // We are in the wait state (meaning we had already stopped
        // searching), see if we should exit it now.
        processCmdInWaitState(cmd);
    }
    else {
        // Do not execute the command within the polling thread.
        // Add it to the pending stack.
        add_pending(cmd);
    }
}

void Protocol::sendPong(const string &arg)
{
    cout << "pong " << arg << endl << (flush);
}

void Protocol::process_st_command(const string &cmd_args)
{
   stringstream s(cmd_args);
   float time_limit_sec;
   // we allow fractional seconds although UI may not support it
   s >> time_limit_sec;
   if (s.bad() || time_limit_sec <= 0.0) {
      cout << debugPrefix() << "illegal value for st command: " << cmd_args << endl;
      return;
   } else {
      srctype = FixedTime;
   }
   // convert to ms. and subtract a buffer to prevent losses on time
   time_limit = int(time_limit_sec * 1000 - std::min<int>(int(time_limit_sec*100),100));
}

score_t Protocol::contemptFromRatings(int computer_rating,int opponent_rating) {
   const int rdiff = computer_rating-opponent_rating;
   return static_cast<score_t>(4*Params::PAWN_VALUE*(1.0/(1.0+exp(-rdiff/400.0)) - 0.5));
}

int Protocol::getIncrUCI(const ColorType side) {
    return side == White ? winc : binc;
}

bool Protocol::accept_draw(Board &board) {
   if (doTrace)
      cout << debugPrefix() << "in accept_draw" << endl;
   // Code to handle draw offers.
   int rating_diff = opponent_rating - computer_rating;
   // ignore draw if we have just started searching
   if (last_score == Constants::MATE) {
      return false;
   }
   // If it's a 0-increment game and the opponent has < 1 minute,
   // and we have more time, decline
   int inc = uci ? getIncrUCI(board.oppositeSide()) : incr;
   if (!computer && inc == 0 && opp_time < 6000 && time_left > opp_time) {
      return false;
   }
   // See if we do not have enough material to mate
   const ColorType side = computer_plays_white ? White : Black;
   const Material &ourmat = board.getMaterial(side);
   const Material &oppmat = board.getMaterial(OppositeColor(side));
   if (ourmat.noPawns() && (ourmat.kingOnly() || ourmat.infobits() == Material::KB ||
                            ourmat.infobits() == Material::KN)) {
      // We don't have mating material
      if (doTrace)
         cout << debugPrefix() << "no mating material, accept draw" << endl;
      return true;
   }
   // accept a draw in pawnless endings with even material, unless
   // our score is way positive
   if (ourmat.noPawns() && oppmat.noPawns() &&
       ourmat.materialLevel() <= 5 &&
      last_score < Params::PAWN_VALUE) {
      if (doTrace)
         cout << debugPrefix() << "pawnless ending, accept draw" << endl;
      return true;
   }
#ifdef SYZYGY_TBS
   const Material &wMat = board.getMaterial(White);
   const Material &bMat = board.getMaterial(Black);
   if(options.search.use_tablebases &&
      wMat.men() + bMat.men() <= EGTBMenCount) {
      if (doTrace)
         cout << debugPrefix() << "checking tablebases .." << endl;
      // accept a draw when the tablebases say it's a draw
      score_t tbscore;
      if (SyzygyTb::probe_wdl(board,tbscore,true) && std::abs(tbscore) <= SyzygyTb::CURSED_SCORE) {
         if (doTrace) {
            cout << debugPrefix() << "tablebase score says draw" << endl;
         }
         return true;
      }
   }
#endif
   if (opponent_rating == 0)
      return false;
   // accept a draw if our score is negative .. how much negative
   // depends on opponent rating.
   if (doTrace)
      cout << debugPrefix() << "checking draw score .." << endl;
   ColorType tmp = board.sideToMove();
   board.setSideToMove(side);
   score_t draw_score = searcher->drawScore(board);
   board.setSideToMove(tmp);
   const score_t threshold = Params::PAWN_VALUE/4;
   if (doTrace) {
      cout << debugPrefix() << "rating_diff = " << rating_diff << endl;
      cout << debugPrefix() << "draw_score = " << draw_score << endl;
      cout << debugPrefix() << "last_score = " << last_score << endl;
      cout << debugPrefix() << "threshold = " << threshold << endl;
   }
   return draw_score > threshold && last_score <= draw_score;
}


void Protocol::do_help() {
#ifndef _WIN32
   if (!isatty(1)) return;
#endif
   cout << "analyze:         enter Winboard analyze mode" << endl;
   cout << "black:           set computer to play Black" << endl;
   cout << "bk:              show book moves" << endl;
   cout << "computer:        used to indicate the opponent is a computer" << endl;
   cout << "draw:            offer a draw" << endl;
   cout << "easy:            disable pondering" << endl;
   cout << "edit:            enter Winboard edit mode" << endl;
   cout << "force:           disable computer moving" << endl;
   cout << "go:              start searching" << endl;
   cout << "hard:            enable pondering" << endl;
   cout << "hint:            compute a hint for the current position" << endl;
   cout << "ics <hostname>:  set the name of the ICS host" << endl;
   cout << "level <a b c>:   set the time control:" << endl;
   cout << "  a -> moves to time control" << endl;
   cout << "  b -> minutes per game" << endl;
   cout << "  c -> increment in seconds" << endl;
   cout << "name <string>:   set the name of the opponent" << endl;
   cout << "new:             start a new game" << endl;
   cout << "nopost:          disable output during search" << endl;
   cout << "otim <int>:      set opponent time remaining (in centiseconds)" << endl;
   cout << "post:            show output during search" << endl;
   cout << "quit:            terminate the program" << endl;
   cout << "remove:          back up a full move" << endl;
   cout << "resign:          resign the current game" << endl;
   cout << "result <string>: set the game result (0-1, 1/2-1/2 or 1-0)" << endl;
   cout << "sd <x>:          limit thinking to depth x" << endl;
   cout << "setboard <FEN>:  set board to a specified FEN string" << endl;
   cout << "st <x>:          limit thinking to x seconds" << endl;
   cout << "test <epd_file> -d <depth> -t <sec/move> <-x iter> <-N pvs> <-v>:  run test suite" << endl;
   cout << "time <int>:      set computer time remaining (in centiseconds)" << endl;
   cout << "undo:            back up a half move" << endl;
   cout << "white:           set computer to play White" << endl;
   cout << "test <file> <-t seconds> <-x # moves> <-v> <-o outfile>: "<< endl;
   cout << "   - run an EPD testsuite" << endl;
   cout << "eval <file>:     evaluate a FEN position." << endl;
   cout << "perft <depth>:   compute perft value for a given depth" << endl;
}


void Protocol::save_game() {
   if (uci) return;                               // not supported
   if (doTrace) cout << debugPrefix() << "in save_game" << endl;
   if (doTrace) cout << debugPrefix() << "game_moves=" << gameMoves->num_moves() << endl;
   if (gameMoves->num_moves() == 0 || !options.store_games) {
      if (doTrace) cout << debugPrefix() << "out of save_game" << endl;
      return;
   }
   if (game_file) {
      vector<ChessIO::Header> headers;
      string opening_name, eco;
      if (ecoCoder) {
         if (doTrace) cout << debugPrefix() << "calling classify" << endl;
         ecoCoder->classify(*gameMoves,eco,opening_name);
         headers.push_back(ChessIO::Header("ECO",eco));
      }
      char crating[15];
      char orating[15];
      sprintf(crating,"%d",computer_rating);
      sprintf(orating,"%d",opponent_rating);

      if (hostname.length() > 0) {
          headers.push_back(ChessIO::Header("Site",hostname));
      }

      if (computer_plays_white) {
         headers.push_back(ChessIO::Header("Black",
            opponent_name.length() > 0 ? opponent_name : "?"));
         if (computer_rating)
            headers.push_back(ChessIO::Header("WhiteElo",crating));
         if (opponent_rating)
            headers.push_back(ChessIO::Header("BlackElo",orating));
      }
      else {
         headers.push_back(ChessIO::Header("White",
            opponent_name.length() > 0 ? opponent_name : "?"));
         if (opponent_rating)
            headers.push_back(ChessIO::Header("WhiteElo",orating));
         if (computer_rating)
            headers.push_back(ChessIO::Header("BlackElo",crating));
      }
      if (start_fen.size()) {
         // we had a non-standard starting position for the game
          headers.push_back(ChessIO::Header("FEN",start_fen));
      }
      stringstream timec;
      if (moves != 0) {
          timec << moves << '/';
      }
      timec << minutes*60;
      string timestr = timec.str();
      if (incr) {
         timec << '+' << fixed << setprecision(2) << incr/1000.0F;
         timestr = timec.str();
         size_t per = timestr.find('.');
         if (per != string::npos) {
             int eraseCount = 0;
             // remove trailing zeros and decimal pt if possible:
             for (auto it = timestr.end()-1; it != timestr.begin(); it--) {
                 if (*it == '0' || *it == '.') {
                     ++eraseCount;
                     if (*it == '.') break;
                 }
                 else {
                     break;
                 }
             }
             if (eraseCount) {
                 timestr.erase(timestr.size()-eraseCount,eraseCount);
             }
         }
      } else {
          timestr = timec.str();
      }
      headers.push_back(ChessIO::Header("TimeControl",timestr));
      string result;
      theLog->getResultAsString(result);
      ChessIO::store_pgn(*game_file, *gameMoves,
         computer_plays_white ? White : Black,
         result,
         headers);
   }
   if (doTrace) cout << debugPrefix() << "out of save_game" << endl;
}

void Protocol::move_image(const Board &board, Move m, ostream &buf, bool uci) {
    Notation::image(board,m,uci ? Notation::OutputFormat::UCI : Notation::OutputFormat::WB,buf);
}

void Protocol::uciOut(int depth, score_t score, time_t time,
uint64_t nodes, uint64_t tb_hits, const string &best_line_image, int multipv) {
   stringstream s;
   s << "info";
   s << " multipv " << (multipv == 0 ? 1 : multipv);
   s << " depth " << depth << " score ";
   Scoring::printScoreUCI(score,s);
   if (stats.failHigh) s << " lowerbound";
   if (stats.failLow) s << " upperbound";
   s << " time " << time << " nodes " << nodes;
   if (time>300) s << " nps " << (long)((1000L*nodes)/time);
   if (tb_hits) {
      s << " tbhits " << tb_hits;
   }
   s << " hashfull " << searcher->hashTable.pctFull();
   if (best_line_image.length()) {
      s << " pv ";
      s << best_line_image;
   }
   cout << s.str() << endl;
   if (doTrace) {
      theLog->write(s.str().c_str()); theLog->write_eol();
   }
}


void Protocol::uciOut(const Statistics &stats) {
   uciOut(stats.depth,stats.display_value,searcher->getElapsedTime(),
      stats.num_nodes,stats.tb_hits,
      stats.best_line_image,0);
}


void Protocol::post_output(const Statistics &stats) {
   last_score = stats.value;
   score_t score = stats.display_value;
   if (score == Constants::INVALID_SCORE) {
      return; // no valid score yet
   }
   if (verbose) {
       if (uci) {
           if (options.search.multipv > 1) {
               // output stats only when multipv array has been filled
               if (stats.multipv_count == stats.multipv_limit) {
                   for (unsigned i = 0; i < stats.multipv_limit; i++) {
                       uciOut(stats.multi_pvs[i].depth,
                              stats.multi_pvs[i].display_value,
                              searcher->getElapsedTime(),
                              stats.num_nodes,
                              stats.tb_hits,
                              stats.multi_pvs[i].best_line_image,
                              i+1);
                   }
               }
           }
           else {
               uciOut(stats);
           }
       }
   }
   else if (post) {
       // "post" output for Winboard
       cout << stats.depth << ' ' <<
           static_cast<int>((score*100)/Params::PAWN_VALUE) << ' ' <<
           searcher->getElapsedTime()/10 << ' ' <<
           stats.num_nodes << ' ' <<
           stats.best_line_image << endl << (flush);
   }
}

void Protocol::checkPendingInSearch(SearchController *controller) {
    // Except for the special case of the UCI wait state, all commands
    // received are queued in the pending stack. Examine the queue
    // here in order. Some commands will be executed and removed from
    // the stack. The rest may trigger events such as search
    // termination, but their actual execution is delayed until after
    // search completion.
    // Note: typically the pending stack is very small during search.
    Lock(input_lock);
    auto it = pending.begin();
    bool exit = false;
    while (it != pending.end() && !exit) {
        if (processPendingInSearch(controller,*it,exit)) {
            it = pending.erase(it);
        } else {
            it++;
        }
    }
    Unlock(input_lock);
}

bool Protocol::processPendingInSearch(SearchController *controller, const string &cmd, bool &exit)
{
    if (doTrace) {
        cout << debugPrefix() << "command in search: " << cmd << endl;
    }
    string cmd_word, cmd_args;
    // extract first word of command:
    split_cmd(cmd,cmd_word,cmd_args);
    exit = false;
    if (uci) {
        if (cmd == "quit") {
            controller->terminateNow();
            exit = true;
            return false;
        }
        else if (cmd == "ponderhit") {
            // We predicted the opponent's move, so we need to
            // continue doing the ponder search but adjust the time
            // limit.
            ponderhit = true;
            ponder_status = PonderStatus::Hit;
            // continue the search in non-ponder mode
            if (srctype != FixedDepth) {
                if (doTrace) {
                    stringstream s;
                    s << debugPrefix() << " time_limit=" << time_limit << " movestogo=" <<
                        movestogo << " time_left=" << time_left << " opp_time=" << opp_time << '\0';
                    cout << s.str() << endl << (flush);
                }
                // Compute how much longer we must search
                timeMgmt::Times times;
                calcTimes(true,controller->getComputerSide(),times);
                time_target = last_time_target = times.time_target;
                controller->setTimeLimit(times.time_target,times.extra_time);
            }
            controller->setTalkLevel(TalkLevel::Whisper);
            controller->setBackground(false);
            // Since we have shifted to foreground mode, show the current
            // search statistics:
            post_output(ponder_stats);
            return true;
        }
        else if ((cmd_word == "position" ||
                  cmd == "ucinewgame")) {
            // These commands should end the search - we
            // need to prepare to search a new position.
            controller->terminateNow();
            return false;
        } else {
            // Most other commands do not terminate the search. Execute them
            // now. (technically, according the UCI spec, setoption is not
            // allowed during search: but UIs such as ChessBase assume it is).
            Board &board = controller->pondering() ? *ponder_board : *main_board;
            exit = do_command(cmd,board);
            return true;
        }
    }
    else if (analyzeMode) {
        if (cmd == "undo" || cmd == "setboard") {
            controller->terminateNow();
            return false;
        }
        else if (cmd == "exit") {
            controller->terminateNow();
            analyzeMode = false;
            return true;
        }
        else if (cmd == "bk") {
            do_command(cmd, *main_board);
            return true;
        }
        else if (cmd == ".") {
            analyze_output(stats);
            return true;
        }
        else if (cmd_word == "usermove" || text_to_move(*main_board,cmd) != NullMove) {
            controller->terminateNow();
            return false;
        }
        // all other commands are ignored but remain in the
        // pending stack
    }
    else if (cmd == "?") {
        // Winboard 3.6 or higher sends this to terminate a search
        // in progress
        if (doTrace) cout << debugPrefix() << "? received: terminating." << endl;
        controller->terminateNow();
        return true;
    }
    else if (cmd_word == "ping") {
        // new for Winboard 4.2
        // The protocol requires an immediate response if we are
        // pondering. However, if a command to terminate the search
        // has already been received but the ponder search is still going,
        // queue the "ping" command until the ponder search is
        // actually terminated.
        if (controller->pondering() && ponder_stats.state != Terminated) {
            sendPong(cmd_args);
            return true;
        } else {
            return false;
        }
    }
    else if (cmd == "quit" || cmd == "end") {
        controller->terminateNow();
        // don't process any commands after quit
        exit = true;
        // keep "quit" on the stack
        return false;
    }
    else if (cmd == "hint" ||
             cmd_word == "level" ||
             cmd_word == "st" ||
             cmd_word == "sd" ||
             cmd_word == "time" ||
             cmd_word == "otim" ||
             cmd_word == "rating" ||
             cmd_word == "option" ||
             cmd_word == "name" ||
             cmd == "computer" ||
             cmd == "post" ||
             cmd == "nopost" ||
             cmd == "draw" ||
             cmd == "easy" ||
             cmd == "hard") {
        // Some of these commands are not expected during a search
        // but we process them anyway. They do not stop the search.
        do_command(cmd,controller->pondering() ? *ponder_board : *main_board);
        return true;
    }
    else if (cmd == "resign" || cmd_word == "result") {
        game_end = true;
        if (doTrace) {
            cout << debugPrefix() << "received_result: " << cmd << endl;
        }
        controller->terminateNow();
        // set the state to Terminated - this is a signal that
        // regardless of the search result, we should not send
        // a move, because the UI has terminated the game.
        if (controller->pondering()) {
            ponder_stats.state = Terminated;
        } else {
            stats.state = Terminated;
        }
        // Stop processing commands in the search monitor loop.
        // Queued commands will be processed after search completion.
        exit = true;
        return false;
    }
    else if (cmd == "new" || cmd == "test" || cmd == "bench" ||
             cmd == "edit" || cmd == "remove" || cmd == "undo" ||
             cmd_word == "setboard" || cmd == "analyze" ||
             cmd == "go" || cmd == "exit" || cmd == "white" ||
             cmd == "black" || cmd == "playother") {
        // These commands terminate the search. They are processed
        // after search completion.
        controller->terminateNow();
        return false;
    }
    else if (cmd == "force") {
        forceMode = true;
        controller->terminateNow();
        return true;
    }
    else if (cmd_word == "ics") {
        // ICC can send during a search
        hostname = cmd_args;
        return true;
    }
    else {
        // Try to parse command as a move
        Move rmove = get_move(cmd_word, cmd_args);
        if (IsNull(rmove)) {
            if (doTrace) {
                cout << debugPrefix() << "cmd in search not processed: " << cmd << " (expected move)";
            }
            return false;
        } else {
            last_move = rmove;
            if (doTrace) {
                cout << debugPrefix() << "predicted move = ";
                MoveImage(predicted_move,cout);
                cout << " last move = ";
                MoveImage(last_move,cout);
                cout << endl;
            }
            if (forceMode || analyzeMode || !controller->pondering()) {
                controller->terminateNow();
                return false;
            }
            else if (!IsNull(predicted_move) && MovesEqual(predicted_move,last_move)) {
                // A move arrived during a ponder search and it was
                // the predicted move, in other words we got a ponder hit.
                if (doTrace) {
                    cout << debugPrefix() << "ponder ok" << endl;
                }
                execute_move(*main_board,last_move);
                // We predicted the opponent's move, so we need to
                // continue doing the ponder search but adjust the time
                // limit.
                ponder_status = PonderStatus::Hit;
                if (srctype != FixedDepth) {
                    timeMgmt::Times times;
                    calcTimes(true,controller->getComputerSide(),times);
                    time_target = last_time_target = times.time_target;
                    controller->setTimeLimit(times.time_target,times.extra_time);
                }
                controller->setTalkLevel(TalkLevel::Whisper);
                controller->setBackground(false);
                post_output(ponder_stats);
                return true;
            }
            else {
                if (doTrace) cout << debugPrefix() << "ponder not ok" << endl;
                // We can't use the results of pondering because we
                // did not predict the opponent's move.  Stop the
                // search and then execute the move.
                ponder_status = PonderStatus::NoHit;
                controller->terminateNow();
                return false;
            }
        }
    }
    return false;
}

int Protocol::monitor(SearchController *s, const Statistics &) {
    checkPendingInSearch(s);
    return 0;
}

void Protocol::edit_mode_cmds(Board &board,ColorType &side,const string &cmd)
{
    unordered_set<char> pieces({'P','N','B','R','Q','K','p','n','b','r','q','k'});
    if (cmd == "white") {
       side = White;
    }
    else if (cmd == "black") {
       side = Black;
    }
    else if (cmd == "#") {
       for (int i = 0; i < 64; i++) {
          board.setContents(EmptyPiece,i);
       }
    }
    else if (cmd == "c") {
       side = OppositeColor(side);
    }
    else if (cmd == ".") {
       editMode = false;
       board.setSecondaryVars();
       // edit doesn't set the castle status, so try to deduce it
       // from the piece positions
       if (board.kingSquare(White) == chess::E1) {
          if (board[chess::A1] == WhiteRook &&
              board[chess::H1] == WhiteRook)
             board.setCastleStatus(CanCastleEitherSide,White);
          else if (board[chess::A1] == WhiteRook)
             board.setCastleStatus(CanCastleQSide,White);
          else if (board[chess::H1] == WhiteRook)
             board.setCastleStatus(CanCastleKSide,White);
       }
       else
          board.setCastleStatus(CantCastleEitherSide,White);
       if (board.kingSquare(Black) == chess::E8) {
          if (board[chess::A8] == BlackRook &&
              board[chess::H8] == BlackRook)
             board.setCastleStatus(CanCastleEitherSide,Black);
          else if (board[chess::A8] == BlackRook)
             board.setCastleStatus(CanCastleQSide,Black);
          else if (board[chess::H8] == BlackRook)
             board.setCastleStatus(CanCastleKSide,Black);
       }
       else {
          board.setCastleStatus(CantCastleEitherSide,Black);
       }
    }
    else {
       if (cmd.length()>0 && pieces.count(cmd[0])) {
          char c = tolower(cmd[0]);
          Piece p = EmptyPiece;
          switch (c) {
          case 'p':
             p = MakePiece(Pawn,side);
             break;
          case 'n':
             p = MakePiece(Knight,side);
             break;
          case 'b':
             p = MakePiece(Bishop,side);
             break;
          case 'r':
             p = MakePiece(Rook,side);
             break;
          case 'q':
             p = MakePiece(Queen,side);
             break;
          case 'k':
             p = MakePiece(King,side);
             break;
          default:
             break;
          }
          if (p != EmptyPiece) {
             Square sq = SquareValue(cmd.substr(1));
             if (!IsInvalid(sq)) {
                board.setContents(p,sq);
             }
          }
       }
    }
}

void Protocol::calcTimes(bool pondering, ColorType side, timeMgmt::Times &times) 
{
    if (srctype == FixedTime) {
        times.time_target = time_limit;
        times.extra_time = 0;
    } else if (uci) {
        timeMgmt::calcTimeLimitUCI(movestogo,
                                   getIncrUCI(side),
                                   time_left,
                                   pondering,
                                   ics,
                                   times);
    } else {
        timeMgmt::calcTimeLimit(moves, incr, time_left, pondering, ics, times);
    }
    if (doTrace) {
        cout << debugPrefix() << "time_target = " << times.time_target << endl;
        cout << debugPrefix() << "xtra time = " << times.extra_time << endl;
    }
}

void Protocol::ponder(Board &board, Move move, Move predicted_reply, bool uci)
{
    ponder_status = PonderStatus::None;
    ponder_move = NullMove;
    ponder_stats.clear();
    if (doTrace) {
       cout << debugPrefix() << "in ponder(), move = ";
       MoveImage(move,cout);
       cout << " predicted reply = ";
       MoveImage(predicted_reply,cout);
       cout << endl;
    }
    if (uci || (!IsNull(move) && !IsNull(predicted_reply))) {
        if (!uci) {
            predicted_move = predicted_reply;
            // We have already set up the ponder board with the board
            // position after our last move. Now make the move we predicted.
            //
            BoardState previous_state = ponder_board->state;
            ASSERT(legalMove(*ponder_board,predicted_reply));
            ponder_board->doMove(predicted_reply);
            //
            // We must also add this move to the global move list,
            // otherwise repetition detection will be broken. Note,
            // though, that in case of a ponder miss we must later
            // remove this move.
            //
            gameMoves->add_move(board,previous_state,predicted_reply,"",true);
        }
        time_target = INFINITE_TIME;
        // in reduced strength mode, limit the ponder search time
        // (do not ponder indefinitely)
        if (options.search.strength < 100) {
            time_target = last_time_target;
            if (doTrace) cout << debugPrefix() << "limiting ponder time to " <<
                             time_target << endl;
        }
        if (doTrace) {
            cout << debugPrefix() << "starting to ponder" << endl;
        }
        ponder_status = PonderStatus::Pending;
        if (srctype == FixedDepth) {
            ponder_move = searcher->findBestMove(
                uci ? *main_board : *ponder_board,
                srctype,
                0,
                0,
                ply_limit, true, uci,
                ponder_stats,
                (doTrace) ? TalkLevel::Debug : TalkLevel::Silent);
        }
        else {
            time_target = last_time_target = INFINITE_TIME;
            ponder_move = searcher->findBestMove(
                uci ? *main_board : *ponder_board,
                FixedTime,
                time_target,
                0,            /* extra time allowed */
                Constants::MaxPly,           /* ply limit */
                true,         /* background */
                uci,
                ponder_stats,
                (doTrace) ? TalkLevel::Debug : TalkLevel::Silent);
        }
        if (doTrace) {
            cout << debugPrefix() << "done pondering" << endl;
        }
        // Ensure "ping" response is set if ping was received while
        // searching:
        if (!uci) {
            checkPendingInSearch(searcher);
        }
    }
    last_computer_move = ponder_move;
    last_computer_stats = ponder_stats;
    // Clean up the global move array, if we got no ponder hit.
    if (!uci && gameMoves->num_moves() > 0 && gameMoves->last().wasPonder()) {
        gameMoves->remove_move();
    }
    if (doTrace) {
        cout << debugPrefix() << "ponder move = ";
        MoveImage(ponder_move,cout);
        cout << endl;
        cout << debugPrefix() << "out of ponder()" << endl;
    }
}

Move Protocol::search(SearchController *searcher, Board &board,
                   const MoveSet &movesToSearch, Statistics &stats, bool infinite)
{
    last_stats.clear();
    last_score = Constants::MATE;
    ponder_move = NullMove;
    if (doTrace) cout << debugPrefix() << "in search()" << endl;

    Move move = NullMove;
    stats.fromBook = false;
    // Note: not clear what the semantics should be when "searchmoves"
    // is specified and using "own book." Currently we force a search
    // in this case and ignore the book moves.
    if (!infinite && options.book.book_enabled && movesToSearch.empty()) {
        move = openingBook.pick(board);
        if (!IsNull(move)) stats.fromBook = true;
    }

    if (IsNull(move)) {
        // no book move
        stats.clear();
        TalkLevel level = TalkLevel::Silent;
        if (verbose && !uci) {
           level = TalkLevel::Test;
        }
        else if (doTrace) {
           level = TalkLevel::Debug;
        }
        MoveSet excludes;
        if (srctype == FixedDepth) {
            move = searcher->findBestMove(board,
                srctype,
                0,
                0,
                ply_limit, false, uci,
                stats,
                level,
                excludes,
                movesToSearch);
        }
        else {
            timeMgmt::Times times;
            if (infinite) {
                times.time_target = INFINITE_TIME;
                times.extra_time = 0;
            } else {
                if (doTrace) cout << debugPrefix() << " time_limit=" << time_limit << " movestogo=" <<
                                 movestogo << endl;
                calcTimes(false,board.sideToMove(),times);
            }
            time_target = last_time_target = times.time_target;
            move = searcher->findBestMove(board,
                srctype,
                times.time_target,
                times.extra_time,
                Constants::MaxPly, false, uci,
                stats,
                level,
                excludes,
                movesToSearch);
        }
        if (doTrace) {
            cout << debugPrefix() << "search done : move = ";
            MoveImage(move,cout);
            cout << endl;
        }
        last_stats = stats;
    }
    else {
        if (ics || uci) {
            vector< Move > choices;
            int moveCount = 0;
            if (options.book.book_enabled) {
               moveCount = openingBook.book_moves(board,choices);
            }
            stringstream s;
            s << "book moves (";
            for (int i=0;i<moveCount;i++) {
                Notation::image(board,choices[i],Notation::OutputFormat::SAN,s);
                if (i < moveCount-1)
                    s << ", ";
            }
            s << "), choosing ";
            Notation::image(board,move,Notation::OutputFormat::SAN,s);
            if (uci) {
                cout << "info string " << s.str() << endl;
            }
            if (ics) {
                if (computer)
                    cout << "tellics kibitz " << s.str() << endl;
                else
                    cout << "tellics whisper " << s.str() << endl;
            }
        }
        stats.clear();
    }
    last_computer_move = move;
    last_computer_stats = stats;
    return move;
}

int Protocol::isDraw(const Board &board, Statistics &last_stats, string &reason) {
   if (last_stats.state == Stalemate) {
       if (doTrace) cout << debugPrefix() << "stalemate" << endl;
       reason = "Stalemate";
       return 1;
   }
   else if (last_stats.value < Constants::MATE-1 &&
            board.state.moveCount >= 100) {
       // Note: do not count as draw if we have checkmated opponent!
       if (doTrace) cout << debugPrefix() << "50 move draw" << endl;
       reason = "50 move draw";
       return 1;
   }
   else if (board.materialDraw()) {
       if (doTrace) cout << debugPrefix() << "material draw" << endl;
       reason = "Insufficient material";
       return 1;
   }
   else if (board.repetitionDraw()) {
       if (doTrace) cout << debugPrefix() << "repetition draw" << endl;
       reason = "Repetition";
       return 1;
   }
   else if (stats.state == Draw) {
       return 1;
   }
   return 0;
}

static void kibitz(SearchController *searcher, bool computer, Statistics &last_stats, bool multithread) {
    stringstream s;
    if (computer)
        s << "tellics kibitz ";
    else
        s << "tellics whisper ";
    s << "time=" << fixed << setprecision(2) <<
        (float)searcher->getElapsedTime()/1000.0 << " sec. score=";
    Scoring::printScore(last_stats.display_value,s);
    s << " depth=" << last_stats.depth;
    if (searcher->getElapsedTime() > 0) {
        s << " nps=";
        Statistics::printNPS(s,last_stats.num_nodes,searcher->getElapsedTime());
    }
    if (last_stats.tb_hits) {
        s << " egtb=" << last_stats.tb_hits << '/' << last_stats.tb_probes;
    }
#if defined(SMP_STATS)
    if (multithread) {
        s << " cpu=" << fixed << setprecision(2) <<
            searcher->getCpuPercentage() << '%';
    }
#endif
    if (last_stats.best_line_image.length()) {
        s << " pv: " << last_stats.best_line_image;
    }
    cout << s.str() << endl;
}

void Protocol::send_move(Board &board, Move &move, Statistics
                         &stats) {
    // In case of multi-pv, make sure the high-scoring move is
    // sent as best move.
    if (stats.multipv_limit > 1) {
        move = stats.multi_pvs[0].best;
    }
    last_move = move;
    last_stats = stats;
    ColorType sideToMove = board.sideToMove();
    if (stats.state == Terminated) {
        // A ponder search was interrupted because Winboard set the
        // game result. We should not send a move or try to set the result.
        return;
    }
    else if (!game_end) {
        if (!uci) {
            string reason;
            if (isDraw(board,last_stats,reason)) {
                // A draw position exists before we even move (probably
                // because the opponent did not claim the draw).
                // Send the result command to claim the draw.
                if (doTrace) {
                    cout << debugPrefix() << "claiming draw before move";
                    if (reason.length()) cout << " (" << reason << ")";
                    cout << endl;
                }
                cout << "1/2-1/2 {" << reason << "}" << endl;
                // Wait for Winboard to send a "result" command before
                // actually concluding it's a draw.
                // Set flag to indicate we are waiting.
                result_pending = true;
                return;
            }
        }
        if (!IsNull(move)) {
            stringstream img;
            Notation::image(board,last_move,Notation::OutputFormat::SAN,img);
            last_move_image = img.str();
            theLog->add_move(board,last_move,last_move_image,&last_stats,searcher->getElapsedTime(),true);
            // Perform learning (if enabled):
            learn(board,board.repCount());
            stringstream movebuf;
            move_image(board,last_move,movebuf,uci);

            if (uci) {
                cout << "bestmove " << movebuf.str();
                if (!easy && !IsNull(stats.best_line[1])) {
                    stringstream ponderbuf;
#ifdef _DEBUG
                    BoardState s(board.state);
                    board.doMove(move);
                    // ensure ponder move is legal
                    ASSERT(legalMove(board,stats.best_line[1]));
                    board.undoMove(move,s);
#endif
                    move_image(board,stats.best_line[1],ponderbuf,uci);
                    cout << " ponder " << ponderbuf.str();
                }
                cout << endl << (flush);
            }
            else { // Winboard
                // Execute the move and prepare to ponder.
                BoardState previous_state = board.state;
                board.doMove(last_move);
                gameMoves->add_move(board,previous_state,last_move,last_move_image,false);

                *ponder_board = board;

                string reason;
                if (isDraw(board,last_stats,reason)) {
                    // It will be a draw after we move (by rule).
                    // Following the current protocol standard, send
                    // "offer draw" and then send the move (formerly
                    // we would send the move then send the result,
                    // which is incorrect).
                    cout << "offer draw" << endl;
                }
                if (xboard42) {
                    cout << "move " << movebuf.str() << endl;
                }
                else {
                    cout << gameMoves->num_moves()/2 << ". ... ";
                    cout << movebuf.str() << endl;
                }
                cout << (flush);
            }
        }
        else if (uci) {
            // must always send a "bestmove" command even if no move is available, to
            // acknowledge the previous "stop" command.
            cout << "bestmove 0000" << endl;
        } else {
            if (doTrace) cout << debugPrefix() << "warning : move is null" << endl;
        }
        if (ics && ((srctype == FixedDepth && searcher->getElapsedTime() >= 250) || time_target >= 250) &&
            stats.display_value != Constants::INVALID_SCORE) {
            kibitz(searcher,computer,last_stats,options.search.ncpus>1);
        }
    }
    if (uci) return; // With UCI, GUI is in charge of game end detection
    // We already checked for draws, check now for other game end
    // conditions.
    if (!game_end) {
        if (last_stats.value >= Constants::MATE-1) {
            if (doTrace) cout << debugPrefix() << "last_score = mate" << endl;
            if (sideToMove == White) {
                theLog->setResult("1-0");
                cout << "1-0 {White mates}" << endl;
            }
            else {
                theLog->setResult("0-1");
                cout << "0-1 {Black mates}" << endl;
            }
            game_end = true;
        }
        else if (last_stats.state == Checkmate) {
            if (doTrace) cout << debugPrefix() << "state = Checkmate" << endl;
            if (sideToMove == White) {
                theLog->setResult("0-1");
                cout << "0-1 {Black mates}" << endl;
            }
            else {
                theLog->setResult("1-0");
                cout << "1-0 {White mates}" << endl;
            }
            game_end = true;
        }
    }
    if (last_stats.state == Resigns) {
        // Don't resign a zero-increment game if the opponent is short
        // on time
        if (!(incr == 0 && opp_time < 2000)) {
            // Winboard passes the resign command to ICS, but ignores it
            // itself.
            if (computer_plays_white) {
                theLog->setResult("0-1 {White resigns}");
                cout << "0-1 {White resigns}" << endl;
            }
            else {
                theLog->setResult("1-0 {Black resigns}");
                cout << "1-0 {Black resigns}" << endl;
            }
            game_end = true;
        }
        else {   // reset flag - we're not resigning
            stats.end_of_game = game_end = false;
        }
    }
}

void Protocol::processCmdInWaitState(const string &cmd) {
    if (doTrace) {
        cout << debugPrefix() << "got command in wait state: " << cmd << (flush) << endl;
    }
    // we expect a "stop" or "ponderhit"
    if (cmd == "ponderhit" || cmd == "stop") {
        send_move(*main_board,last_computer_move,last_computer_stats);
        uciWaitState = false;
    } else if (cmd == "quit") {
        uciWaitState = false;
        polling_terminated = true;
    } else if (cmd == "ucinewgame") {
        // Arena at least can send this w/o "stop"
        uciWaitState = false;
    }
}

Move Protocol::analyze(SearchController &searcher, Board &board, Statistics &stats)
{
    last_stats.clear();
    last_score = Constants::MATE;

    stats.clear();
    if (doTrace) {
        cout << debugPrefix() << "entering analysis search" << endl;
    }
    Move move = searcher.findBestMove(board,
                                      FixedTime,
                                      INFINITE_TIME,
                                      0,
                                      Constants::MaxPly, false, uci,
                                      stats,
                                      TalkLevel::Whisper);
    if (doTrace) {
        cout << debugPrefix() << "search done : move = ";
        MoveImage(move,cout);
        cout << endl;
    }

    last_stats = stats;
    post_output(stats);

    return move;
}

void Protocol::doHint() {
    // try book move first
    vector<Move> moves;
    unsigned count = 0;
    if (options.book.book_enabled) {
        count = openingBook.book_moves(*main_board,moves);
    }
    if (count > 0) {
        if (count == 1)
            cout << "Book move: " ;
        else
            cout << "Book moves: ";
        for (unsigned i = 0; i<count; i++) {
            Notation::image(*main_board,moves[i],Notation::OutputFormat::SAN,cout);
            if (i<count-1) cout << ' ';
        }
        cout << endl;
        return;
    }
    else {
        // no book move, see if we have a ponder move
        const string &img = last_stats.best_line_image;
        if (img.length()) {
            string::const_iterator it = img.begin();
            while (it != img.end() && !isspace(*it)) it++;
            string last_move;
            if (it != img.end()) {
                it++;
                while (it != img.end() && !isspace(*it)) last_move += *it++;
            }
            if (last_move.length()) {
                cout << "Hint: " << last_move << endl;
                return;
            }
        }
    }
    // no ponder move or book move. If we are already pondering but
    // have no ponder move we could wait a while for a ponder result,
    // but we just return for now.
    if (searcher->pondering()) return;
    if (doTrace) cout << debugPrefix() << "computing hint" << endl;

    Statistics tmp;
    // do low-depth search for hint move
    Move move = searcher->findBestMove(*main_board,
        FixedDepth,
        0,
        0,
        4, false, uci,
        tmp,
        (doTrace) ? TalkLevel::Debug : TalkLevel::Silent);
    if (!IsNull(move)) {
        cout << "Hint: ";
        Notation::image(*main_board,move,Notation::OutputFormat::SAN,cout);
        cout << endl;
    }
}

void Protocol::analyze_output(const Statistics &stats) {
    cout << "stat01: " <<
        searcher->getElapsedTime() << " " << stats.num_nodes << " " <<
        stats.depth << " " <<
        stats.mvleft << " " << stats.mvtot << endl;
}

void Protocol::analyze(Board &board)
{
    if (doTrace) cout << debugPrefix() << "entering analysis mode" << endl;
    while (analyzeMode) {
        Board previous(board);
        analyze(*searcher,board,stats);
        if (doTrace) cout << debugPrefix() << "analysis mode: out of search" << endl;
        // Process commands received while searching; exit loop
        // if "quit" seen.
        if (do_all_pending(board)==AllPendingStatus::Quit) {
            break;
        }
        while (board == previous && analyzeMode) {
            // The user has given us no new position to search. We probably
            // got here because the search has terminated early, due to
            // forced move, forced mate, tablebase hit, or hitting the max
            // ply depth. Wait here for more input.
            if (doTrace) cout << debugPrefix() << "analysis mode: wait for input" << endl;
            if (inputSem.wait()) {
                break;
            }
            while (!pending.empty()) {
                Lock(input_lock);
                string cmd (pending.front());
                pending.erase(pending.begin());
                Unlock(input_lock);
                string cmd_word, cmd_arg;
                split_cmd(cmd,cmd_word,cmd_arg);
#ifdef _TRACE
                cout << debugPrefix() << "processing cmd in analysis mode: " << cmd << endl;
#endif
                if (cmd == "undo" || cmd == "setboard") {
                    do_command(cmd,board);
                }
                // Technically "quit" is not supposed to be the way
                // to exit analysis mode but we allow it.
                else if (cmd == "exit" || cmd == "quit") {
                    analyzeMode = 0;
                }
                else if (cmd == "bk") {
                    do_command(cmd,board);
                }
                else if (cmd == "hint") {
                    do_command(cmd,board);
                }
                else if (cmd_word == "usermove" || text_to_move(board,cmd) != NullMove) {
                    Move m = get_move(cmd_word, cmd_arg);
                    if (!IsNull(m)) {
                        board.doMove(m);
                    }
                }
                else if (cmd == ".") {
                    analyze_output(stats);
                }
            }
        }
    }
    if (doTrace) cout << debugPrefix() << "exiting analysis mode" << endl;
}

void Protocol::undo( Board &board)
{
    if (theLog->current() < 1) return;             // ignore "undo"

    board.undoMove((*theLog)[theLog->current()-1].move(),
        (*theLog)[theLog->current()-1].state());
    theLog->back_up();
    gameMoves->remove_move();
    last_stats.clear();
    if (theLog->current()) {
        last_move = (*theLog)[theLog->current()-1].move();
        last_move_image = (*theLog)[theLog->current()-1].image();
    }
    else {
        last_move = NullMove;
        last_move_image.clear();
    }
    // In case we have backed up from the end of the game, reset
    // the "game end" flag.
    game_end = false;
}

void Protocol::setCheckOption(const string &value, int &dest) {
   stringstream buf(value);
   int tmp;
    buf >> tmp;
    if (!buf.bad() && !buf.fail()) {
        dest = tmp != 0;
    }
}

// Execute a move made by the opponent or in "force" mode.
void Protocol::execute_move(Board &board,Move m)
{
    if (doTrace) {
        cout << debugPrefix() << "execute_move: ";
        MoveImage(m,cout);
        cout << endl;
    }
    last_move = m;
    stringstream img;
    Notation::image(board,m,Notation::OutputFormat::SAN,img);
    last_move_image = img.str();
    theLog->add_move(board,m,last_move_image,nullptr,searcher->getElapsedTime(),true);
    BoardState previous_state = board.state;
    board.doMove(m);
    // If our last move added was the pondering move, replace it
    if (gameMoves->num_moves() > 0 && gameMoves->last().wasPonder()) {
        gameMoves->remove_move();
    }
    gameMoves->add_move(board,previous_state,m,last_move_image,false);
}

#ifdef TUNE
void Protocol::setTuningParam(const string &name, const string &value)
{
   // set named parameters that are in the tuning set
   int index = tune_params.findParamByName(name);
   if (index > 0) {
      stringstream buf(value);
      int tmp;
      buf >> tmp;
      if (!buf.bad() && !buf.fail()) {
         tune_params[index].current = tmp;
         // apply params to Scoring module
         tune_params.applyParams();
      }
      else {
         if (uci) cout << "info ";
         else cout << "#";
         cout << "Warning: invalid value for option " <<
            name << ": " << value << endl;
         return;
      }
   }
   else {
      if (uci) cout << "info ";
      else cout << "#";
      cout << "Warning: invalid option name \"" <<
            name << "\"" << endl;
   }
}
#endif

void Protocol::processWinboardOptions(const string &args) {
    string name, value;
    size_t eq = args.find("=");
    if (eq == string::npos) {
        // no value
        name = args;
    } else {
        name = args.substr(0,eq);
        value = args.substr(eq+1);
    }
    // trim spaces
    name = name.erase(0 , name.find_first_not_of(' ') );
    name = name.erase( name.find_last_not_of(' ') + 1);
    value = value.erase(0, value.find_first_not_of(' '));
    value = value.erase(value.find_last_not_of(' ') + 1);
    // handle option settings
    if (doTrace) {
        cout << debugPrefix() << "setting option " << name << "=" << value << endl;
    }
    if (name == "Favor frequent book moves") {
        Options::setOption<unsigned>(value,options.book.frequency);
    } else if (name == "Favor high-weighted book moves") {
        Options::setOption<unsigned>(value,options.book.weighting);
    } else if (name == "Favor best book moves") {
        Options::setOption<unsigned>(value,options.book.scoring);
    } else if (name == "Randomize book moves") {
        Options::setOption<unsigned>(value,options.book.random);
    } else if (name == "Can resign") {
        setCheckOption(value,options.search.can_resign);
    } else if (name == "Resign threshold") {
        Options::setOption<int>(value,options.search.resign_threshold);
    } else if (name == "Position learning") {
        setCheckOption(value,options.learning.position_learning);
    } else if (name == "Strength") {
        Options::setOption<int>(value,options.search.strength);
#ifdef NUMA
    } else if (name == "Set processor affinity") {
       int tmp = options.search.set_processor_affinity;
       setCheckOption(value,options.search.set_processor_affinity);
       if (tmp != options.search.set_processor_affinity) {
           searcher->recalcBindings();
       }
#endif
    }
    else if (name == "Move overhead") {
        Options::setOption<int>(value,options.search.move_overhead);
    }
#ifdef TUNE
    else {
       setTuningParam(name,value);
    }
#else
    else {
       cout << debugPrefix() << "Warning: invalid option name \"" << name << "\"" << endl;
   }
#endif
   searcher->updateSearchOptions();
}

uint64_t Protocol::perft(Board &board, int depth) {
   if (depth == 0) return 1;

   uint64_t nodes = 0ULL;
   RootMoveGenerator mg(board);
   Move m;
   BoardState state = board.state;
   int order = 0;
   while ((m = mg.nextMove(order)) != NullMove) {
      if (depth > 1) {
         board.doMove(m);
         nodes += perft(board,depth-1);
         board.undoMove(m,state);
      } else {
         // skip do/undo
         nodes++;
      }
   }
   return nodes;
}

void Protocol::loadgame(Board &board,ifstream &file) {
    vector<ChessIO::Header> hdrs(20);
    long first;
    ChessIO::collect_headers(file,hdrs,first);
    ColorType side = White;
    for (;;) {
        string num;
        ChessIO::Token tok = ChessIO::get_next_token(file);
        if (tok.type == ChessIO::Eof)
            break;
        else if (tok.type == ChessIO::Number) {
            num = tok.val;
        }
        else if (tok.type == ChessIO::GameMove) {
            // parse the move
            Move m = Notation::value(board,side,Notation::InputFormat::SAN,tok.val);
            if (IsNull(m) ||
                !legalMove(board,StartSquare(m),
                           DestSquare(m))) {
                cerr << "Illegal move" << endl;
                break;
            }
            else {
                BoardState previous_state = board.state;
                string image;
                // Don't use the current move string as the input
                // parser is forgiving and will accept incorrect
                // SAN. Convert it here to the correct form:
                Notation::image(board,m,Notation::OutputFormat::SAN,image);
                gameMoves->add_move(board,previous_state,m,image.c_str(),false);
                board.doMove(m);
            }
            side = OppositeColor(side);
        }
        else if (tok.type == ChessIO::Result) {
            break;
        }
    }
}


#ifdef SYZYGY_TBS
bool Protocol::validTbPath(const string &path) {
   // Shredder at least sets path to "<empty>" for tb types that are disabled
   return path != "" && path != "<empty>";
}
#endif

bool Protocol::uciOptionCompare(const string &a, const string &b) {
   if (a.length() != b.length()) {
      return false;
   } else {
      for (unsigned i = 0; i < a.length(); i++) {
         if (tolower(a[i]) != tolower(b[i])) return false;
      }
      return true;
   }
}

bool Protocol::do_command(const string &cmd, Board &board) {
    if (doTrace) {
        cout << debugPrefix() << "do_command: " << cmd << endl;
    }
    if (doTrace && uci) {
        theLog->write(cmd.c_str()); theLog->write_eol();
    }
    string cmd_word, cmd_args;
    split_cmd(cmd, cmd_word, cmd_args);
    if (cmd == "uci") {
        uci = true;
        verbose = true; // TBD: fixed for now
        // Learning is disabled because we don't have full game history w/ scores
        options.learning.position_learning = 0;
        cout << "id name " << "Arasan " << Arasan_Version;
        cout << endl;
        cout << "id author Jon Dart" << endl;
        cout << "option name Hash type spin default " <<
            options.search.hash_table_size/(1024L*1024L) << " min 4 max " <<
#ifdef _64BIT
            "64000" << endl;
#else
            "2000" << endl;
#endif
        cout << "option name Ponder type check default true" << endl;
        cout << "option name Contempt type spin default 0 min -200 max 200" << endl;
#ifdef SYZYGY_TBS
        cout << "option name Use tablebases type check default ";
        if (options.search.use_tablebases) cout << "true"; else cout << "false";
        cout << endl;
        cout << "option name SyzygyTbPath type string default " <<
            options.search.syzygy_path << endl;
        cout << "option name SyzygyUse50MoveRule type check default true" << endl;
        cout << "option name SyzygyProbeDepth type spin default " <<
            options.search.syzygy_probe_depth <<
           " min 0 max 64" << endl;
#endif
        cout << "option name MultiPV type spin default 1 min 1 max " << Statistics::MAX_PV << endl;
        cout << "option name OwnBook type check default true" << endl;
        cout << "option name Favor frequent book moves type spin default " <<
            options.book.frequency << " min 0 max 100" << endl;
        cout << "option name Favor best book moves type spin default " <<
            options.book.scoring << " min 0 max 100" << endl;
        cout << "option name Favor high-weighted book moves type spin default " <<
            options.book.weighting << " min 0 max 100" << endl;
        cout << "option name Randomize book moves type spin default " <<
            options.book.random << " min 0 max 100" << endl;
        cout << "option name Threads type spin default " <<
            options.search.ncpus << " min 1 max " <<
            Constants::MaxCPUs << endl;
        cout << "option name UCI_LimitStrength type check default false" << endl;
        cout << "option name UCI_Elo type spin default " <<
            1000+options.search.strength*16 << " min 1000 max 2600" << endl;
#ifdef NUMA
        cout << "option name Set processor affinity type check default " <<
           (options.search.set_processor_affinity ? "true" : "false") << endl;
#endif
        cout << "option name Move overhead type spin default " <<
            30 << " min 0 max 1000" << endl;
        cout << "uciok" << endl;
        return true;
    }
    else if (cmd == "quit") {
        return false;
    }
    else if (uci && cmd_word == "setoption") {
        string name, value;
        size_t nam = cmd_args.find("name");
        if (nam != string::npos) {
            // search for "value"
            size_t val = cmd_args.find(" value",nam+4);
            if (val != string::npos) {
               name = cmd_args.substr(nam+4,val-nam-4);
               // trim spaces
               name = name.erase(0 , name.find_first_not_of(' ') );
               name = name.erase( name.find_last_not_of(' ') + 1);
               value = cmd_args.substr(val+6);
               value = value.erase(0, value.find_first_not_of(' '));
               value = value.erase(value.find_last_not_of(' ') + 1);
            }
        }
        if (uciOptionCompare(name,"Hash")) {
            if (!memorySet) {
                size_t old = options.search.hash_table_size;
                // size is in megabytes
                stringstream buf(value);
                int size;
                buf >> size;
                if (buf.bad()) {
                    cout << "info problem setting hash size to " << buf.str() << endl;
                }
                else {
                    options.search.hash_table_size = (size_t)size*1024L*1024L;
                    if (old != options.search.hash_table_size) {
                       searcher->resizeHash(options.search.hash_table_size);
                    }
                }
            }
        }
        else if (uciOptionCompare(name,"Ponder")) {
            easy = !(value == "true");
        }
        else if (uciOptionCompare(name,"Contempt")) {
            stringstream buf(value);
            int uciContempt;
            buf >> uciContempt;
            if (buf.bad()) {
               cout << "info problem setting contempt value" << endl;
            }
            else if (uciContempt < -200 || uciContempt > 200) {
               cout << "invalid contempt value, must be >=-200, <= 200 centipawns" << endl;
            }
            else {
               searcher->setContempt(uciContempt);
            }
        }
#ifdef SYZYGY_TBS
        else if (uciOptionCompare(name,"Use tablebases")) {
            options.search.use_tablebases = (value == "true");
        }
        else if (uciOptionCompare(name,"SyzygyTbPath") && validTbPath(value)) {
           unloadTb();
           options.search.syzygy_path = value;
           options.search.use_tablebases = 1;
        }
        else if (uciOptionCompare(name,"SyzygyUse50MoveRule")) {
           options.search.syzygy_50_move_rule = (value == "true");
        }
        else if (uciOptionCompare(name,"SyzygyProbeDepth")) {
           Options::setOption<int>(value,options.search.syzygy_probe_depth);
        }
#endif
        else if (uciOptionCompare(name,"OwnBook")) {
            options.book.book_enabled = (value == "true");
        }
        else if (uciOptionCompare(name,"Favor frequent book moves")) {
            Options::setOption<unsigned>(value,options.book.frequency);
        }
        else if (uciOptionCompare(name,"Favor best book moves")) {
            Options::setOption<unsigned>(value,options.book.scoring);
        }
        else if (uciOptionCompare(name,"Favor high-weighted book moves")) {
            Options::setOption<unsigned>(value,options.book.weighting);
        }
        else if (uciOptionCompare(name,"Randomize book moves")) {
            Options::setOption<unsigned>(value,options.book.random);
        }
        else if (uciOptionCompare(name,"MultiPV")) {
            Options::setOption<int>(value,options.search.multipv);
            options.search.multipv = std::min<int>(Statistics::MAX_PV,options.search.multipv);
            // GUIs (Shredder at least) send 0 to turn multi-pv off: but
            // our option counts the # of lines to show, so we set it to
            // 1 in this case.
            if (options.search.multipv == 0) options.search.multipv = 1;
            stats.multipv_count = 0;
            // migrate current stats to 1st Multi-PV table entry:
            stats.multi_pvs[0] =
                Statistics::MultiPVEntry(stats);
        }
        else if (uciOptionCompare(name,"Threads")) {
            int threads = options.search.ncpus;
            if (Options::setOption<int>(value,threads) && threads >0 && threads <= Constants::MaxCPUs) {
                options.search.ncpus = threads;
                searcher->setThreadCount(options.search.ncpus);
            }
        }
        else if (uciOptionCompare(name,"UCI_LimitStrength")) {
            uciStrengthOpts.limitStrength = (value == "true");
            if (uciStrengthOpts.limitStrength) {
               options.setRating(uciStrengthOpts.eco);
            } else {
               // reset to full strength
               options.setRating(2600);
            }
        } else if (uciOptionCompare(name,"UCI_Elo")) {
            int rating;
            if (Options::setOption<int>(value,rating)) {
               uciStrengthOpts.eco = rating;
               if (uciStrengthOpts.limitStrength) {
                  options.setRating(rating);
               }
            }
	}
#ifdef NUMA
        else if (uciOptionCompare(name,"Set processor affinity")) {
           int tmp = options.search.set_processor_affinity;
           options.search.set_processor_affinity = (value == "true");
           if (tmp != options.search.set_processor_affinity) {
               searcher->recalcBindings();
           }
        }
#endif
        else if (uciOptionCompare(name,"Move overhead")) {
           Options::setOption<int>(value,options.search.move_overhead);
        }
#ifdef TUNE
        else {
           setTuningParam(name,value);
        }
#else
        else {
           cout << "info error: invalid option name \"" << name << "\"" << endl;
        }
#endif
        searcher->updateSearchOptions();
    }
    else if (uci && cmd == "ucinewgame") {
        do_command("new",board);
        return true;
    }
    else if (uci && cmd == "isready") {
        delayedInit();
        cout << "readyok" << endl;
    }
    else if (uci && cmd_word == "position") {
        ponder_move = NullMove;
        if (cmd_args.substr(0,8) == "startpos") {
            board.reset();
            gameMoves->removeAll();
        }
        else if (cmd_args.substr(0,3) == "fen") {
            string fen;
            int valid = 0;
            if (cmd_args.length() > 3) {
                fen = cmd_args.substr(3);
                valid = BoardIO::readFEN(board, fen);
            }
            if (!valid) {
                if (doTrace) cout << debugPrefix() << "warning: invalid fen!" << endl;
            }
            // clear some global vars
            stats.clear();
            ponder_stats.clear();
            last_stats.clear();
            last_move = NullMove;
            last_move_image.clear();
            gameMoves->removeAll();
            predicted_move = NullMove;
            ponder_status = PonderStatus::None;
        }
        size_t movepos = cmd_args.find("moves");
        if (movepos != string::npos) {
            stringstream s(cmd_args.substr(movepos+5));
            istream_iterator<string> it(s);
            istream_iterator<string> eos;
            while (it != eos) {
                string move(*it++);
                Move m = Notation::value(board,board.sideToMove(),Notation::InputFormat::UCI,move);
                if (!IsNull(m)) {
                   BoardState previous_state = board.state;
                   board.doMove(m);
                   gameMoves->add_move(board,previous_state,m,"",false);
                }
            }
        }
    }
    else if (uci && cmd_word == "debug") {
        if (cmd_args == "on") {
            doTrace = true;
        }
        else if (cmd_args == "off") {
            doTrace = false;
        }
    }
    else if (editMode) {
       edit_mode_cmds(board,side,cmd_word);
    }
    else if (cmd_word == "bench") {
       Bench b;
       Bench::Results res = b.bench(true);
       cout << res;
    }
    else if (cmd_word == "test") {
        string filename;
        ofstream *out_file = nullptr;
        streambuf *sbuf = cout.rdbuf();
        stringstream s(cmd_args);
        istream_iterator<string> it(s);
        istream_iterator<string> eos;
        Tester::TestOptions opts;
        if (it != eos) {
            filename = *it++;
            if (it != eos) {
                while (it != eos) {
                    if (*it == "-d") {
                        if (++it == eos) {
                            cerr << "expected number after -d" << endl;
                        } else {
                            stringstream num(*it);
                            num >> opts.depth_limit;
                            it++;
                        }
                    }
                    else if (*it == "-t") {
                        if (++it == eos) {
                            cerr << "expected number after -t" << endl;
                        } else {
                            stringstream num(*it);
                            num >> opts.time_limit;
                            opts.time_limit *= 1000; // convert seconds to ms
                            it++;
                        }
                    }
                    else if (*it == "-v") {
                        it++;
                        opts.verbose = true;
                        continue;
                    }
                    else if (*it == "-x") {
                        if (++it == eos) {
                            cerr << "expected number after -x" << endl;
                        } else {
                            stringstream num(*it);
                            num >> opts.early_exit_plies;
                            it++;
                        }
                    }
                    else if (*it == "-N") {
                        if (++it == eos) {
                            cerr << "Expected number after -N" << endl;
                        } else {
                            stringstream num(*it);
                            num >> opts.moves_to_search;
                            it++;
                        }
                    }
                    else if (*it == "-o") {
                        if (++it == eos) {
                            cerr << "Expected filename after -o" << endl;
                        } else {
                            out_file = new ofstream((*it).c_str(), ios::out | ios::trunc);
                            // redirect stdout
                            cout.rdbuf(out_file->rdbuf());
                            break;
                        }
                    } else if ((*it)[0] == '-') {
                        cerr << "unexpected switch: " << *it << endl;
                        it++;
                    } else {
                        break;
                    }
                }
                if (opts.depth_limit == 0 && opts.time_limit == 0) {
                    cerr << "error: time (-t) or depth (-d) must be specified" << endl;
                }
                else {
                    do_command("new",board);
                    Tester tester;
                    tester.do_test(searcher,filename,opts);
                    cout << "test complete" << endl;
                }
                if (out_file) {
                    out_file->close();
                    delete out_file;
                    cout.rdbuf(sbuf);               // restore console output
                }
            }
            else
                cout << "invalid command" << endl;
        }
        else
            cout << "invalid command" << endl;
    }
    else if (cmd_word == "perft") {
       if (cmd_args.length()) {
          stringstream ss(cmd_args);
          int depth;
          if ((ss >> depth).fail()) {
             cerr << "usage: perft <depth>" << endl;
          } else {
             Board b;
             cout << "perft " << depth << " = " << perft(b,depth) << endl;
          }
       }
       else {
          cerr << "usage: perft <depth>" << endl;
       }
    }
    else if (cmd_word == "eval") {
        string filename;
        if (cmd_args.length()) {
            filename = cmd_args;
            ifstream pos_file( filename.c_str(), ios::in);
            pos_file >> board;
            if (!pos_file.good()) {
                cout << "File not found, or bad format." << endl;
            }
            else {
                delayedInit();
#ifdef SYZYGY_TBS
                score_t tbscore;
                if (options.search.use_tablebases) {
                   MoveSet rootMoves;
                   if (SyzygyTb::probe_root(board,board.anyRep(),tbscore,rootMoves) >= 0) {
                      cout << "score = ";
                      if (tbscore == -SyzygyTb::CURSED_SCORE)
                         cout << "draw (Cursed Loss)";
                      else if (tbscore == -SyzygyTb::CURSED_SCORE)
                         cout << "draw (Cursed Win)";
                      else
                         Scoring::printScore(tbscore,cout);
                      cout << " (from Syzygy tablebases)" << endl;
                   }
                }
#endif
                score_t score;
                if ((score = Scoring::tryBitbase(board))!= Constants::INVALID_SCORE) {
                    cout << "bitbase score=";
                    Scoring::printScore(score,cout);
                    cout << endl;
                }
                Scoring::init();
                if (board.isLegalDraw()) {
                     cout << "position evaluates to draw (statically)" << endl;
                }
                Scoring *s = new Scoring();
                s->init();
                cout << board << endl;
                Scoring::printScore(s->evalu8(board),cout);
                cout << endl;
                board.flip();
                cout << board << endl;
                Scoring::printScore(s->evalu8(board),cout);
                delete s;
                cout << endl;
            }
        }
    }
    else if (uci && cmd == "stop") {
        searcher->stop();
        return true;
    }
    else if (uci && cmd_word == "go") {
        string option;
        srctype = TimeLimit;
        bool do_ponder = false;
        movestogo = 0;
        bool infinite = false;
        stringstream ss(cmd_args);
        istream_iterator<string> it(ss);
        istream_iterator<string> eos;
        MoveSet movesToSearch;
        while (it != eos) {
            option = *it++;
            if (option == "wtime") {
                srctype = TimeLimit;
                if (it == eos) break;
                int t = 0;
                if (Options::setOption<int>(*it++,t)) {
                   if (board.sideToMove() == White)
                       time_left = t;
                   else
                       opp_time = t;
                }
            }
            else if (option == "btime") {
                srctype = TimeLimit;
                if (it == eos) break;
                int t = 0;
                if (Options::setOption<int>(*it++,t)) {
                    if (board.sideToMove() == Black)
                        time_left = t;
                    else
                        opp_time = t;
                }
            }
            else if (option == "infinite") {
                srctype = FixedTime;
                time_limit = INFINITE_TIME;
                infinite = true;
            }
            else if (option == "winc") {
                if (it == eos) break;
                // incr is in milliseconds
                int tmp = 0;
                if (Options::setOption<int>(*it++,tmp)) {
                    winc = tmp;
                }
            }
            else if (option == "binc") {
                if (it == eos) break;
                // incr is in milliseconds
                int tmp = 0;
                if (Options::setOption<int>(*it++,tmp)) {
                    binc = tmp;
                }
            }
            else if (option == "movestogo") {
                if (it == eos) break;
                Options::setOption<int>(*it++,movestogo);
            }
            else if (option == "depth") {
                if (it == eos) break;
                if (Options::setOption<int>(*it++,ply_limit)) {
                    srctype = FixedDepth;
                }
            }
            else if (option == "movetime") {
                if (it == eos) break;
                stringstream s(*it++);
                int srctime;
                s >> srctime;
                if (s.bad() || srctime < 0.0) {
                    cerr << "movetime: invalid argument" << endl;
                } else {
                    srctype = FixedTime;
                    // set time limit (milliseconds)
                    time_limit = srctime;
                }
            }
            else if (option == "ponder") {
                do_ponder = true;
                ponderhit = false;
            }
            else if (option == "searchmoves") {
                for (;it != eos;it++) {
                    Move m = Notation::value(board,board.sideToMove(),
                                             Notation::InputFormat::UCI,*it);
                    if (IsNull(m)) {
                        // illegal move, or end of move list
                        break;
                    } else {
                        movesToSearch.insert(m);
                    }
                }
            }
        }
        forceMode = false;
        // Some GUIs such as Fritz send tablebase options after
        // "isready" and so it is possible tablebases and book are
        // not initialized. Make sure they are before we execute
        // "go:"
        delayedInit();
        if (do_ponder) {
            ponder(board,NullMove,NullMove,1);
            // We should not send the move unless we have received a
            // "ponderhit" command, in which case we were pondering the
            // right move. If pondering completes early, we must wait
            // for ponderhit before sending. But also send the move if
            // we were stopped - this is the "handshake" that tells the
            // UI we received the stop.
            if (doTrace) {
                cout << debugPrefix() << "done pondering: stopped=" << (int)searcher->wasStopped() << " move=";
                Notation::image(board,ponder_move,Notation::OutputFormat::SAN,cout);
                cout << (flush) << endl;
            }
            if (ponderhit || searcher->wasStopped()) {
                // ensure we send an "info" command - may not have been
                // sent if the previous move was forced or a tb hit.
                uciOut(ponder_stats);
                send_move(board,ponder_move,ponder_stats);
                ponder_move = NullMove;
                ponderhit = false;
            }
            else {
                // We completed pondering early - the protocol requires
                // that we delay sending the move until "ponderhit" or
                // "stop" is received.
                if (doTrace) {
                    cout << debugPrefix() << "ponder stopped early" << endl;
                }
                Lock(input_lock);
                // To avoid races, check with the input mutux locked
                // that we do not now have ponderhit or stop in the
                // pending stack
                vector<string> newPending(pending);
                uciWaitState = true;
                for (const std::string &cmd : pending) {
                    if (cmd == "stop" || cmd == "ponderhit") {
                        uciWaitState = false;
                    } else {
                        newPending.push_back(cmd);
                    }
                }
                pending = newPending;
                if (!uciWaitState) {
                    cout << debugPrefix() << "ponderhit in stack, sending move" << endl << (flush);
                    uciOut(ponder_stats);
                    send_move(board,ponder_move,ponder_stats);
                    ponder_move = NullMove;
                    ponderhit = false;
                }
                else if (doTrace) {
                    cout << debugPrefix() << "entering wait state" << endl << (flush);
                }
                Unlock(input_lock);
            }
        }
        else {
            CLOCK_TYPE startTime;
            if (doTrace) {
                startTime = getCurrentTime();
                cout << debugPrefix() << "starting search" << (flush) << endl;
            }
            best_move = search(searcher,board,movesToSearch,stats,infinite);
            if (doTrace) {
                cout << "done searching, elapsed time=" << getElapsedTime(startTime,getCurrentTime()) << ", stopped=" << (int)searcher->wasStopped() << (flush) << endl;
            }
            if (infinite && !searcher->wasStopped()) {
                // ensure we send some info in analysis mode:
                post_output(stats);
                // We were told "go infinite" but completed searching early
                // (due to a mate or forced move or tb hit). The protocol
                // requires that we go into a wait state before sending the
                // move. We will exit when a "stop" command is received.
                uciWaitState = true;
            }
            else {
                send_move(board,best_move,stats);
            }
        }
        // reset stopped flag after search
        searcher->setStop(false);
    }
    else if (uci && cmd == "ponderhit") {
        ponderhit = true;
        if (!IsNull(ponder_move)) {
            uciOut(ponder_stats);
            send_move(board,ponder_move,ponder_stats);
        }
    }
    else if (cmd == "help") {
        do_help();
    }
    else if (cmd == "end") {
        return false;
    }
    else if (cmd == "new") {
        if (!analyzeMode) save_game();
        board.reset();
        bool wasEmpty = theLog->num_moves() == 0;
        theLog->clear();
        if (!uci && !wasEmpty) theLog->write_header();
        computer_plays_white = false;
        // Note: "new" does not reset analyze mode
        forceMode = false;
        game_end = result_pending = false;
        computer = 0;
        opponent_name = "";
        stats.clear();
        ponder_stats.clear();
        last_stats.clear();
        last_move = NullMove;
        last_move_image.clear();
        gameMoves->removeAll();
        predicted_move = NullMove;
        ponder_status = PonderStatus::None;
        start_fen.clear();
        delayedInit();
        searcher->clearHashTables();
#ifdef TUNE
        tune_params.applyParams();
#endif
        if (!analyzeMode && ics) {
           cout << "kib Hello from Arasan " << Arasan_Version << endl;
        }
        if (doTrace) cout << debugPrefix() << "finished 'new' processing" << endl;
    }
    else if (cmd == "random" || cmd_word == "variant") {
        // ignore
    }
    else if (cmd_word == "protover") {
        // new in Winboard 4.2
        cout << "feature name=1 setboard=1 san=1 usermove=1 ping=1 ics=1 playother=0 sigint=0 colors=0 analyze=1 debug=1 memory=1 smp=1 variants=\"normal\"";
#ifdef SYZYGY_TBS
        cout << " egt=\"syzygy\"";
#endif
        cout << " option=\"Favor frequent book moves -spin " <<
            options.book.frequency << " 1 100\"";
        cout << " option=\"Favor best book moves -spin " <<
            options.book.scoring << " 1 100\"";
        cout << " option=\"Favor high-weighted book moves -spin " <<
            options.book.weighting << " 1 100\"";
        cout << " option=\"Randomize book moves -spin " <<
            options.book.random << " 1 100\"";
        cout << " option=\"Can resign -check " <<
            options.search.can_resign << "\"";
        cout << " option=\"Resign threshold -spin " <<
            options.search.resign_threshold << " -1000 0" << "\"";
        cout << " option=\"Position learning -check " <<
            options.learning.position_learning << "\"";
        // strength option (new for 14.2)
        cout << " option=\"Strength -spin " << options.search.strength << " 0 100\"";
#ifdef NUMA
        cout << " option=\"Set processor affinity -check " <<
            options.search.set_processor_affinity << "\"" << endl;
#endif
        cout << " option=\"Move overhead -spin " << 30 << " 0 1000\"" << endl;
        cout << "myname=\"" << "Arasan " << Arasan_Version << "\"" << endl;
        // set done = 0 because it may take some time to initialize tablebases.
        cout << "feature done=0" << endl;
        delayedInit();
        cout << "feature done=1" << endl;
        xboard42 = true;
    }
    else if (cmd == "computer") {
        computer = 1;
    }
    else if (cmd_word == "ping") {
        // not in a search, so send "pong" immediately
        sendPong(cmd_args);
    }
    else if (cmd_word == "ics") {
        hostname = cmd_args;
    }
    else if (cmd == "hint") {
        doHint();
    }
    else if (cmd == "bk") {
        // list book moves
	vector<Move> moves;
        unsigned count = 0;
        delayedInit(); // to allow "bk" before "new"
        if (options.book.book_enabled) {
            count = openingBook.book_moves(*main_board,moves);
        }
        if (count == 0) {
            cout << '\t' << "No book moves for this position." << endl
                << endl;
        }
        else {
            cout << " book moves:" << endl;
            for (unsigned i = 0; i<count; i++) {
                cout << '\t';
                Notation::image(*main_board,moves[i],Notation::OutputFormat::SAN,cout);
                cout << endl;
            }
            cout << endl;
        }
    }
    else if (cmd == "depth") {
    }
    else if (cmd_word == "level") {
        parseLevel(cmd_args, moves, minutes, incr);
        srctype = TimeLimit;
    }
    else if (cmd_word == "st") {
        process_st_command(cmd_args);
    }
    else if (cmd_word == "sd") {
        stringstream s(cmd_args);
        s >> ply_limit;
        srctype = FixedDepth;
    }
    else if (cmd_word == "time") {
        // my time left
        int t;
        stringstream s(cmd_args);
        s >> t;
        time_left = t*10; // convert from centiseconds to ms
    }
    else if (cmd_word == "otim") {
        // opponent's time left
        int t;
        stringstream s(cmd_args);
        s >> t;
        opp_time = t*10; // convert from centiseconds to ms
    }
    else if (cmd == "post") {
        post = 1;
    }
    else if (cmd == "nopost") {
        post = 0;
    }
    else if (cmd_word == "result") {
        // Game has ended
        theLog->setResult(cmd_args.c_str());
        save_game();
        game_end = true;
        gameMoves->removeAll();
        // Note: xboard may not send "new" before starting a new
        // game, at least in offline mode (-cp). To be safe,
        // execute "new" now, so we are ready for another game.
        do_command("new",board);
    }
    else if (cmd == "savegame") {
    }
    else if (cmd == "remove") {
        undo(board);
        undo(board);
    }
    else if (cmd == "undo") {
        undo(board);
    }
    else if (cmd == "resign") {
        // our opponent has resigned
        cout << debugPrefix() << "setting log result" << endl;
        if (computer_plays_white)
            theLog->setResult("0-1");
        else
            theLog->setResult("1-0");
        cout << debugPrefix() << "set log result" << endl;
    }
    else if (cmd == "draw") {
        // "draw" command. Requires winboard 3.6 or higher.
        if (accept_draw(board)) {
            // Notify opponent. don't assume draw is concluded yet.
            cout << "offer draw" << endl;
        }
        else if (doTrace) {
            cout << debugPrefix() << "draw declined" << endl;
        }
    }
    else if (cmd_word == "setboard") {
        start_fen = cmd_args;
        stringstream s(start_fen,ios::in);
        ChessIO::load_fen(s,board);
    }
    else if (cmd_word == "loadgame") {
        string filename = cmd_args;
        ifstream file(filename.c_str(),ios::in);
        loadgame(board,file);
    }
    else if (cmd == "edit") {
       for (int i = 0; i < 64; i++) {
          board.setContents(EmptyPiece,i);
       }
       board.setSecondaryVars();
       side = White;
       editMode = true;
    }
    else if (cmd == "analyze") {
        analyzeMode = 1;
        analyze(board);
    }
    else if (cmd == "exit") {
        if (analyzeMode) analyzeMode = 0;
    }
    else if (cmd == ".") {
        analyze_output(stats);
    }
    else if (cmd == "go") {
        forceMode = false;
        // set the side flag here - do not rely on the deprecated
        // "white" and "black" commands.
        computer_plays_white = board.sideToMove() == White;
        MoveSet movesToSearch;
        Move reply = search(searcher,board,movesToSearch,stats,false);
        if (!forceMode) {
            send_move(board,reply,stats);
        }
    }
    else if (cmd == "easy") {
        easy = true;
    }
    else if (cmd == "hard") {
        easy = false;
    }
    else if (cmd == "white" || cmd == "black") {
        computer_plays_white = (cmd == "white");
    }
    else if (cmd_word == "name") {
        // We've received the name of our opponent.
        opponent_name = cmd_args;
    }
    else if (cmd == "bogus" || cmd == "accepted") {
    }
    else if (cmd == "force") {
        forceMode = true;
    }
    else if (cmd_word == "rating") {
        stringstream args(cmd_args);
        args >> computer_rating;
        args >> opponent_rating;
        score_t contempt = contemptFromRatings(computer_rating,opponent_rating);
        if (doTrace) {
            cout << debugPrefix() << "contempt (calculated from ratings) = ";
            Scoring::printScore(contempt,cout);
            cout << endl;
        }
        if (searcher) searcher->setContempt(contempt);
    }
    else if (cmd == "computer") {
        computer = 1;
    }
    else if (cmd_word == "option") {
        // Winboard option command
        processWinboardOptions(cmd_args);
    }
    else if (cmd_word == "cores") {
        // Setting -c on the Arasan command line takes precedence over
        // what the GUI sets
        if (!cpusSet) {
           // set number of threads
           stringstream ss(cmd_args);
           if((ss >> options.search.ncpus).fail()) {
               cerr << "invalid value following 'cores'" << endl;
           } else {
              if (doTrace) {
                 cout << debugPrefix() << "setting cores to " << options.search.ncpus << endl;
              }
              options.search.ncpus = std::min<int>(options.search.ncpus,Constants::MaxCPUs);
              searcher->updateSearchOptions();
              searcher->setThreadCount(options.search.ncpus);
           }
        }
    }
    else if (cmd_word == "memory") {
        // Setting -H on the Arasan command line takes precedence over
        // what the GUI sets
        if (!memorySet) {
            // set memory size in MB
            stringstream ss(cmd_args);
            uint64_t mbs;
            ss >> mbs;
            if (ss.fail() || ss.bad()) {
               cout << debugPrefix() << "invalid value following 'memory'" << endl;
            } else {
               size_t mb_size = mbs*1024L*1024L;
               if (doTrace) {
                   cout << debugPrefix() << "setting hash size to " << mb_size << " bytes " << endl << (flush);
               }
               options.search.hash_table_size = mb_size;
               searcher->updateSearchOptions();
               searcher->resizeHash(options.search.hash_table_size);
           }
        }
        else {
            cout << debugPrefix() << "warning: memory command ignored due to -H on command line" << endl;
        }
    }
    else if  (cmd_word == "egtpath") {
        size_t space = cmd_args.find_first_of(' ');
        string type = cmd_args.substr(0,space);
        transform(type.begin(), type.end(), type.begin(), ::tolower);
        if (type.length() > 0) {
           transform(type.begin(), type.begin()+1, type.begin(), ::toupper);
        }
        string path;
        if (space != string::npos) path = cmd_args.substr(space+1);
#ifdef _WIN32
        // path may be in Unix format. Convert.
        string::iterator it = path.begin();
        while (it != path.end()) {
             if (*it == '/') {
                 *it = '\\';
             }
             it++;
        }
#endif
#ifdef SYZYGY_TBS
        // Unload existing tb set if in use and if path has changed
        if (options.tbPath() != path) {
           if (doTrace) cout << debugPrefix() << "unloading tablebases" << endl;
           unloadTb();
        }
        // Set the tablebase options. But do not initialize the
        // tablebases here. Defer until delayedInit is called again.
        // One reason to do this is that we may receive multiple
        // egtpath commands from Winboard.
        options.search.use_tablebases = 1;
        options.search.syzygy_path = path;
        if (doTrace) {
           cout << debugPrefix() << "setting Syzygy tb path to " << options.search.syzygy_path << endl;
        }
#endif
    }
    else {
        // see if it could be a move
        string movetext;
        if (cmd_word == "usermove") {
            // new for Winboard 4.2
            movetext = cmd_args;
        } else {
            movetext = cmd;
        }
        string::iterator it = movetext.begin();
        while (it != movetext.end() && !isalpha(*it)) it++;
        movetext.erase(movetext.begin(),it);
        if (doTrace) {
            cout << debugPrefix() << "move text = " << movetext << endl;
        }
        Move move;
        if ((move = text_to_move(board,movetext)) != NullMove) {
            if (game_end) {
                if (forceMode)
                    game_end = false;
                else {
                    if (doTrace) cout << debugPrefix() << "ignoring move " << movetext << " received after game end" << endl;
                    return true;
                }
            }
            if (doTrace) {
                cout << debugPrefix() << "got move: " << movetext << endl;
            }
            // make the move on the board
            execute_move(board,move);
            if (analyzeMode) {
               // re-enter analysis loop
               analyze(board);
            }
            Move reply;
            if (!forceMode && !analyzeMode) {
               // determine what to do with the pondering result, if
               // there is one.
               if (ponder_status == PonderStatus::Pending &&
                   MovesEqual(predicted_move,move) && !IsNull(ponder_move)) {
                  // We completed pondering already and we got a reply to
                  // this move (e.g. might be a forced reply).
                  if (doTrace) cout << debugPrefix() << "pondering complete already" << endl;
                  if (doTrace) {
                     cout << debugPrefix() << "sending ponder move ";
                     MoveImage(ponder_move,cout);
                     cout << endl << (flush);
                  }
                  reply = ponder_move;
                  stats = ponder_stats;
                  post_output(stats);
                  game_end = game_end || stats.end_of_game;
                  if (doTrace) cout << debugPrefix() << "game_end = " << game_end << endl;
                  predicted_move = ponder_move = NullMove;
               }
               else {
                  predicted_move = ponder_move = NullMove;
                  ponder_status = PonderStatus::None;
                  MoveSet movesToSearch;
                  reply = search(searcher,board,movesToSearch,stats,false);
                  // Note: we may know the game has ended here before
                  // we get confirmation from Winboard. So be sure
                  // we set the global game_end flag here so that we won't
                  // start pondering after the game is over.
                  game_end = game_end || stats.end_of_game;
                  if (doTrace) {
                     cout << debugPrefix() << "state = " << stats.state << endl;
                     cout << debugPrefix() << "game_end = " << game_end  << endl;
                  }
               }
               // Check for game end conditions like resign, draw acceptance, et
               if (check_pending(board)==PendingStatus::GameEnd) {
                  game_end = true;
               } else if (!forceMode) {
                  // call send_move even if game_end = true because we
                  // handle resignation, etc. there.
                  send_move(board,reply,stats);
               }
            }
            while (!forceMode && !analyzeMode && !game_end && !result_pending && !easy && time_target >= 100 /* 0.1 second */ &&
                   !IsNull(stats.best_line[1])) {
               PendingStatus result;
               // check pending commands again before pondering in case
               // we have a resign or draw, or a move has come in (no
               // good pondering if the opponent has replied already).
               if ((result = check_pending(board)) != PendingStatus::Nothing) {
                   return true;
               }
               ponder(board,reply,stats.best_line[1],uci);
               // check again for game-ending commands before we process
               // ponder result
               if (check_pending(board) == PendingStatus::GameEnd) {
                   return true;
               }
               if (analyzeMode || forceMode) {
                   break;
               }
               if (doTrace) {
                     cout << debugPrefix() << "ponder_status=";
                     if (ponder_status == PonderStatus::Hit) cout << "Hit";
                     else if (ponder_status == PonderStatus::NoHit) cout << "NoHit";
                     else if (ponder_status == PonderStatus::None) cout << "None";
                     else cout << "Pending";
                     cout << endl;
               }
               // We are done pondering. If we got a ponder hit
               // (opponent made our predicted move), then we are ready
               // to move now.
               if (ponder_status == PonderStatus::Hit && !IsNull(ponder_move) && !game_end
                   && !forceMode && !analyzeMode) {
                  // we got a reply from pondering
                  if (doTrace) {
                     cout << debugPrefix() << "sending ponder move" << endl;
                  }
                  stats = ponder_stats;
                  send_move(board,ponder_move,stats);
                  post_output(stats);
                  reply = ponder_move;
                  predicted_move = ponder_move = NullMove;
                  // Continue pondering now with the new predicted
                  // move from "stats".
               }
               else {
                  // Ponder move is not usuable, or at least not
                  // immediately. Exit the loop.
                  // if status is "Pending," we completed pondering early
                  // and may be able to use the pondering move later.
                  if (ponder_status == PonderStatus::Pending) {
                      if (doTrace) {
                          cout << debugPrefix() << "exiting ponder loop" << endl;
                      }
                      break;
                  }
               }
            }
        }
        if (doTrace) {
            cout << debugPrefix() << "out of ponder loop" << endl;
        }
    }
    return true;
}
