// Copyright 1994-2002, 2004, 2008-2009, 2014, 2017, 2021 by Jon Dart.
// All Rights Reserved.

#include "learn.h"
#include "globals.h"
#include "scoring.h"
#include <cstddef>
#include <sstream>

// max ply for position learning
#define POSITION_MAX_PLY 60

void learn(const Board &board, int rep_count) {
    // Do position learning. If our score has dropped or
    // jumped recently, append an entry to the position learning file.

    if (globals::theLog->current() == 0)
        return;
    const LogEntry &last_entry =
        (*globals::theLog)[globals::theLog->current() - 1];

    score_t last_score = last_entry.score();
    score_t last_depth = last_entry.depth();
    if (globals::options.learning.position_learning && !last_entry.fromBook() &&
        globals::theLog->current() >= 3) {
        int i = globals::theLog->current() - 1;
        int out_of_book = 0;
        while (i >= 0) {
            const LogEntry &prev = (*globals::theLog)[i];
            if (prev.fromBook())
                break;
            ++out_of_book;
            i -= 2;
        }
        if (out_of_book >= 3 && out_of_book <= 20) {
            score_t diff1 = 0, diff2 = 0;
            const LogEntry &prev =
                (*globals::theLog)[globals::theLog->current() - 3];
#ifdef TUNE
            diff1 = fabs(last_score - prev.score());
#else
            diff1 = std::abs(last_score - prev.score());
#endif
            if (globals::theLog->current() >= 5) {
                const LogEntry &prev =
                    (*globals::theLog)[globals::theLog->current() - 5];
                if (!prev.fromBook()) {
#ifdef TUNE
                    diff2 = fabs(last_score - prev.score());
#else
                    diff2 = std::abs(last_score - prev.score());
#endif
                }
            }
            int score_threshold =
                globals::options.learning.position_learning_threshold;
            if (last_depth >
                    globals::options.learning.position_learning_minDepth &&
                (diff1 > score_threshold || diff2 > score_threshold)) {
                // last 2 or more moves were not from book, and score has
                // changed significantly. Append to the learn file.
                std::stringstream s;
                s << (std::hex) << board.hashCode(rep_count);
                s << ' ' << (std::dec) << (board.checkStatus() == InCheck)
                  << ' ' << last_score << ' ' << last_depth << ' ';
                MoveImage(last_entry.move(), s);
                s << '\n';
                std::ofstream log;
                log.open(globals::learnFileName.c_str(),
                         std::ios_base::out | std::ios_base::app);
                log << s.str();
                log.close();
                std::stringstream str;
                str << "learning position, score = ";
                Scoring::printScore(last_score, str);
                str << " depth = " << last_depth << "\n";
                globals::theLog->write(str.str());
            }
        }
    }
}

int getLearnRecord(std::istream &learnFile, LearnRecord &rec) {
    learnFile >> std::hex >> rec.hashcode >> std::dec >> rec.in_check >>
        rec.score >> rec.depth;
    std::string moveImage;
    learnFile >> moveImage;
    rec.start = rec.dest = InvalidSquare;
    rec.promotion = Empty;
    if (moveImage != "") {
        // Parse the move, which is in simple algebraic (not SAN)
        const char *p = moveImage.c_str();
        rec.start = SquareValue(p);
        if (strlen(p) > 3)
            p += 3;
        rec.dest = SquareValue(p);
        if (strlen(p) > 2) {
            p += 2;
            if (*p == '=') {
                p++;
                switch (*p) {
                case 'B':
                    rec.promotion = Bishop;
                    break;
                case 'N':
                    rec.promotion = Knight;
                    break;
                case 'R':
                    rec.promotion = Rook;
                    break;
                case 'Q':
                    rec.promotion = Queen;
                    break;
                default:
                    break;
                }
            }
        }
    }
    return learnFile.good() && !learnFile.eof();
}
