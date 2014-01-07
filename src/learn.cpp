// Copyright 1994-2002, 2004, 2008-2009 by Jon Dart.  All Rights Reserved.

#include "learn.h"
#include "globals.h"
#include "book.h"
#include "util.h"
#include "scoring.h"
#include <sstream>

// max ply for position learning
#define POSITION_MAX_PLY 60

void learn(const Board &board, int rep_count)
{
   // First, do position learning. If our score has dropped or
   // jumped recently, append an entry to the position learning file.

   if (theLog->current() == 0) return;
   const LogEntry &last_entry = (*theLog)[theLog->current()-1];
   
   int last_score = last_entry.score();
   int last_depth = last_entry.depth();
   const BookInfo &book_info = last_entry.getBookInfo();
   if (options.learning.position_learning && !book_info.isValid() && theLog->current() >= 3)
   {
      int i = theLog->current()-1;
      int out_of_book = 0;
      while (i>=0) {
         const LogEntry &prev = (*theLog)[i];
         const BookInfo &book_info = prev.getBookInfo();
	 if (book_info.isValid()) break;
	 ++out_of_book;
	 i-=2;
      }
      if (out_of_book >= 3 && out_of_book <= 20) {
         int diff1 = 0;
         int diff2 = 0;
         const LogEntry &prev = (*theLog)[theLog->current()-3];
         diff1 = Util::Abs(last_score - prev.score());
         if (theLog->current()>=5) {
            const LogEntry &prev = (*theLog)[theLog->current()-5];
            const BookInfo &book_info = prev.getBookInfo();
	    if (!book_info.isValid()) {
               diff2 = Util::Abs(last_score - prev.score());
	    }
         }
         int score_threshold = options.learning.position_learning_threshold;
         if (last_depth > options.learning.position_learning_minDepth &&
            (diff1 > score_threshold || diff2 > score_threshold)) {
            // last 2 or more moves were not from book, and score has changed
            // significantly. Append to the learn file.
            stringstream s;
            s << (hex) << board.hashCode(rep_count);
            s << ' ' << (dec) << (board.checkStatus() == InCheck) << 
                 ' ' << last_score << ' ' << last_depth << ' ';
            MoveImage(last_entry.move(),s);
            s << '\n';
            ofstream log;
            log.open(learnFileName.c_str(),ios_base::out | ios_base::app);
            log << s.str();
            log.close();
            stringstream str;
            str << "learning position, score = ";
            Scoring::printScore(last_score,str);
            str << " depth = " << last_depth << "\n";
            theLog->write(str.str());
         }
      }
   }

   // If we are a few moves out of the opening book, and if 
   // our score is considerably off zero, update the book to
   // avoid or steer towards this position in the future.
   //
   if (!options.learning.score_learning) return;
   int count = options.learning.score_learning_horizon;
   int last_book_move = -1;
   int min_score = Constants::MATE, max_score = -Constants::MATE;
   int i;
   for (i = theLog->current()-1; i>=0 && count>=0; i-=2,--count)
   {
      const LogEntry &entry = (*theLog)[i];
      const BookInfo &book_info = entry.getBookInfo();
      if (book_info.isValid()) // it was a book move
      {
         if (count > 0) return; // not at learning threshold
         last_book_move = i;
         break;
      }
      else
      {
         if (entry.score() < min_score)
            min_score = entry.score();
         if (entry.score() > max_score)
            max_score = entry.score();
      }
   }
   if (last_book_move < 0) return;
   if ((last_depth < options.learning.score_learning_minDepth) ||
       (Util::Abs(last_score) < options.learning.score_learning_threshold))
   {
      return; // we don't have an accurate score, or the
              // most recent score is close to even
   }
   theLog->write("learning"); theLog->write_eol();
   float learn_factor = (last_score*1.0F)/64.0F;
   int first = 1;
   for (i = last_book_move; i >= 0; i-=2)
   {
      const LogEntry &entry = (*theLog)[i];
      const BookInfo &book_info = entry.getBookInfo();
      if (book_info.isValid())
      {
         if (!first)
         {
            learn_factor /= 1.0F*book_info.getTotalMoves();
         }
         openingBook->update(book_info,learn_factor);
         first = 0;
      }
   }
}

void learnResult(ColorType computerSide, Log::GameResult result, int ratingDiff)
{
   int last_book_move = -1;
   int i;
   for (i = theLog->current()-1; i>=0; i--)
   {
      const LogEntry &entry = (*theLog)[i];
      const BookInfo &book_info = entry.getBookInfo();
      if (book_info.isValid()) // it was a book move
      {
         last_book_move = i;
         break;
      }
   }
   if (last_book_move < 0) return;
   int diff = 0;
   // don't learn from low-rated opponents
   if (ratingDiff < -250) return;
   if (result == Log::DrawResult) {
     if (ratingDiff>100) 
       diff = 1; // draw from hi-rated opponent
     else
       return; // regular draw
   } else {
      diff = (result == Log::WhiteWin) ? 1 : -1;
      if (computerSide == Black) diff = -diff;
   }
   BookLocation loc;
   while (i>0) {
     const LogEntry &entry = (*theLog)[i];
     BookInfo &book_info = (BookInfo&)entry.getBookInfo();
     book_info.getLocation(loc);
     int before = book_info.get_learned_result();
     book_info.update_learned_result(diff);      
     int after = book_info.get_learned_result();
     openingBook->update(&book_info);

     stringstream str;
     str << "Learning result .. page = " << (int)loc.page << " index = " <<
       (int)loc.index << " current value = " << before << " new value = " << after << '\n';
     theLog->write(str.str());

     // see if there's an alternative book move at this position
     if (openingBook->reader->book_move_count(book_info.hashCode())> 1)
       return; // all done
     i-=2;
   }
}

int getLearnRecord(istream &learnFile, LearnRecord &rec) {
  learnFile >> (hex) >> rec.hashcode >> (dec) >> rec.in_check >>
    rec.score >> rec.depth;
  string moveImage;
  learnFile >> moveImage;
  rec.start = rec.dest = InvalidSquare;
  rec.promotion = Empty;
  if (moveImage != "") {
    // Parse the move, which is in simple algebraic (not SAN)
    const char *p = moveImage.c_str();
    rec.start = SquareValue(p);
    if (strlen(p) > 3) p+=3;
    rec.dest = SquareValue(p);
    if (strlen(p) > 2) {
      p+=2;
      if (*p == '=') {
        p++;
        switch(*p) {
        case 'B': rec.promotion = Bishop; break;
        case 'N': rec.promotion = Knight; break;
        case 'R': rec.promotion = Rook; break;
        case 'Q': rec.promotion = Queen; break;
        default: break;
        }
      }
    }
  }
  return learnFile.good() && !learnFile.eof();
}
