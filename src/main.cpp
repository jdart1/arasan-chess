#include <fstream>
#include <iostream>
#include "chess.h"
#include "bitboard.h"
#include "attacks.h"
#include "scoring.h"
#include "board.h"
#include "movegen.h"
#include "search.h"
#include "chessio.h"
#include "globals.h"

int CDECL main(int argc, char ** argv) {
   Board board;
   Bitboard::init();

   init_globals(argv[0]);

   Attacks::init();
   Scoring::init();
   int arg = 1;
   if (argc < 2) return -1;
#if defined (_WIN32) || (defined (__GNUC__) && (__GNUC__ >=3))
      ifstream pos_file( argv[arg], ios::in);
#else
      ifstream pos_file( argv[arg], ios::in | ios::nocreate);
#endif
   if (!pos_file.good())
   {
      cout << "File not found, or bad format." << endl;
      return -1;
   }
   while (pos_file.good() && !pos_file.eof()) {
     char buf[512];
      pos_file.getline(buf,511);
      if (!pos_file)
      {
	break;
      }
      // Try to parse this line as an EPD command.
#if defined(__GNUC__) && _GNUC_PREREQ(3,2)
      string istr(buf);
      istringstream stream(istr);
#else
      istrstream stream(buf,512);
#endif
      string id;
      EPDRecord *epd_rec =
        ChessIO::readEPDRecord(stream,board);
      Scoring scor;
      SearchContext c;
      SearchController ctrl;
      Statistics stats;
      Move best = ctrl.findBestMove( 
   			      board, FixedTime,
   			      60*100,0,Constants::MaxPly,0,0,stats,Debug);
   }
   return 0;
}
