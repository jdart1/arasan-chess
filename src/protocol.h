// Handles Winboard/xboard/UCI protocol.
// Copyright 1997-2022 by Jon Dart. All Rights Reserved.
//
#ifndef _PROTOCOL_H
#define _PROTOCOL_H

#include "board.h"
#include "calctime.h"
#include "eco.h"
#include "input.h"
#include "search.h"

#include <mutex>
#include <vector>

// Handles Winboard/xboard/UCI protocol.
class Protocol {

public:
    Protocol(const Board &,bool traceOn, bool icsMode, bool cpus_set, bool memory_set);

    virtual ~Protocol();

    // read input from stdin and dispatch commands
    void poll(bool &terminated);

    void save_game();

    bool traceOn() const noexcept {
        return doTrace;
    }

    bool hasPending() const noexcept {
        return pending.size() > 0;
    }

    bool isSearching() const noexcept {
        return searcher->searching();
    }

private:

    static const char * UCI_DEBUG_PREFIX;
    static const char * CECP_DEBUG_PREFIX;

    enum class PendingStatus { Nothing, GameEnd, Move };

    bool popPending(std::string &cmd) {
        std::unique_lock<std::mutex> inputMtx;
        if (pending.empty()) {
            return false;
        }
        cmd = pending.front();
        pending.erase(pending.begin());
        return true;
    }

    // split a command line into a verb (cmd_word) and arguments (cmd_args)
    void split_cmd(const std::string &cmd, std::string &cmd_word, std::string &cmd_args);

    // Convert move text to Move type
    Move text_to_move(const Board &board, const std::string &input);

    // Parse move input (may be preceded by "usermove" or may just be
    // move text)
    Move get_move(const std::string &cmd_word, const std::string &cmd_args);

    // Do all pending commands in stack.
    // Return false if "quit" was seen.
    bool do_all_pending(Board &board);

    // Check for user move, resign or result in pending stack.
    // If found, return status and do not dequeue command.
    PendingStatus check_pending(Board &board);

    // handle Winboard "level" command
    void parseLevel(const std::string &cmd, int &moves, float &minutes, int &incr);

    // respond to a Winboard "ping" command
    void sendPong(const std::string &arg);

    // handle "st" (time control) from Winboard
    void process_st_command(const std::string &cmd_args);

    // In Winboard mode, on the chess server, compute a contempt value
    // from ratings
    score_t contemptFromRatings(int computer_rating,int opponent_rating);

    // return the UCI increment for "side"
    int getIncrUCI(const ColorType side);

    // determine whether or not to accept a draw offer
    bool accept_draw(Board &board);

    // issue some help text to the console
    void do_help();

    // Format and output a move in the right format (UCI/Winboard)
    void move_image(const Board &board, Move m, std::ostream &buf, bool uci);

    // output status for UCI ("info" output)
    void uciOut(int depth, score_t score, time_t time,
                uint64_t nodes, uint64_t tb_hits, const std::string &best_line_image, int multipv);


    // overload status output that gets info from Statistics
    void uciOut(const Statistics &stats);

    // Callback from search - generates status output to UI
    void post_output(const Statistics &stats);

    // Process a command during search. Return true if processed
    // (should be removed from pending stack). Also sets exit flag
    // if processing should terminate.
    bool processPendingInSearch(SearchController *controller, const std::string &cmd, bool &exit);

    // Callback from search - check for input, handle any commands received.
    // Returns true if search should terminate.
    bool monitor(SearchController *s, const Statistics &);

    // handle commands in edit mode (Winboard protocol)
    void edit_mode_cmds(Board &board,ColorType &side,const std::string &cmd);

    void calcTimes(bool pondering, ColorType side, timeMgmt::Times &times);

    // do a ponder search
    void ponder(Board &board, Move move, Move predicted_reply, bool uci);

    // foreground search using the current board position.
    Move search(SearchController *searcher, Board &board,
                const MoveSet &movesToSearch, Statistics &stats, bool infinite);

    // return true if current board position is a draw by the
    // rules of chess. If so, also set the "reason" std::string
    int isDraw(const Board &board, Statistics &last_stats, std::string &reason);

    // Send a move to the UI
    void send_move(Board &board, Move &move, Statistics &);

    // handle a command when we are halted waiting for more UCI input
    void processCmdInWaitState(const std::string &cmd);

    // Find best move using the current board position.
    Move analyze(SearchController &searcher, Board &board, Statistics &stats);

    // Handle the "hint" command (issued by the Arasan GUI)
    void doHint();

    // output search status ("stat01", for Winboard)
    void analyze_output(const Statistics &stats);

    // Winboard analyze mode
    void analyze(Board &board);

    // undo command (Winboard)
    void undo( Board &board);

    // set check tupe option for Winboard
    void setCheckOption(const std::string &value, int &dest);

    // make a move on the board, add to log and search history
    void execute_move(Board &board,Move m);

#ifdef TUNE
    // Set a tuning parameter passed on the command line (used for
    // CLOP for example).
    void setTuningParam(const std::string &name, const std::string &value);
#endif

    // Process options for Winboard
    void processWinboardOptions(const std::string &args);

    // Execute a perft test and return node count
    uint64_t perft(Board &board, int depth);

    // Set the board position from a file
    void loadgame(Board &board, std::ifstream &file);

#ifdef SYZYGY_TBS
    // Validate a TB path sent from the UI (UCI)
    bool validTbPath(const std::string &path);
#endif

    // Case-insensitive compare for UCI options
    bool uciOptionCompare(const std::string &a, const std::string &b);

    // Execute a command, return false if program should terminate.
    bool do_command(const std::string &cmd, Board &board);

    // Result from pondering. Hit = predicted opponent move,
    // NoHit = did not predict opponent move, Pending =
    // opponent move not received yet.
    enum class PonderStatus {None, Hit, NoHit, Pending};

    bool verbose;
    bool post;
    SearchController *searcher;
    Move last_move;
    std::string last_move_image;
    Move last_computer_move;
    Statistics last_computer_stats;
    std::string game_pathname;
    std::ofstream *game_file;
    int time_left;
    int opp_time;
    float minutes;
    int incr; // Winboard increment
    int winc; // UCI increment
    int binc; // UCI increment
    bool computer;
    bool computer_plays_white;
    std::string opponent_name;
    bool ics;
    bool forceMode;
    bool analyzeMode;
    bool editMode;
    ColorType side;
    int moves;
    Board *ponder_board, *main_board;
    PonderStatus ponder_status;
    Move predicted_move;
    Move ponder_move, best_move;
    Statistics stats, last_stats, ponder_stats;
    int time_target;
    int last_time_target;
    int computer_rating;
    int opponent_rating;
    // stack of pending commands, received but not yet executed:
    std::vector<std::string> pending;
    bool doTrace; // true if -t on command line
    bool easy; // set if no pondering
    bool game_end;
    bool result_pending;
    score_t last_score;
    ECO *ecoCoder;
    std::string hostname;
    bool xboard42;
    SearchType srctype;
    int time_limit;
    int ply_limit;
    std::string start_fen;

    bool uci;
    int movestogo;
    bool ponderhit;
    // set true if waiting for "ponderhit" or "stop"
    bool uciWaitState;
    std::string test_file;
    bool cpusSet; // true if cmd line specifies -c
    bool memorySet; // true if cmd line specifies -H
    std::string &debugPrefix;
    std::mutex inputMtx;

    Input input;

    struct UciStrengthOpts
    {
        bool limitStrength;
        int elo;

        UciStrengthOpts()
            : limitStrength(false),
              elo(Options::MAX_RATING)
            {
            }
    } uciStrengthOpts;

};

#endif
