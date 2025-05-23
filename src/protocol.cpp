// Copyright 1997-2025 by Jon Dart. All Rights Reserved.
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
#include "movearr.h"
#include "movegen.h"
#include "notation.h"
#include "options.h"
#include "scoring.h"
#include "tunable.h"
#ifdef SYZYGY_TBS
#include "syzygy.h"
#endif
#include "tester.h"
#include "threadp.h"
#ifdef UNIT_TESTS
#include "unit.h"
#endif

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <regex>
#include <unordered_set>

using namespace std::placeholders;

const char * Protocol::UCI_DEBUG_PREFIX = "info string ";
const char * Protocol::CECP_DEBUG_PREFIX = "# ";

// handle CECP "level" command. Return true if parsed ok, false if error.
static bool parseLevel(const std::string &cmd, int &moveCount, float &minutes, int &incr)
{
    // not exact for decimal values, so must still check for validity
    const auto pattern = std::regex("^(\\d+)\\s+(\\d+)(\\:([\\d\\.]+))?\\s+([\\d\\.]+)$");

    std::smatch match;
    std::string movesStr, minutesStr, secondsStr, incStr;
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
        return false;
    }

    float time1, time2, floatincr;

    std::stringstream nums(minutesStr);
    nums >> time1;
    if (nums.bad()) {
       return false;
    }
    else if (secondsStr.size()) {
        std::stringstream nums2(secondsStr);
        nums2 >> time2;
        if (nums2.bad()) {
            return false;
        } else {
            minutes = time1 + time2/60;
        }
    } else {
        minutes = time1;
    }
    std::stringstream movesStream(movesStr);
    movesStream >> moveCount;
    if (movesStream.bad()) {
        return false;
    }

    std::stringstream incStream(incStr);
    incStream >> floatincr;
    if (incStream.bad()) {
        return false;
    } else {
        // Winboard increment is in seconds, convert to our
        // internal value (milliseconds).
        incr = int(1000*floatincr);
    }
    return true;
}

static Move text_to_move(const Board &board, const std::string &input) {
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

    // split a command line into a verb (cmd_word) and arguments (cmd_args)
static void split_cmd(const std::string &cmd, std::string &cmd_word, std::string &cmd_args) {
   size_t space = cmd.find_first_of(' ');
   cmd_word = cmd.substr(0,space);
   cmd_args = cmd.substr(cmd_word.length());
   size_t start = cmd_args.find_first_not_of(' ');
   if (start != std::string::npos) {
      cmd_args.erase(0,start);
   }
}

Protocol::Protocol(const Board &board, bool traceOn, bool icsMode, bool cpus_set, bool memory_set)
    : verbose(false), post(false), searcher(nullptr), last_move(NullMove),
      last_move_image("(null)"), last_computer_move(NullMove), time_left(0), opp_time(0),
      minutes(5.0), incr(0), winc(0), binc(0), computer(false), computer_plays_white(false),
      ics(icsMode), forceMode(false), analyzeMode(false), editMode(false), side(White), moves(0),
      ponder_board(new Board()), main_board(new Board(board)), ponder_status(PonderStatus::None),
      predicted_move(NullMove), ponder_move(NullMove), best_move(NullMove), time_target(0),
      last_time_target(Constants::INFINITE_TIME), computer_rating(0), opponent_rating(0),
      doTrace(traceOn), easy(false), game_end(false), result_pending(false),
      last_score(Constants::MATE), ecoCoder(nullptr), xboard42(false), srctype(TimeLimit),
      time_limit(Constants::INFINITE_TIME), ply_limit(Constants::MaxPly), lastAdded(0), uci(false),
      movestogo(0), ponderhit(false), uciWaitState(false), cpusSet(cpus_set), memorySet(memory_set),
      debugPrefix(globals::debugPrefix) {
    ecoCoder = new ECO();
    searcher = new SearchController();
    searcher->registerPostFunction(std::bind(&Protocol::post_output,this,_1));
    searcher->registerMonitorFunction(std::bind(&Protocol::monitor,this,_1,_2));
}

Protocol::~Protocol()
{
    delete main_board;
    delete ponder_board;
    delete ecoCoder;
    delete searcher;
}

// Read from input, outside of the search
void Protocol::poll(bool &polling_terminated)
{
    while (!polling_terminated) {
        // execute pending commands before getting more input
        if (!do_all_pending(*main_board)) {
            break;
        }
        // blocking read
        if (!input.readInput(pending, inputMtx)) {
            polling_terminated = true;
        }
    }
    if (doTrace) std::cout << debugPrefix << "exited polling loop" << std::endl;
    // handle termination.
    save_game();
    if (doTrace) {
        std::cout << debugPrefix << "terminating" << std::endl;
    }
}

Move Protocol::get_move(const std::string &cmd_word, const std::string &cmd_args) {
    std::string move;
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
        std::cout << debugPrefix << "move text = " << move << std::endl;
    }
    return text_to_move(*main_board,move);
}

bool Protocol::do_all_pending(Board &board)
{
    bool retVal = true;
    bool debug = debugPrefix.size() > 0;
    if (doTrace && debug) std::cout << debugPrefix << "in do_all_pending" << std::endl;
    std::string cmd;
    while (!globals::polling_terminated && popPending(cmd)) {
        if (doTrace && debug) {
            std::cout << debugPrefix << "pending command(a): " << cmd << std::endl;
        }
        if (uciWaitState) {
            // can get here if we completed pondering early
            processCmdInWaitState(cmd);
        }
        else if (!do_command(cmd,board)) {
            if (doTrace && debug) {
                std::cout << "command \"" << cmd << "\" returned false" << std::endl;
            }
            retVal = false;
            break;
        }
    }
    if (doTrace && debug) {
        std::cout << debugPrefix << "out of do_all_pending, list size=" << pending.size() << std::endl;
    }
    return retVal;
}

Protocol::PendingStatus Protocol::check_pending(Board &board) {
    if (doTrace) std::cout << debugPrefix << "in check_pending" << std::endl;
    PendingStatus retVal = PendingStatus::Nothing;
    while (true) {
        std::string cmd;
        {
            std::unique_lock<std::mutex> lock(inputMtx);
            if (pending.empty()) break;
            cmd = pending.front();
            std::string cmd_word, cmd_args;
            split_cmd(cmd,cmd_word,cmd_args);
            if (cmd_word == "result" ||
                cmd == "new" ||
                cmd == "quit" ||
                cmd == "resign") {
                if (doTrace) std::cout << debugPrefix << "game end signal in pending stack" << std::endl;
                retVal = PendingStatus::GameEnd;
                break;
            }
            else if (cmd_word == "usermove" || text_to_move(board,cmd) != NullMove) {
                if (doTrace) std::cout << debugPrefix << "move in pending stack" << std::endl;
                retVal = PendingStatus::UserMove;
                break;
            }
            else {
                // remove command from pending stack
                if (doTrace) std::cout << debugPrefix << "erasing " << *(pending.begin()) << std::endl;
                pending.erase(pending.begin());
            }
        }
        if (doTrace) {
            std::cout << debugPrefix << "calling do_command from check_pending" << (std::flush) << std::endl;
        }
        // execute command after unlocking mutex
        do_command(cmd,board);
    }
    return retVal;
}

void Protocol::sendPong(const std::string &arg)
{
    std::cout << "pong " << arg << std::endl << (std::flush);
}

void Protocol::process_st_command(const std::string &cmd_args)
{
   std::stringstream s(cmd_args);
   float time_limit_sec;
   // we allow fractional seconds although UI may not support it
   s >> time_limit_sec;
   if (s.bad() || time_limit_sec <= 0.0) {
      std::cout << debugPrefix << "illegal value for st command: " << cmd_args << std::endl;
      return;
   } else {
      srctype = FixedTime;
   }
   // convert to ms. and subtract a buffer to prevent losses on time
   time_limit = int(time_limit_sec * 1000 - std::min<int>(int(time_limit_sec*100),100));
}

int Protocol::getIncrUCI(const ColorType c) {
    return c == White ? winc : binc;
}

bool Protocol::accept_draw(Board &board) {
   if (doTrace)
      std::cout << debugPrefix << "in accept_draw" << std::endl;
   // Code to handle draw offers.
   const ColorType mySide = computer_plays_white ? White : Black;
   int rating_diff = opponent_rating - computer_rating;
   // ignore draw if we have just started searching
   if (last_score == Constants::MATE) {
      return false;
   }
   // If it's a 0-increment game and the opponent has < 1 minute,
   // and we have more time, decline
   int inc = uci ? getIncrUCI(OppositeColor(mySide)) : incr;
   if (!computer && inc == 0 && opp_time < 6000 && time_left > opp_time) {
      return false;
   }
   // See if we do not have enough material to mate
   const Material &ourmat = board.getMaterial(mySide);
   const Material &oppmat = board.getMaterial(OppositeColor(mySide));
   if (ourmat.noPawns() && (ourmat.kingOnly() || ourmat.infobits() == Material::KB ||
                            ourmat.infobits() == Material::KN)) {
      // We don't have mating material
      if (doTrace)
         std::cout << debugPrefix << "no mating material, accept draw" << std::endl;
      return true;
   }
   // accept a draw in pawnless endings with even material, unless
   // our score is way positive
   if (ourmat.noPawns() && oppmat.noPawns() &&
       ourmat.materialLevel() <= 5 &&
      last_score < Scoring::PAWN_VALUE) {
      if (doTrace)
         std::cout << debugPrefix << "pawnless ending, accept draw" << std::endl;
      return true;
   }
#ifdef SYZYGY_TBS
   const Material &wMat = board.getMaterial(White);
   const Material &bMat = board.getMaterial(Black);
   if (globals::options.search.use_tablebases &&
       wMat.men() + bMat.men() <= globals::EGTBMenCount) {
       if (doTrace)
           std::cout << debugPrefix << "checking tablebases .." << std::endl;
       // accept a draw when the tablebases say it's a draw
       score_t tbscore;
       if (SyzygyTb::probe_wdl(board,tbscore,true) && std::abs(tbscore) <= SyzygyTb::CURSED_SCORE) {
           if (doTrace) {
               std::cout << debugPrefix << "tablebase score says draw" << std::endl;
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
      std::cout << debugPrefix << "checking draw score .." << std::endl;
   ColorType tmp = board.sideToMove();
   board.setSideToMove(mySide);
   score_t draw_score = searcher->drawScore(board);
   board.setSideToMove(tmp);
   const score_t threshold = Scoring::PAWN_VALUE/4;
   if (doTrace) {
      std::cout << debugPrefix << "rating_diff = " << rating_diff << std::endl;
      std::cout << debugPrefix << "draw_score = " << draw_score << std::endl;
      std::cout << debugPrefix << "last_score = " << last_score << std::endl;
      std::cout << debugPrefix << "threshold = " << threshold << std::endl;
   }
   return draw_score > threshold && last_score <= draw_score;
}


void Protocol::do_help() {
#ifndef _WIN32
   if (!isatty(1)) return;
#endif
   std::cout << "analyze:         enter Winboard analyze mode" << std::endl;
   std::cout << "black:           set computer to play Black" << std::endl;
   std::cout << "bk:              show book moves" << std::endl;
   std::cout << "computer:        used to indicate the opponent is a computer" << std::endl;
   std::cout << "draw:            offer a draw" << std::endl;
   std::cout << "easy:            disable pondering" << std::endl;
   std::cout << "edit:            enter Winboard edit mode" << std::endl;
   std::cout << "force:           disable computer moving" << std::endl;
   std::cout << "go:              start searching" << std::endl;
   std::cout << "hard:            enable pondering" << std::endl;
   std::cout << "hint:            compute a hint for the current position" << std::endl;
   std::cout << "ics <hostname>:  set the name of the ICS host" << std::endl;
   std::cout << "level <a b c>:   set the time control:" << std::endl;
   std::cout << "  a -> moves to time control" << std::endl;
   std::cout << "  b -> minutes per game" << std::endl;
   std::cout << "  c -> increment in seconds" << std::endl;
   std::cout << "name <string>:   set the name of the opponent" << std::endl;
   std::cout << "new:             start a new game" << std::endl;
   std::cout << "nopost:          disable output during search" << std::endl;
   std::cout << "otim <int>:      set opponent time remaining (in centiseconds)" << std::endl;
   std::cout << "post:            show output during search" << std::endl;
   std::cout << "quit:            terminate the program" << std::endl;
   std::cout << "remove:          back up a full move" << std::endl;
   std::cout << "resign:          resign the current game" << std::endl;
   std::cout << "result <string>: set the game result (0-1, 1/2-1/2 or 1-0)" << std::endl;
   std::cout << "sd <x>:          limit thinking to depth x" << std::endl;
   std::cout << "setboard <FEN>:  set board to a specified FEN string" << std::endl;
   std::cout << "st <x>:          limit thinking to x seconds" << std::endl;
   std::cout << "test <epd_file> -d <depth> -t <sec/move> <-x iter> <-N pvs> <-v>:  run test suite" << std::endl;
   std::cout << "time <int>:      set computer time remaining (in centiseconds)" << std::endl;
   std::cout << "undo:            back up a half move" << std::endl;
   std::cout << "white:           set computer to play White" << std::endl;
   std::cout << "test <file> <-t seconds> <-x # moves> <-v> <-o outfile>: "<< std::endl;
   std::cout << "   - run an EPD testsuite" << std::endl;
   std::cout << "eval:            evaluate current position." << std::endl;
   std::cout << "perft <depth>:   compute perft value for a given depth" << std::endl;
}


void Protocol::save_game() {
   if (uci) return;                               // not supported
   if (doTrace) std::cout << debugPrefix << "in save_game" << std::endl;
   if (doTrace) std::cout << debugPrefix << "game_moves=" << globals::gameMoves->num_moves() << std::endl;
   if (globals::gameMoves->num_moves() == 0 || !globals::options.games.store_games) {
      if (doTrace) std::cout << debugPrefix << "out of save_game" << std::endl;
      return;
   }
   if (globals::game_file.is_open()) {
      std::vector<ChessIO::Header> headers;
      std::string opening_name, eco;
      if (ecoCoder) {
         if (doTrace) std::cout << debugPrefix << "calling classify" << std::endl;
         ecoCoder->classify(*globals::gameMoves,eco,opening_name);
         headers.push_back(ChessIO::Header("ECO",eco));
      }
      std::stringstream crating, orating;
      crating << computer_rating;
      orating << opponent_rating;

      if (hostname.length() > 0) {
          headers.push_back(ChessIO::Header("Site",hostname));
      }

      if (computer_plays_white) {
         headers.push_back(ChessIO::Header("Black",opponent_name.length() > 0 ? opponent_name : "?"));
         if (computer_rating)
             headers.push_back(ChessIO::Header("WhiteElo",crating.str()));
         if (opponent_rating)
             headers.push_back(ChessIO::Header("BlackElo",orating.str()));
      }
      else {
         headers.push_back(ChessIO::Header("White",opponent_name.length() > 0 ? opponent_name : "?"));
         if (opponent_rating)
             headers.push_back(ChessIO::Header("WhiteElo",orating.str()));
         if (computer_rating)
             headers.push_back(ChessIO::Header("BlackElo",crating.str()));
      }
      if (start_fen.size()) {
         // we had a non-standard starting position for the game
          headers.push_back(ChessIO::Header("FEN",start_fen));
      }
      std::stringstream timec;
      if (moves != 0) {
          timec << moves << '/';
      }
      timec << minutes*60;
      std::string timestr = timec.str();
      if (incr) {
         timec << '+' << std::fixed << std::setprecision(2) << incr/1000.0F;
         timestr = timec.str();
         size_t per = timestr.find('.');
         if (per != std::string::npos) {
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
      std::string result = globals::gameMoves->getResult();
      if (!(ChessIO::store_pgn(globals::game_file, *globals::gameMoves,
                               computer_plays_white ? White : Black, result, headers))) {
          if (doTrace) {
              std::cout << debugPrefix << "error in save_game: " << strerror(errno) << std::endl;
          }
      }
   }
   if (doTrace) std::cout << debugPrefix << "out of save_game" << std::endl;
}

void Protocol::move_image(const Board &board, Move m, std::ostream &buf, bool is_uci) {
    Notation::image(board,m,is_uci ? Notation::OutputFormat::UCI : Notation::OutputFormat::WB,buf);
}

void Protocol::uciOut(int depth, score_t score, time_t time,
uint64_t nodes, uint64_t tb_hits, const std::string &best_line_image, int multipv) {
   std::stringstream s;
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
   std::cout << s.str() << std::endl;
}


void Protocol::uciOut(const Statistics &s) {
   uciOut(s.depth,s.display_value,searcher->getElapsedTime(),
      s.num_nodes,stats.tb_hits,
      s.best_line_image,0);
}

void Protocol::post_output(const Statistics &s) {
   last_score = s.value;
   score_t score = s.display_value;
   if (score == Constants::INVALID_SCORE) {
      return; // no valid score yet
   }
   if (verbose) {
       if (uci) {
           if (globals::options.search.multipv > 1) {
               // output stats only when multipv array has been filled
               if (s.multipv_count == s.multipv_limit) {
                   for (unsigned i = 0; i < s.multipv_limit; i++) {
                       uciOut(s.multi_pvs[i].depth,
                              s.multi_pvs[i].display_value,
                              searcher->getElapsedTime(),
                              s.num_nodes,
                              s.tb_hits,
                              s.multi_pvs[i].best_line_image,
                              i+1);
                   }
               }
           }
           else {
               uciOut(s);
           }
       }
   }
   else if (post) {
       // "post" output for Winboard
       std::cout << s.depth << ' ' <<
           static_cast<int>((score*100)/Scoring::PAWN_VALUE) << ' ' <<
           searcher->getElapsedTime()/10 << ' ' <<
           s.num_nodes << ' ' <<
           s.best_line_image << std::endl << (std::flush);
   }
}

bool Protocol::processPendingInSearch(SearchController *controller, const std::string &cmd, bool &exit)
{
    if (doTrace) {
        std::cout << debugPrefix << "command in search: " << cmd << std::endl;
    }
    std::string cmd_word, cmd_args;
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
                    std::cout << debugPrefix << "time_limit=" << time_limit << " movestogo=" <<
                        movestogo << " time_left=" << time_left << " opp_time=" << opp_time << std::endl;
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
        if (doTrace) std::cout << debugPrefix << "? received: terminating." << std::endl;
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
            std::cout << debugPrefix << "received_result: " << cmd << std::endl;
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
                std::cout << debugPrefix << "cmd in search not processed: " << cmd << " (expected move)";
            }
            return false;
        } else if (!game_end) {
            last_move = rmove;
            if (doTrace) {
                std::cout << debugPrefix << "predicted move = ";
                MoveImage(predicted_move,std::cout);
                std::cout << " last move = ";
                MoveImage(last_move,std::cout);
                std::cout << std::endl;
            }
            if (forceMode || analyzeMode || !controller->pondering()) {
                controller->terminateNow();
                return false;
            }
            else if (!IsNull(predicted_move) && MovesEqual(predicted_move,last_move)) {
                // A move arrived during a ponder search and it was
                // the predicted move, in other words we got a ponder hit.
                if (doTrace) {
                    std::cout << debugPrefix << "ponder ok" << std::endl;
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
                if (doTrace) std::cout << debugPrefix << "ponder not ok" << std::endl;
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

bool Protocol::monitor(SearchController *s, const Statistics &) {
    // check for input and examine commands if any were received
    bool exit = false;
    if (input.checkInput(pending, inputMtx)) {
        // special case for wait state - check only for commands
        // that may exit the wait state
        std::unique_lock<std::mutex> lock(inputMtx);
        while (uciWaitState && !pending.empty()) {
            std::string cmd(pending.front());
            pending.erase(pending.begin());
            processCmdInWaitState(cmd);
        }
        auto it = pending.begin();
        while (it != pending.end() && !exit) {
            if (processPendingInSearch(s,*it,exit)) {
                it = pending.erase(it);
            } else {
                it++;
            }
        }
    }
    return exit;
}

void Protocol::edit_mode_cmds(Board &board,ColorType &c,const std::string &cmd)
{
    std::unordered_set<char> pieces({'P','N','B','R','Q','K','p','n','b','r','q','k'});
    if (cmd == "white") {
       c = White;
    }
    else if (cmd == "black") {
       c = Black;
    }
    else if (cmd == "#") {
       for (int i = 0; i < 64; i++) {
          board.setContents(EmptyPiece,i);
       }
    }
    else if (cmd == "c") {
       c = OppositeColor(side);
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
          Piece p = EmptyPiece;
          switch (tolower(cmd[0])) {
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

void Protocol::calcTimes(bool pondering, ColorType c, timeMgmt::Times &times) {
    if (srctype == FixedTime) {
        times.time_target = time_limit;
        times.extra_time = 0;
    } else {
        timeMgmt::calcTimeLimit(moves, uci ? getIncrUCI(c) : incr, time_left, opp_time, pondering,
                                times);
    }
    if (doTrace) {
        std::cout << debugPrefix << "time_target = " << times.time_target << std::endl;
        std::cout << debugPrefix << "xtra time = " << times.extra_time << std::endl;
    }
}

template <bool isUCI> void Protocol::ponder(Board &board, Move move, Move predicted_reply) {
    ponder_status = PonderStatus::None;
    ponder_move = NullMove;
    ponder_stats.clear();
    if (isUCI || (!IsNull(move) && !IsNull(predicted_reply))) {
        if (!isUCI) {
            if (doTrace) {
                std::cout << debugPrefix << "in ponder(), move = ";
                MoveImage(move,std::cout);
                std::cout << " predicted reply = ";
                MoveImage(predicted_reply,std::cout);
                std::cout << std::endl;
            }
            predicted_move = predicted_reply;
            // We have already set up the ponder board with the board
            // position after our last move. Now make the move we predicted.
            //
            assert(legalMove(*ponder_board,predicted_reply));
            //
            // We must add this move to the global move list,
            // otherwise repetition detection will be broken. Note,
            // though, that in case of a ponder miss we must later
            // remove this move.
            //
            globals::gameMoves->add_move(board,predicted_reply,"",true,false);
            ponder_board->doMove(predicted_reply);
        }
        time_target = Constants::INFINITE_TIME;
        // in reduced strength mode, limit the ponder search time
        // (do not ponder indefinitely)
        if (globals::options.search.strength < 100) {
            time_target = last_time_target;
            if (doTrace) std::cout << debugPrefix << "limiting ponder time to " <<
                             time_target << std::endl;
        }
        if (doTrace) {
            std::cout << debugPrefix << "starting to ponder" << std::endl;
        }
        ponder_status = PonderStatus::Pending;
        if (srctype == FixedDepth) {
            ponder_move = searcher->findBestMove(
                isUCI ? *main_board : *ponder_board,
                srctype,
                0,
                0,
                ply_limit, true, isUCI,
                ponder_stats,
                (doTrace) ? TalkLevel::Debug : TalkLevel::Silent);
        }
        else {
            time_target = last_time_target = Constants::INFINITE_TIME;
            ponder_move = searcher->findBestMove(
                isUCI ? *main_board : *ponder_board,
                FixedTime,
                time_target,
                0,            /* extra time allowed */
                Constants::MaxPly,           /* ply limit */
                true,         /* background */
                isUCI,
                ponder_stats,
                (doTrace) ? TalkLevel::Debug : TalkLevel::Silent);
        }
        if (doTrace) {
            std::cout << debugPrefix << "done pondering" << std::endl;
        }
        // Ensure "ping" response is set if ping was received while
        // searching:
        if (!isUCI) {
            if (doTrace) {
                std::cout << debugPrefix << "handling pending commands" << std::endl;
            }
            std::unique_lock<std::mutex> lock(inputMtx);
            auto it = pending.begin();
            bool exit = false;
            while (it != pending.end() && !exit) {
                if (doTrace) {
                    std::cout << debugPrefix << "handling pending command: " << *it << std::endl;
                }
                if (processPendingInSearch(searcher,*it,exit)) {
                    it = pending.erase(it);
                } else {
                    it++;
                }
            }
        }
    }
    last_computer_move = ponder_move;
    last_computer_stats = ponder_stats;
    // Clean up the global move array, if we got no ponder hit.
    if (!isUCI && globals::gameMoves->num_moves() > 0 && globals::gameMoves->back().wasPonder()) {
        globals::gameMoves->remove_last();
    }
    if (doTrace) {
        std::cout << debugPrefix << "ponder move = ";
        MoveImage(ponder_move,std::cout);
        std::cout << std::endl;
        std::cout << debugPrefix << "out of ponder()" << std::endl;
    }
}

Move Protocol::search(Board &board,
                      const MoveSet &movesToSearch, Statistics &s, bool infinite)
{
    last_stats.clear();
    last_score = Constants::MATE;
    ponder_move = NullMove;
    if (doTrace) std::cout << debugPrefix << "in search()" << std::endl;

    Move move = NullMove;
    s.fromBook = false;
    // Note: not clear what the semantics should be when "searchmoves"
    // is specified and using "own book." Currently we force a search
    // in this case and ignore the book moves.
    if (!infinite && globals::options.book.book_enabled && movesToSearch.empty()) {
        move = globals::openingBook.pick(board, doTrace);
        if (!IsNull(move)) s.fromBook = true;
    }

    if (IsNull(move)) {
        // no book move
        s.clear();
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
                s,
                level,
                excludes,
                movesToSearch);
        }
        else {
            timeMgmt::Times times;
            if (infinite) {
                times.time_target = Constants::INFINITE_TIME;
                times.extra_time = 0;
            } else {
                calcTimes(false,board.sideToMove(),times);
            }
            time_target = last_time_target = times.time_target;
            move = searcher->findBestMove(board,
                srctype,
                times.time_target,
                times.extra_time,
                Constants::MaxPly, false, uci,
                s,
                level,
                excludes,
                movesToSearch);
        }
        if (doTrace) {
            std::cout << debugPrefix << "search done : move = ";
            MoveImage(move,std::cout);
            std::cout << std::endl;
        }
        last_stats = s;
    }
    else {
        if (ics || uci) {
            std::vector< Move > choices;
            int moveCount = 0;
            if (globals::options.book.book_enabled) {
               moveCount = globals::openingBook.book_moves(board,choices);
            }
            std::stringstream out;
            out << "book moves (";
            for (int i=0;i<moveCount;i++) {
                Notation::image(board,choices[i],Notation::OutputFormat::SAN,out);
                if (i < moveCount-1)
                    out << ", ";
            }
            out << "), choosing ";
            Notation::image(board,move,Notation::OutputFormat::SAN,out);
            if (uci) {
                std::cout << UCI_DEBUG_PREFIX << out.str() << std::endl;
            }
            if (ics) {
                if (computer)
                    std::cout << "tellics kibitz " << out.str() << std::endl;
                else
                    std::cout << "tellics whisper " << out.str() << std::endl;
            }
        }
        s.clear();
    }
    last_computer_move = move;
    last_computer_stats = s;
    return move;
}

// return true if current board position is a draw by the
// rules of chess. If so, also set the "reason"
static bool isDraw(const Board &board, const Statistics &last_stats, std::string &reason) {
   if (last_stats.state == Stalemate) {
       reason = "Stalemate";
       return true;
   }
   else if (last_stats.value < Constants::MATE-1 &&
            board.state.moveCount >= 100) {
       // Note: do not count as draw if last move delivered checkmate!
       reason = "50 move draw";
       return true;
   }
   else if (board.materialDraw()) {
       reason = "Insufficient material";
       return true;
   }
   else if (board.repetitionDraw()) {
       reason = "Repetition";
       return true;
   }
   else if (last_stats.state == Draw) {
       return true;
   }
   return false;
}

static void kibitz(SearchController *searcher, bool computer, Statistics &last_stats, bool multithread) {
    std::stringstream s;
    if (computer)
        s << "tellics kibitz ";
    else
        s << "tellics whisper ";
    s << "time=" << std::fixed << std::setprecision(2) <<
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
        s << " cpu=" << std::fixed << std::setprecision(2) <<
            searcher->getCpuPercentage() << '%';
    }
#endif
    if (last_stats.best_line_image.length()) {
        s << " pv: " << last_stats.best_line_image;
    }
    std::cout << s.str() << std::endl;
}

void Protocol::send_move(Board &board, Move &move, Statistics &s) {
    // In case of multi-pv, make sure the high-scoring move is
    // sent as best move.
    if (s.multipv_limit > 1) {
        move = s.multi_pvs[0].best;
    }
    last_move = move;
    last_stats = s;
    ColorType sideToMove = board.sideToMove();
    if (s.state == Terminated) {
        // A ponder search was interrupted because Winboard set the
        // game result. We should not send a move or try to set the result.
        return;
    }
    else if (last_stats.state == Resigns) {
        // Don't resign a zero-increment game if the opponent is short
        // on time
        if (srctype == TimeLimit && incr == 0 && opp_time < 2000) {
            // reset flag - we're not resigning
            s.end_of_game = game_end = false;
        }
        else {
            if (computer_plays_white) {
                globals::gameMoves->setResult("0-1 {White resigns}");
                std::cout << "0-1 {White resigns}" << std::endl;
            }
            else {
                globals::gameMoves->setResult("1-0 {Black resigns}");
                std::cout << "1-0 {Black resigns}" << std::endl;
            }
            game_end = true;
            // Don't send the move
            return;
        }
    }
    if (!game_end) {
        if (!uci) {
            std::string reason;
            if (isDraw(board,last_stats,reason)) {
                // A draw position exists before we even move (probably
                // because the opponent did not claim the draw).
                // Send the result command to claim the draw.
                if (doTrace) {
                    std::cout << debugPrefix << "claiming draw before move";
                    if (reason.length()) std::cout << " (" << reason << ")";
                    std::cout << std::endl;
                }
                std::cout << "1/2-1/2 {" << reason << "}" << std::endl;
                // Wait for Winboard to send a "result" command before
                // actually concluding it's a draw.
                // Set flag to indicate we are waiting.
                result_pending = true;
                return;
            }
        }
        if (!IsNull(move)) {
            std::stringstream img;
            Notation::image(board,last_move,Notation::OutputFormat::SAN,img);
            last_move_image = img.str();
            std::stringstream movebuf;
            move_image(board,last_move,movebuf,uci);

            if (uci) {
                std::cout << "bestmove " << movebuf.str();
                if (!easy && !IsNull(s.best_line[1])) {
#ifdef _DEBUG
                    BoardState bs(board.state);
                    board.doMove(move);
                    // ensure ponder move is legal
                    assert(legalMove(board,s.best_line[1]));
                    board.undoMove(move,bs);
#endif
                    std::stringstream ponderbuf;
                    move_image(board,s.best_line[1],ponderbuf,uci);
                    std::cout << " ponder " << ponderbuf.str() << std::endl;
                }
                else {
                    std::cout << std::endl;
                }
                globals::gameMoves->add_move(board,last_move,last_move_image,&last_stats,false);
                // Perform learning (if enabled):
                learn(board,*globals::gameMoves,doTrace);
            }
            else { // Winboard
                globals::gameMoves->add_move(board,last_move,last_move_image,&last_stats,false);
                // Perform learning (if enabled):
                learn(board,*globals::gameMoves,doTrace);
                // Execute the move and prepare to ponder.
                board.doMove(last_move);
                *ponder_board = board;
                std::string reason;
                if (isDraw(board,last_stats,reason)) {
                    // It will be a draw after we move (by rule).
                    // Following the current protocol standard, send
                    // "offer draw" and then send the move (formerly
                    // we would send the move then send the result,
                    // which is incorrect).
#ifdef _TRACE
                    std::cout << debugPrefix << "draw predicted after move";
                    if (reason.length()) std::cout << " (" << reason << ")";
                    std::cout << std::endl;
#endif
                    std::cout << "offer draw" << std::endl;

                }
                if (xboard42) {
                    std::cout << "move " << movebuf.str() << std::endl;
                }
                else {
                    std::cout << globals::gameMoves->num_moves()/2 << ". ... ";
                    std::cout << movebuf.str() << std::endl;
                }
                std::cout << (std::flush);
            }
        }
        else if (uci) {
            // must always send a "bestmove" command even if no move is available, to
            // acknowledge the previous "stop" command.
            std::cout << "bestmove 0000" << std::endl;
        } else {
            if (doTrace) std::cout << debugPrefix << "warning : move is null" << std::endl;
        }
        if (ics && ((srctype == FixedDepth && searcher->getElapsedTime() >= 250) || time_target >= 250) &&
            s.display_value != Constants::INVALID_SCORE) {
            kibitz(searcher,computer,last_stats,globals::options.search.ncpus>1);
        }
    }
    if (uci) return; // With UCI, GUI is in charge of game end detection
    // We already checked for draws, check now for other game end
    // conditions.
    if (!game_end) {
        if (last_stats.value >= Constants::MATE-1) {
            if (doTrace) std::cout << debugPrefix << "last_score = mate" << std::endl;
            if (sideToMove == White) {
                globals::gameMoves->setResult("1-0");
                std::cout << "1-0 {White mates}" << std::endl;
            }
            else {
                globals::gameMoves->setResult("0-1");
                std::cout << "0-1 {Black mates}" << std::endl;
            }
            game_end = true;
        }
        else if (last_stats.state == Checkmate) {
            if (doTrace) std::cout << debugPrefix << "state = Checkmate" << std::endl;
            if (sideToMove == White) {
                globals::gameMoves->setResult("0-1");
                std::cout << "0-1 {Black mates}" << std::endl;
            }
            else {
                globals::gameMoves->setResult("1-0");
                std::cout << "1-0 {White mates}" << std::endl;
            }
            game_end = true;
        }
    }
}

void Protocol::processCmdInWaitState(const std::string &cmd) {
    if (doTrace) {
        std::cout << debugPrefix << "got command in wait state: " << cmd << (std::flush) << std::endl;
    }
    // we expect a "stop" or "ponderhit"
    if (cmd == "ponderhit" || cmd == "stop") {
        send_move(*main_board,last_computer_move,last_computer_stats);
        uciWaitState = false;
    } else if (cmd == "quit") {
        uciWaitState = false;
        globals::polling_terminated = true;
    } else if (cmd == "ucinewgame") {
        // Arena at least can send this w/o "stop"
        uciWaitState = false;
    }
}

Move Protocol::analysisSearch(const Board &board)
{
    last_stats.clear();
    last_score = Constants::MATE;

    stats.clear();
    if (doTrace) {
        std::cout << debugPrefix << "entering analysis search" << std::endl;
    }
    Move move = searcher->findBestMove(board,
                                       FixedTime,
                                       Constants::INFINITE_TIME,
                                       0,
                                       Constants::MaxPly, false, uci,
                                       stats,
                                       TalkLevel::Whisper);
    if (doTrace) {
        std::cout << debugPrefix << "search done : move = ";
        MoveImage(move,std::cout);
        std::cout << std::endl;
    }

    last_stats = stats;
    post_output(stats);

    return move;
}

void Protocol::doHint() {
    // try book move first
    std::vector<Move> bookMoves;
    unsigned count = 0;
    if (globals::options.book.book_enabled) {
        count = globals::openingBook.book_moves(*main_board,bookMoves);
    }
    if (count > 0) {
        if (count == 1)
            std::cout << "Book move: " ;
        else
            std::cout << "Book moves: ";
        for (unsigned i = 0; i<count; i++) {
            Notation::image(*main_board,bookMoves[i],Notation::OutputFormat::SAN,std::cout);
            if (i<count-1) std::cout << ' ';
        }
        std::cout << std::endl;
        return;
    }
    else {
        // no book move, see if we have a ponder move
        const std::string &img = last_stats.best_line_image;
        if (img.length()) {
            std::string::const_iterator it = img.begin();
            while (it != img.end() && !isspace(*it)) it++;
            std::string hint;
            if (it != img.end()) {
                it++;
                while (it != img.end() && !isspace(*it)) hint += *it++;
            }
            if (hint.length()) {
                std::cout << "Hint: " << hint << std::endl;
                return;
            }
        }
    }
    // no ponder move or book move. If we are already pondering but
    // have no ponder move we could wait a while for a ponder result,
    // but we just return for now.
    if (searcher->pondering()) return;
    if (doTrace) std::cout << debugPrefix << "computing hint" << std::endl;

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
        std::cout << "Hint: ";
        Notation::image(*main_board,move,Notation::OutputFormat::SAN,std::cout);
        std::cout << std::endl;
    }
}

void Protocol::analyze_output(const Statistics &s) {
    std::cout << "stat01: " <<
        searcher->getElapsedTime() << " " << stats.num_nodes << " " <<
        s.depth << " " <<
        s.mvleft << " " << s.mvtot << std::endl;
}

void Protocol::analyze(Board &board)
{
    if (doTrace) std::cout << debugPrefix << "entering analysis mode" << std::endl;
    while (analyzeMode) {
        Board previous(board);
        analysisSearch(board);
        if (doTrace) std::cout << debugPrefix << "analysis mode: out of search" << std::endl;
        // Process commands received while searching; exit loop
        // if "quit" seen.
        if (!do_all_pending(board)) {
            break;
        }
        while (board.hashCode() == previous.hashCode() && analyzeMode) {
            // The user has given us no new position to search. We probably
            // got here because the search has terminated early, due to
            // forced move, forced mate, tablebase hit, or hitting the max
            // ply depth. Wait here for more input.
            if (doTrace) std::cout << debugPrefix << "analysis mode: wait for input" << std::endl;
            if (!input.readInput(pending, inputMtx)) {
                break;
            }
            std::string cmd;
            while (popPending(cmd)) {
                std::string cmd_word, cmd_arg;
                split_cmd(cmd,cmd_word,cmd_arg);
                if (doTrace) {
                    std::cout << debugPrefix << "processing cmd in analysis mode: " << cmd << std::endl;
                }
                if (cmd == "undo" || cmd == "setboard") {
                    do_command(cmd,board);
                }
                // Technically "quit" is not supposed to be the way
                // to exit analysis mode but we allow it.
                else if (cmd == "exit" || cmd == "quit") {
                    analyzeMode = false;
                }
                else if (cmd == "bk") {
                    do_command(cmd,board);
                }
                else if (cmd == "hint") {
                    do_command(cmd,board);
                }
                else if (cmd_word == "usermove" || text_to_move(board,cmd) != NullMove) {
                    Move m = get_move(cmd_word, cmd_arg);
                    if (!IsNull(m) && legalMove(board, m)) {
                        execute_move(board, m);
                    } else if (doTrace) {
                        std::cout << globals::debugPrefix <<
                            "illegal or unparseable move received in analysis mode" << std::endl;
                    }
                }
                else if (cmd == ".") {
                    analyze_output(stats);
                }
            }
        }
    }
    if (doTrace) std::cout << debugPrefix << "exiting analysis mode" << std::endl;
}

void Protocol::undo( Board &board)
{
    if (globals::gameMoves->getCurrent() < 2) return; // ignore "undo"
    const MoveRecord &mr = globals::gameMoves->at(globals::gameMoves->getCurrent()-2);
    if (!BoardIO::readFEN(board,mr.fen)) {
        // invalid FEN (should not happen)
        return;
    }
    // We do not actually remove the move from the array, because we
    // could be in force or analyze mode, in which we might later
    // go forward again through the game.
    globals::gameMoves->back_up();
    last_stats.clear();
    last_move = mr.move;
    last_move_image = mr.image;
    // In case we have backed up from the end of the game, reset
    // the "game end" flag.
    game_end = false;
}

// Execute a move made by the opponent or in "force" mode.
void Protocol::execute_move(Board &board,Move m)
{
    if (doTrace) {
        std::cout << debugPrefix << "execute_move: ";
        MoveImage(m,std::cout);
        std::cout << std::endl;
    }
    last_move = m;
    std::stringstream img;
    Notation::image(board,m,Notation::OutputFormat::SAN,img);
    last_move_image = img.str();
    // If our last move added was the pondering move, remove it - then it will be
    // replaced by result from the ponder search.
    if (globals::gameMoves->num_moves() > 0 && globals::gameMoves->back().wasPonder()) {
        globals::gameMoves->remove_last();
    }
    globals::gameMoves->add_move(board,m,last_move_image,nullptr,false);
    board.doMove(m);
}

void Protocol::processWinboardOptions(const std::string &args) {
    std::string name, value;
    size_t eq = args.find("=");
    if (eq == std::string::npos) {
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
        std::cout << debugPrefix << "setting option " << name << "=" << value << std::endl;
    }
    if (name == "SyzygyUse50MoveRule") {
        Options::setOption<bool>(value,globals::options.search.syzygy_50_move_rule);
    }
    else if (name == "SyzygyProbeDepth") {
        Options::setOption<int>(value,globals::options.search.syzygy_probe_depth);
    }
    else if (name == "OwnBook") {
        Options::setOption<bool>(value,globals::options.book.book_enabled);
    } else if (name == "BookPath") {
        Options::setOption<std::string>(value,globals::options.book.book_path);
        // force close of current book, opening of new one:
        globals::openingBook.close();
        globals::delayedInit();
    } else if (name == "Book variety") {
        Options::setOption<unsigned>(value,globals::options.book.variety);
        globals::openingBook.setVariety(globals::options.book.variety);
    } else if (name == "Store games") {
        Options::setOption<bool>(value,globals::options.games.store_games);
        if (!globals::options.games.store_games && globals::game_file.is_open()) {
            globals::game_file.close();
        }
        globals::initGameFile();
    } else if (name == "Game pathname") {
        Options::setOption<std::string>(value,globals::options.games.game_pathname);
        if (globals::game_file.is_open()) globals::game_file.close();
        globals::initGameFile();
    } else if (name == "Can resign") {
        Options::setOption<bool>(value,globals::options.search.can_resign);
    } else if (name == "Resign threshold") {
        Options::setOption<int>(value,globals::options.search.resign_threshold);
    } else if (name == "Position learning") {
        Options::setOption<bool>(value,globals::options.learning.position_learning);
    } else if (name == "Learning file") {
        Options::setOption<std::string>(value,globals::options.learning.file_name);
    } else if (name == "Strength") {
        Options::setOption<int>(value,globals::options.search.strength);
    } else if (name == "NNUE File") {
        Options::setOption<std::string>(value,globals::options.search.nnueFile);
        globals::nnueInitDone = false; // force re-init
#ifdef NUMA
    } else if (name == "Set processor affinity") {
       int tmp = globals::options.search.set_processor_affinity;
       Options::setOption<bool>(value,globals::options.search.set_processor_affinity);
       if (tmp != globals::options.search.set_processor_affinity) {
           searcher->recalcBindings();
       }
#endif
    }
    else if (name == "Move overhead") {
        Options::setOption<int>(value,globals::options.search.move_overhead);
    }
    else {
       std::cout << debugPrefix << "Warning: invalid option name \"" << name << "\"" << std::endl;
   }
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

void Protocol::loadgame(Board &board, std::ifstream &file) {
    std::vector<ChessIO::Header> hdrs(20);
    long first;
    ColorType stm = White; // side to move
    ChessIO::PGNReader pgnReader(file);
    pgnReader.collectHeaders(hdrs,first);
    ChessIO::TokenReader tokenReader(pgnReader);
    for (;;) {
        std::string num;
        ChessIO::Token tok = tokenReader.nextToken();
        if (tok.type == ChessIO::Eof)
            break;
        else if (tok.type == ChessIO::Number) {
            num = tok.val;
        }
        else if (tok.type == ChessIO::GameMove) {
            // parse the move
            Move m = Notation::value(board,stm,Notation::InputFormat::SAN,tok.val);
            if (IsNull(m) || !legalMove(board,m)) {
                std::cerr << "Illegal move" << std::endl;
                break;
            }
            else {
                std::string image;
                // Don't use the current move string as the input
                // parser is forgiving and will accept incorrect
                // SAN. Convert it here to strict SAN.
                Notation::image(board,m,Notation::OutputFormat::SAN,image);
                globals::gameMoves->add_move(board,m,image,nullptr,false);
                board.doMove(m);
            }
            stm = OppositeColor(stm);
        }
        else if (tok.type == ChessIO::Result) {
            break;
        }
    }
}


#ifdef SYZYGY_TBS
bool Protocol::validTbPath(const std::string &path) {
   // Shredder at least sets path to "<empty>" for tb types that are disabled
   return path != "" && path != "<empty>";
}
#endif

bool Protocol::uciOptionCompare(const std::string &a, const std::string &b) {
   if (a.length() != b.length()) {
      return false;
   } else {
      for (unsigned i = 0; i < a.length(); i++) {
         if (tolower(a[i]) != tolower(b[i])) return false;
      }
      return true;
   }
}

bool Protocol::do_command(const std::string &cmd, Board &board) {
    if (doTrace && debugPrefix.size() > 0) {
        std::cout << debugPrefix << "do_command: " << cmd << std::endl;
    }
    std::string cmd_word, cmd_args;
    split_cmd(cmd, cmd_word, cmd_args);
    if (cmd == "uci") {
        uci = true;
        globals::debugPrefix = UCI_DEBUG_PREFIX;
        verbose = true; // TBD: fixed for now
        std::cout << "id name " << "Arasan " << Arasan_Version;
        std::cout << std::endl;
        std::cout << "id author Jon Dart" << std::endl;
        std::cout << "option name Hash type spin default " <<
            globals::options.search.hash_table_size/(1024L*1024L) << " min 4 max " <<
#ifdef _64BIT
            "64000" << std::endl;
#else
            "2000" << std::endl;
#endif
        std::cout << "option name Ponder type check default true" << std::endl;
        std::cout << "option name Contempt type spin default 0 min -200 max 200" << std::endl;
#ifdef SYZYGY_TBS
        std::cout << "option name Use tablebases type check default ";
        if (globals::options.search.use_tablebases) std::cout << "true"; else std::cout << "false";
        std::cout << std::endl;
        std::cout << "option name SyzygyPath type string default " <<
            globals::options.search.syzygy_path << std::endl;
        std::cout << "option name SyzygyUse50MoveRule type check default true" << std::endl;
        std::cout << "option name SyzygyProbeDepth type spin default " <<
            globals::options.search.syzygy_probe_depth <<
           " min 0 max 64" << std::endl;
#endif
        std::cout << "option name MultiPV type spin default 1 min 1 max " << Statistics::MAX_PV << std::endl;
        std::cout << "option name OwnBook type check default " << (globals::options.book.book_enabled ? "true" : "false") << std::endl;
        std::cout << "option name BookPath type string default " << (globals::options.book.book_path == "" ? "book.bin" : globals::options.book.book_path) << std::endl;
        std::cout << "option name Book variety type spin default " <<
            globals::options.book.variety << " min 0 max 100" << std::endl;
        std::cout << "option name Threads type spin default " <<
            globals::options.search.ncpus << " min 1 max " <<
            Constants::MaxCPUs << std::endl;
        std::cout << "option name Position learning type check default " <<
            (globals::options.learning.position_learning ? "true" : "false") << std::endl;
        std::cout << "option name Learning file type string default " <<
            globals::options.learning.file_name << std::endl;
        std::cout << "option name UCI_LimitStrength type check default false" << std::endl;
        std::cout << "option name UCI_Elo type spin default " << globals::options.getRating(globals::options.search.strength) <<
            " min " << Options::MIN_RATING << " max " << Options::MAX_RATING << std::endl;
        std::cout << "option name NNUE file type string default " << globals::options.search.nnueFile << std::endl;
#ifdef NUMA
        std::cout << "option name Set processor affinity type check default " <<
           (globals::options.search.set_processor_affinity ? "true" : "false") << std::endl;
#endif
        std::cout << "option name Move overhead type spin default " <<
            30 << " min 0 max 1000" << std::endl;
#ifdef TUNE
        // support for tuning via UCI parameters.
        for (const auto &p : Tunable::tunables) {
            const std::string &name = p.first;
            const Tunable *t = p.second;
            std::cout << "option name " << name << " type spin default " << t->default_value << " min " <<
                t->min_value << " max " << t->max_value << std::endl;
        }
#endif
        std::cout << "uciok" << std::endl;
        return true;
    }
    else if (cmd == "quit") {
        return false;
    }
    else if (uci && cmd_word == "setoption") {
        std::string name, value;
        size_t nam = cmd_args.find("name");
        if (nam != std::string::npos) {
            // search for "value"
            size_t val = cmd_args.find(" value",nam+4);
            if (val != std::string::npos) {
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
                size_t old = globals::options.search.hash_table_size;
                // size is in megabytes
                std::stringstream buf(value);
                int size;
                buf >> size;
                if (buf.bad()) {
                    std::cout << debugPrefix << "problem setting hash size to " << buf.str() << std::endl;
                }
                else {
                    globals::options.search.hash_table_size = (size_t)size*1024L*1024L;
                    if (old != globals::options.search.hash_table_size) {
                       searcher->resizeHash(globals::options.search.hash_table_size);
                    }
                }
            }
        }
        else if (uciOptionCompare(name,"Ponder")) {
            easy = !(value == "true");
        }
        else if (uciOptionCompare(name,"Contempt")) {
            std::stringstream buf(value);
            int uciContempt;
            buf >> uciContempt;
            if (buf.bad()) {
               std::cout << debugPrefix << "problem setting contempt value" << std::endl;
            }
            else if (uciContempt < -200 || uciContempt > 200) {
               std::cout << "invalid contempt value, must be >=-200, <= 200 centipawns" << std::endl;
            }
            else {
               searcher->setContempt(uciContempt);
            }
        }
#ifdef SYZYGY_TBS
        else if (uciOptionCompare(name,"Use tablebases")) {
           Options::setOption<bool>(value, globals::options.search.use_tablebases);
           searcher->updateTBOptions();
        }
        else if (uciOptionCompare(name,"SyzygyPath") && validTbPath(value)) {
           globals::unloadTb();
           globals::options.search.syzygy_path = value;
           globals::options.search.use_tablebases = true;
           searcher->updateTBOptions();
        }
        else if (uciOptionCompare(name,"SyzygyUse50MoveRule")) {
            Options::setOption<bool>(value, globals::options.search.syzygy_50_move_rule);
            searcher->updateTBOptions();
        }
        else if (uciOptionCompare(name,"SyzygyProbeDepth")) {
           Options::setOption<int>(value,globals::options.search.syzygy_probe_depth);
           searcher->updateTBOptions();
        }
#endif
        else if (uciOptionCompare(name,"OwnBook")) {
            Options::setOption<bool>(value, globals::options.book.book_enabled);
        }
        else if (uciOptionCompare(name,"BookPath")) {
            Options::setOption<std::string>(value,globals::options.book.book_path);
            // force close of current book, opening of new one:
            globals::openingBook.close();
            globals::delayedInit();
        }
        else if (uciOptionCompare(name,"Book variety")) {
            Options::setOption<unsigned>(value,globals::options.book.variety);
            globals::openingBook.setVariety(globals::options.book.variety);
        }
        else if (uciOptionCompare(name,"Position learning")) {
            Options::setOption<bool>(value,globals::options.learning.position_learning);
        }
        else if (uciOptionCompare(name,"Learning file")) {
            Options::setOption<std::string>(value,globals::options.learning.file_name);
        }
        else if (uciOptionCompare(name,"MultiPV")) {
            Options::setOption<int>(value,globals::options.search.multipv);
            globals::options.search.multipv = std::min<int>(Statistics::MAX_PV,globals::options.search.multipv);
            // GUIs (Shredder at least) send 0 to turn multi-pv off: but
            // our option counts the # of lines to show, so we set it to
            // 1 in this case.
            if (globals::options.search.multipv == 0) globals::options.search.multipv = 1;
            // This value is cached in the searcher, so need to update there
            searcher->setMultiPV(globals::options.search.multipv);
            stats.multipv_count = 0;
            // migrate current stats to 1st Multi-PV table entry:
            stats.multi_pvs[0] =
                Statistics::MultiPVEntry(stats);
        }
        else if (uciOptionCompare(name,"Threads")) {
            int threads = globals::options.search.ncpus;
            if (Options::setOption<int>(value,threads) && threads >0 && threads <= Constants::MaxCPUs) {
                globals::options.search.ncpus = threads;
                searcher->setThreadCount(globals::options.search.ncpus);
            }
        }
        else if (uciOptionCompare(name,"UCI_LimitStrength")) {
            Options::setOption<bool>(value, uciStrengthOpts.limitStrength);
            if (uciStrengthOpts.limitStrength) {
               globals::options.setRating(uciStrengthOpts.elo);
            } else {
               // reset to full strength
               globals::options.setRating(Options::MAX_RATING);
            }
        } else if (uciOptionCompare(name,"UCI_Elo")) {
            int rating;
            if (Options::setOption<int>(value,rating)) {
               uciStrengthOpts.elo = rating;
               if (uciStrengthOpts.limitStrength) {
                  globals::options.setRating(rating);
               }
            }
	}
        else if (uciOptionCompare(name,"NNUE file")) {
           Options::setOption<std::string>(value,globals::options.search.nnueFile);
           globals::nnueInitDone = false; // force re-init
	}
#ifdef NUMA
        else if (uciOptionCompare(name,"Set processor affinity")) {
           int tmp = globals::options.search.set_processor_affinity;
           Options::setOption<bool>(value, globals::options.search.set_processor_affinity;
           if (tmp != globals::options.search.set_processor_affinity) {
               searcher->recalcBindings();
           }
        }
#endif
        else if (uciOptionCompare(name,"Move overhead")) {
           Options::setOption<int>(value,globals::options.search.move_overhead);
        }
#ifdef TUNE
        else {
            auto &extras = Tunable::tunables;
            auto it = extras.find(name);
            if (it == extras.end()) {
                std::cout << debugPrefix << "error: invalid option name \"" << name << "\"" << std::endl;
            }
            else {
                // currently only ints supported
                int tmp = 0;
                if (Options::setOption<int>(value, tmp)) {
                    it->second->setCurrent(tmp);
                }
                else {
                    std::cout << debugPrefix << "failed to update extra option \"" << name << "\"" << std::endl;
                }
                Search::init();
            }
        }
#else
        else {
           std::cout << debugPrefix << "error: invalid option name \"" << name << "\"" << std::endl;
        }
#endif
        searcher->updateSearchOptions();
    }
    else if (uci && cmd == "ucinewgame") {
        do_command("new",board);
        return true;
    }
    else if (uci && cmd == "isready") {
        globals::delayedInit();
        std::cout << "readyok" << std::endl;
    }
    else if (uci && cmd_word == "position") {
        ponder_move = NullMove;
        lastAdded = 0;
        if (cmd_args.substr(0,3) == "fen" || cmd_args.substr(0,8) == "startpos") {
            std::string fen;
            if (cmd_args.substr(0,8) == "startpos") {
                board.reset();
                BoardIO::writeFEN(board,fen,0);
            }
            else {
                int valid = 0;
                if (cmd_args.length() > 3) {
                    fen = cmd_args.substr(3);
                    valid = BoardIO::readFEN(board, fen);
                }
                if (!valid) {
                    if (doTrace) std::cout << debugPrefix << "warning: invalid fen!" << std::endl;
                }
            }
            // clear some global vars
            stats.clear();
            ponder_stats.clear();
            last_stats.clear();
            last_move = NullMove;
            last_move_image.clear();
            if (globals::gameMoves->size() && fen != globals::gameMoves->front().fen) {
                // The fen position does not match the starting FEN for the move array.
                // Assume this is a new game, or some other situation like a takeback,
                // or analysis of a previous move, and clear the array.
                if (doTrace) {
                    std::cout << debugPrefix << "old fen = " << globals::gameMoves->front().fen << " new fen = " <<
                        fen << ", resetting array" << std::endl;
                }
                globals::gameMoves->reset();
            }
            predicted_move = NullMove;
            ponder_status = PonderStatus::None;
        }
        size_t movepos = cmd_args.find("moves");
        if (movepos != std::string::npos) {
            std::stringstream s(cmd_args.substr(movepos+5));
            std::istream_iterator<std::string> eos;
            size_t moveIndex = 0;
            std::istream_iterator<std::string> it(s);
            bool adding = false;
            // read moves. stringstream iterator reads space-separated strings.
            for (; it != eos; ++it, ++moveIndex) {
                std::string move(*it);
                Move m = Notation::value(board,board.sideToMove(),Notation::InputFormat::UCI,move);
                if (!IsNull(m)) {
                    // Arena can send invalid move sequences in some situations
                    if (!validMove(board,m)) {
                        std::cout << debugPrefix << "warning: invalid move received: " << move << ", ignoring" << std::endl;
                        break;
                    }
                    std::string move_img, current_fen;
                    Notation::image(board,m,Notation::OutputFormat::SAN,move_img);
                    BoardIO::writeFEN(board,current_fen,0);
                    // If the current move list has a position matching this new
                    // move sequence, leave that record intact. Otherwise,
                    // truncate the move array and start adding moves.
                    if (!adding && (moveIndex == globals::gameMoves->size() || current_fen != (*globals::gameMoves)[moveIndex].fen)) {
                        adding = true;
                        globals::gameMoves->resize(moveIndex);
                    }
                    if (adding) {
                         if (doTrace) {
                             std::cout << debugPrefix << "adding ";
                             MoveImage(m,std::cout);
                             std::cout << " at position " << moveIndex << std::endl;
                         }
                         globals::gameMoves->add_move(board,m,move_img,nullptr,false);
                    }
                    ++lastAdded;
                    board.doMove(m);
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
       Bench::Results res = b.bench();
       std::cout << res;
    }
    else if (cmd_word == "test") {
        std::string filename;
        std::ofstream *out_file = nullptr;
        std::streambuf *sbuf = std::cout.rdbuf();
        std::stringstream s(cmd_args);
        std::istream_iterator<std::string> it(s);
        std::istream_iterator<std::string> eos;
        Tester::TestOptions opts;
        if (it != eos) {
            filename = *it++;
            if (it != eos) {
                while (it != eos) {
                    if (*it == "-d") {
                        if (++it == eos) {
                            std::cerr << "expected number after -d" << std::endl;
                        } else {
                            std::stringstream num(*it);
                            num >> opts.depth_limit;
                            opts.depth_limit = std::clamp<int>(opts.depth_limit,0,Constants::MaxPly);
                            it++;
                        }
                    }
                    else if (*it == "-t") {
                        if (++it == eos) {
                            std::cerr << "expected number after -t" << std::endl;
                        } else {
                            std::stringstream num(*it);
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
                            std::cerr << "expected number after -x" << std::endl;
                        } else {
                            std::stringstream num(*it);
                            num >> opts.early_exit_plies;
                            it++;
                        }
                    }
                    else if (*it == "-N") {
                        if (++it == eos) {
                            std::cerr << "Expected number after -N" << std::endl;
                        } else {
                            std::stringstream num(*it);
                            num >> opts.moves_to_search;
                            it++;
                        }
                    }
                    else if (*it == "-o") {
                        if (++it == eos) {
                            std::cerr << "Expected filename after -o" << std::endl;
                        } else {
                            out_file = new std::ofstream((*it).c_str(), std::ios::out | std::ios::trunc);
                            // redirect stdout
                            std::cout.rdbuf(out_file->rdbuf());
                            break;
                        }
                    } else if ((*it)[0] == '-') {
                        std::cerr << "unexpected switch: " << *it << std::endl;
                        it++;
                    } else {
                        break;
                    }
                }
                if (opts.depth_limit == 0 && opts.time_limit == 0) {
                    std::cerr << "error: time (-t) or depth (-d) must be specified" << std::endl;
                }
                else {
                    do_command("new",board);
                    Tester tester;
                    tester.do_test(searcher,filename,opts);
                    std::cout << "test complete" << std::endl;
                }
                if (out_file) {
                    out_file->close();
                    delete out_file;
                    std::cout.rdbuf(sbuf);               // restore console output
                }
            }
            else
                std::cout << "invalid command" << std::endl;
        }
        else
            std::cout << "invalid command" << std::endl;
    }
    else if (cmd_word == "perft") {
       if (cmd_args.length()) {
          std::stringstream ss(cmd_args);
          int depth;
          if ((ss >> depth).fail()) {
             std::cerr << "usage: perft <depth>" << std::endl;
          } else {
             Board b;
             std::cout << "perft " << depth << " = " << perft(b,depth) << std::endl;
          }
       }
       else {
          std::cerr << "usage: perft <depth>" << std::endl;
       }
    }
    else if (cmd_word == "eval") {
        globals::delayedInit();
#ifdef SYZYGY_TBS
        score_t tbscore;
        if (globals::options.search.use_tablebases) {
            MoveSet rootMoves;
            if (SyzygyTb::probe_root(board, board.anyRep(), tbscore, rootMoves) >= 0) {
                std::cout << "score = ";
                if (tbscore == -SyzygyTb::CURSED_SCORE)
                    std::cout << "draw (Cursed Loss)";
                else if (tbscore == -SyzygyTb::CURSED_SCORE)
                    std::cout << "draw (Cursed Win)";
                else
                    Scoring::printScore(tbscore, std::cout);
                std::cout << " (from Syzygy tablebases)" << std::endl;
            }
        }
#endif
        score_t score;
        if ((score = Scoring::tryBitbase(board)) != Constants::INVALID_SCORE) {
            std::cout << "bitbase score=";
            Scoring::printScore(score, std::cout);
            std::cout << std::endl;
        }
        if (board.isLegalDraw()) {
            std::cout << "position evaluates to draw (statically)" << std::endl;
        }
        Scoring *s = new Scoring();
        std::cout << board << std::endl;
        std::cout << "NNUE score: ";
        Scoring::printScore(s->evalu8NNUE(board), std::cout);
        std::cout << std::endl;
        board.flip();
        std::cout << board << std::endl;
        std::cout << "NNUE score: ";
        Scoring::printScore(s->evalu8NNUE(board), std::cout);
        std::cout << std::endl;
        delete s;
        std::cout << std::endl;
    }
    else if (uci && cmd == "stop") {
        searcher->stop();
        return true;
    }
    else if (uci && cmd_word == "go") {
        std::string option;
        srctype = TimeLimit;
        movestogo = 0;
        bool do_ponder = false;
        bool infinite = false;
        std::stringstream ss(cmd_args);
        std::istream_iterator<std::string> it(ss);
        std::istream_iterator<std::string> eos;
        MoveSet movesToSearch;
        if (lastAdded != globals::gameMoves->size()) {
            // if we backed up and now are less far from the root
            // position, then truncate the global move array
            globals::gameMoves->resize(lastAdded);
            if (doTrace) {
                std::cout << debugPrefix << "truncated move array to " << lastAdded << std::endl;
            }
        }
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
                time_limit = Constants::INFINITE_TIME;
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
                std::stringstream s(*it++);
                int srctime;
                s >> srctime;
                if (s.bad() || srctime < 0.0) {
                    std::cerr << "movetime: invalid argument" << std::endl;
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
        globals::delayedInit();
        if (do_ponder) {
            ponder<true>(board,NullMove,NullMove);
            // We should not send the move unless we have received a
            // "ponderhit" command, in which case we were pondering the
            // right move. If pondering completes early, we must wait
            // for ponderhit before sending. But also send the move if
            // we were stopped - this is the "handshake" that tells the
            // UI we received the stop.
            if (doTrace) {
                std::cout << debugPrefix << "done pondering: stopped=" << (int)searcher->wasStopped() << " move=";
                Notation::image(board,ponder_move,Notation::OutputFormat::SAN,std::cout);
                std::cout << (std::flush) << std::endl;
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
                    std::cout << debugPrefix << "ponder stopped early, entering wait state" << std::endl;
                }
                // Check that we do not now have ponderhit or stop in the pending stack.
                // To avoid races, check with the input mutex locked.
                std::unique_lock<std::mutex> lock(inputMtx);
                std::vector<std::string> newPending(pending);
                uciWaitState = true;
                for (const std::string &pendingCmd : pending) {
                    if (pendingCmd == "stop" || pendingCmd == "ponderhit") {
                        uciWaitState = false;
                    } else {
                        newPending.push_back(pendingCmd);
                    }
                }
                pending = newPending;
                if (!uciWaitState) {
                    std::cout << debugPrefix << "ponderhit in stack, sending move" << std::endl << (std::flush);
                    uciOut(ponder_stats);
                    send_move(board,ponder_move,ponder_stats);
                    ponder_move = NullMove;
                    ponderhit = false;
                }
                else if (doTrace) {
                    std::cout << debugPrefix << "entering wait state" << std::endl << (std::flush);
                }
            }
        }
        else {
            CLOCK_TYPE startTime;
            if (doTrace) {
                startTime = getCurrentTime();
                std::cout << debugPrefix << "starting search" << (std::flush) << std::endl;
            }
            best_move = search(board,movesToSearch,stats,infinite);
            if (doTrace) {
                std::cout << debugPrefix << "done searching, elapsed time=" << getElapsedTime(startTime,getCurrentTime()) << ", stopped=" << (int)searcher->wasStopped() << (std::flush) << std::endl;
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
        // Note: this is a CECP command but this code can be executed
        // internally in UCI mode also.
        if (!analyzeMode) save_game();
        board.reset();
        computer_plays_white = false;
        // Note: "new" does not reset analyze mode
        forceMode = false;
        game_end = result_pending = false;
        computer = 0;
        opponent_name = "";
        stats.clear();
        ponder_stats.clear();
        last_stats.clear();
        lastAdded = 0;
        last_move = NullMove;
        last_move_image.clear();
        globals::gameMoves->reset();
        predicted_move = NullMove;
        ponder_status = PonderStatus::None;
        start_fen.clear();
        globals::delayedInit();
        searcher->clearHashTables();
        if (!analyzeMode && ics) {
           std::cout << "kib Hello from Arasan " << Arasan_Version << std::endl;
        }
        if (doTrace) std::cout << debugPrefix << "finished 'new' processing" << std::endl;
    }
    else if (cmd == "random" || cmd_word == "variant") {
        // ignore
    }
    else if (cmd_word == "xboard") {
        globals::debugPrefix = CECP_DEBUG_PREFIX;
    }
    else if (cmd_word == "protover") {
        // new in Winboard 4.2
        std::cout << "feature name=1 setboard=1 san=1 usermove=1 ping=1 ics=1 playother=0 sigint=0 colors=0 analyze=1 debug=1 memory=1 smp=1 variants=\"normal\"";
#ifdef SYZYGY_TBS
        std::cout << " egt=\"syzygy\"";
#endif
        std::cout << " option=\"SyzygyUse50MoveRule -check " << globals::options.search.syzygy_50_move_rule << "\"";
        std::cout << " option=\"SyzygyProbeDepth -spin " <<
            globals::options.search.syzygy_probe_depth << " 0 64" << "\"";
        std::cout << " option=\"OwnBook -check " <<
            globals::options.book.book_enabled << "\"";
        std::cout << " option=\"BookPath -path " <<
            globals::options.book.book_path << "\"";
        std::cout << " option=\"Book variety -spin " <<
            globals::options.book.variety << " 0 100\"";
        std::cout << " option=\"Can resign -check " <<
            globals::options.search.can_resign << "\"";
        std::cout << " option=\"Resign threshold -spin " <<
            globals::options.search.resign_threshold << " -1000 0" << "\"";
        std::cout << " option=\"Store games -check " <<
            globals::options.games.store_games << "\"";
        std::cout << " option=\"Game pathname -string " <<
            globals::options.games.game_pathname << "\"";
        std::cout << " option=\"Position learning -check " <<
            globals::options.learning.position_learning << "\"";
        std::cout << " option=\"Learning file -string " <<
            globals::options.learning.file_name << "\"";
        // strength option (new for 14.2)
        std::cout << " option=\"Strength -spin " << globals::options.search.strength << " 0 100\"";
        std::cout << " option=\"NNUE file -string " << globals::options.search.nnueFile << "\"";
#ifdef NUMA
        std::cout << " option=\"Set processor affinity -check " <<
            globals::options.search.set_processor_affinity << "\"" << std::endl;
#endif
        std::cout << " option=\"Move overhead -spin " << 30 << " 0 1000\"";
        std::cout << " myname=\"" << "Arasan " << Arasan_Version << "\"" << std::endl;
        // set done = 0 because it may take some time to initialize tablebases.
        std::cout << "feature done=0" << std::endl;
        globals::delayedInit();
        std::cout << "feature done=1" << std::endl;
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
        // This is a non-standard command used by the Arasan GUI to list book moves
	std::vector<Move> bookMoves;
        unsigned count = 0;
        globals::delayedInit(); // to allow "bk" before "new"
        if (globals::options.book.book_enabled) {
            count = globals::openingBook.book_moves(*main_board,bookMoves);
        }
        if (count == 0) {
            std::cout << '\t' << "No book moves for this position." << std::endl
                << std::endl;
        }
        else {
            std::cout << " book moves:" << std::endl;
            for (unsigned i = 0; i<count; i++) {
                std::cout << '\t';
                Notation::image(*main_board,bookMoves[i],Notation::OutputFormat::SAN,std::cout);
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }
    }
    else if (cmd == "depth") {
    }
    else if (cmd_word == "level") {
        if (!parseLevel(cmd_args, moves, minutes, incr)) {
            std::cout << debugPrefix << "failed to parse 'level' command" << std::endl;
        }
        srctype = TimeLimit;
    }
    else if (cmd_word == "st") {
        process_st_command(cmd_args);
    }
    else if (cmd_word == "sd") {
        std::stringstream s(cmd_args);
        s >> ply_limit;
        srctype = FixedDepth;
    }
    else if (cmd_word == "time") {
        // my time left
        int t;
        std::stringstream s(cmd_args);
        s >> t;
        time_left = t*10; // convert from centiseconds to ms
    }
    else if (cmd_word == "otim") {
        // opponent's time left
        int t;
        std::stringstream s(cmd_args);
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
        globals::gameMoves->setResult(cmd_args);
        game_end = true;
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
        if (computer_plays_white)
            globals::gameMoves->setResult("0-1");
        else
            globals::gameMoves->setResult("1-0");
        if (doTrace) {
            std::cout << debugPrefix << "setting game result to " << globals::gameMoves->getResult() << std::endl;
        }
    }
    else if (cmd == "draw") {
        // "draw" command. Requires winboard 3.6 or higher.
        if (accept_draw(board)) {
            // Notify opponent. don't assume draw is concluded yet.
            std::cout << "offer draw" << std::endl;
        }
        else if (doTrace) {
            std::cout << debugPrefix << "draw declined" << std::endl;
        }
    }
    else if (cmd_word == "setboard") {
        start_fen = cmd_args;
        std::stringstream s(start_fen,std::ios::in);
        ChessIO::load_fen(s,board);
    }
    else if (cmd_word == "loadgame") {
        std::string filename = cmd_args;
        std::ifstream file(filename.c_str(),std::ios::in);
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
        analyzeMode = true;
        analyze(board);
    }
    else if (cmd == "exit") {
        analyzeMode = false;
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
        Move reply = search(board,movesToSearch,stats,false);
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
    else if (cmd == "bogus" || cmd_word == "accepted" || cmd_word == "rejected") {
    }
    else if (cmd == "force") {
        forceMode = true;
    }
    else if (cmd_word == "rating") {
        std::stringstream args(cmd_args);
        args >> computer_rating;
        args >> opponent_rating;
        const int rdiff = computer_rating-opponent_rating;
        score_t contempt = static_cast<score_t>(4*Scoring::PAWN_VALUE*(1.0/(1.0+exp(-rdiff/400.0)) - 0.5));
        if (doTrace) {
            std::cout << debugPrefix << "contempt (calculated from ratings) = ";
            Scoring::printScore(contempt,std::cout);
            std::cout << std::endl;
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
           std::stringstream ss(cmd_args);
           if((ss >> globals::options.search.ncpus).fail()) {
               std::cerr << "invalid value following 'cores'" << std::endl;
           } else {
              if (doTrace) {
                 std::cout << debugPrefix << "setting cores to " << globals::options.search.ncpus << std::endl;
              }
              globals::options.search.ncpus = std::min<int>(globals::options.search.ncpus,Constants::MaxCPUs);
              searcher->updateSearchOptions();
              searcher->setThreadCount(globals::options.search.ncpus);
           }
        }
    }
    else if (cmd_word == "memory") {
        // Setting -H on the Arasan command line takes precedence over
        // what the GUI sets
        if (!memorySet) {
            // set memory size in MB
            std::stringstream ss(cmd_args);
            uint64_t mbs;
            ss >> mbs;
            if (ss.fail() || ss.bad()) {
               std::cout << debugPrefix << "invalid value following 'memory'" << std::endl;
            } else {
               size_t mb_size = mbs*1024L*1024L;
               if (doTrace) {
                   std::cout << debugPrefix << "setting hash size to " << mb_size << " bytes " << std::endl << (std::flush);
               }
               globals::options.search.hash_table_size = mb_size;
               searcher->updateSearchOptions();
               searcher->resizeHash(globals::options.search.hash_table_size);
           }
        }
        else {
            std::cout << debugPrefix << "warning: memory command ignored due to -H on command line" << std::endl;
        }
    }
    else if  (cmd_word == "egtpath") {
        size_t space = cmd_args.find_first_of(' ');
        std::string type = cmd_args.substr(0,space);
        std::transform(type.begin(), type.end(), type.begin(), ::tolower);
        if (type.length() > 0) {
            std::transform(type.begin(), type.begin()+1, type.begin(), ::toupper);
        }
        std::string path;
        if (space != std::string::npos) path = cmd_args.substr(space+1);
#ifdef _WIN32
        // path may be in Unix format. Convert.
        std::replace(path.begin(), path.end(), '/', '\\');
#endif
#ifdef SYZYGY_TBS
        // Unload existing tb set if in use and if path has changed
        if (globals::options.tbPath() != path) {
           if (doTrace) std::cout << debugPrefix << "unloading tablebases" << std::endl;
           globals::unloadTb();
        }
        // Set the tablebase globals::options. But do not initialize the
        // tablebases here. Defer until globals::delayedInit is called again.
        // One reason to do this is that we may receive multiple
        // egtpath commands from Winboard.
        globals::options.search.use_tablebases = 1;
        globals::options.search.syzygy_path = path;
        searcher->updateTBOptions();
        if (doTrace) {
           std::cout << debugPrefix << "setting Syzygy tb path to " << globals::options.search.syzygy_path << std::endl;
        }
#endif
    }
    else if (!uci) {
        // see if it could be a move
        std::string movetext;
        if (cmd_word == "usermove") {
            // new for Winboard 4.2
            movetext = cmd_args;
        } else {
            movetext = cmd;
        }
        auto it = movetext.begin();
        while (it != movetext.end() && !isalpha(*it)) it++;
        movetext.erase(movetext.begin(),it);
        if (doTrace) {
            std::cout << debugPrefix << "move text = " << movetext << std::endl;
        }
        Move move;
        if ((move = text_to_move(board,movetext)) == NullMove) {
            if (doTrace) {
                std::cout << debugPrefix << "failed to parse move" << std::endl;
                std::cout << debugPrefix << "board = ";
                BoardIO::writeFEN(board,std::cout,false);
                std::cout << std::endl;
            }
        }
        else {
            // We have received a move outside a search, so not while pondering
            if (game_end) {
                if (forceMode)
                    game_end = false;
                else {
                    if (doTrace) std::cout << debugPrefix << "ignoring move " << movetext << " received after game end" << std::endl;
                    return true;
                }
            }
            if (doTrace) {
                std::cout << debugPrefix << "got move: " << movetext << std::endl;
            }
            // make the move on the board
            execute_move(board,move);
            Move reply;
            if (!forceMode && !analyzeMode) {
               // determine what to do with the pondering result, if
               // there is one.
               if (ponder_status == PonderStatus::Pending &&
                   MovesEqual(predicted_move,move) && !IsNull(ponder_move)) {
                  // We completed pondering already and we got a reply to
                  // this move (e.g. might be a forced reply).
                  if (doTrace) std::cout << debugPrefix << "pondering complete already" << std::endl;
                  if (doTrace) {
                     std::cout << debugPrefix << "sending ponder move ";
                     MoveImage(ponder_move,std::cout);
                     std::cout << std::endl << (std::flush);
                  }
                  reply = ponder_move;
                  stats = ponder_stats;
                  post_output(stats);
                  game_end = game_end || stats.end_of_game;
                  if (doTrace) std::cout << debugPrefix << "game_end = " << game_end << std::endl;
                  predicted_move = ponder_move = NullMove;
               }
               else {
                  predicted_move = ponder_move = NullMove;
                  ponder_status = PonderStatus::None;
                  MoveSet movesToSearch;
                  reply = search(board,movesToSearch,stats,false);
                  // Note: we may know the game has ended here before
                  // we get confirmation from Winboard. So be sure
                  // we set the global game_end flag here so that we won't
                  // start pondering after the game is over.
                  game_end = game_end || stats.end_of_game;
                  if (doTrace) {
                     std::cout << debugPrefix << "state = " << stats.state << std::endl;
                     std::cout << debugPrefix << "game_end = " << game_end  << std::endl;
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
               ponder<false>(board,reply,stats.best_line[1]);
               // check again for game-ending commands before we process
               // ponder result
               if (check_pending(board) == PendingStatus::GameEnd) {
                   return true;
               }
               if (analyzeMode || forceMode) {
                   break;
               }
               if (doTrace) {
                     std::cout << debugPrefix << "ponder_status=";
                     if (ponder_status == PonderStatus::Hit) std::cout << "Hit";
                     else if (ponder_status == PonderStatus::NoHit) std::cout << "NoHit";
                     else if (ponder_status == PonderStatus::None) std::cout << "None";
                     else std::cout << "Pending";
                     std::cout << std::endl;
               }
               // We are done pondering. If we got a ponder hit
               // (opponent made our predicted move), then we are ready
               // to move now.
               if (ponder_status == PonderStatus::Hit && !IsNull(ponder_move) && !game_end
                   && !forceMode && !analyzeMode) {
                  // we got a reply from pondering
                  if (doTrace) {
                     std::cout << debugPrefix << "sending ponder move" << std::endl;
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
                          std::cout << debugPrefix << "exiting ponder loop" << std::endl;
                      }
                      break;
                  }
               }
            }
        }
        if (doTrace) {
            std::cout << debugPrefix << "out of ponder loop" << std::endl;
        }
    }
    return true;
}
