// Copyright 2010-2015 by Jon Dart. All Rights Reserved.
#include "board.h"
#include "notation.h"
#include "legal.h"
#include "hash.h"
#include "globals.h"
#include "chessio.h"
#include "util.h"
#include "search.h"
#include "nomad.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <map>
extern "C" {
#include <math.h>
#include <ctype.h>
#ifndef _WIN32
#include <semaphore.h>
#endif
};

static int cores = 1;

static int SEARCH_DEPTH = 1;

static int FV_WINDOW = 256;

static int SEARCH_WINDOW = PAWN_VALUE/2;

static const int MAX_THREADS = 64;

static const int THREAD_STACK_SIZE = 8*1024*1024;

static string fen_file;

// results from the threads
static double errors[MAX_THREADS];

struct ThreadData {
    SearchController *searcher;
    int index;
    THREAD thread_id;
} threadDatas[MAX_THREADS];

#ifndef _WIN32
static pthread_attr_t stackSizeAttrib;
#endif

static int search(SearchController* searcher, const Board &board, int alpha, int beta) 
{
   int value;
   options.search.easy_plies = 0;
   options.learning.position_learning = 0;
   Statistics stats;
   searcher->findBestMove(board,
                          FixedDepth,
                          999999,
                          0,
                          SEARCH_DEPTH, false, false,
                          stats,
                          Silent);
   value = stats.value;
   return value;
}

static double sigmoid( double x )
{
   
   const double delta = (double)FV_WINDOW / 7.0;
   double dd, dn, dtemp, dret;
   if      ( x <= -FV_WINDOW ) {
      dret = 0.0;
   }
   else if ( x >=  FV_WINDOW ) {
      dret = 0.0;
   }
   else {
      dn    = exp( - x / delta );
      dtemp = dn + 1.0;
      dd    = delta * dtemp * dtemp;
      dret  = dn / dd;
   }
   return dret;
   
}

static double computeError(SearchController *searcher, int index) {
    double err = 0.0;
	// This is a large object so put it on the heap:
    Scoring *scoring = NULL;
    if (SEARCH_DEPTH < 0) scoring = new Scoring();
    
    uint64 lines = 0ULL;
    string buf;
    double penalty = 0.0;
    ifstream pos_file( fen_file.c_str(), ios::in);
    while (!pos_file.eof()) {
      std::getline(pos_file,buf);
      ++lines;
      stringstream stream(buf);
      EPDRecord epd_rec;
      Board board;
      if (!ChessIO::readEPDRecord(stream,board,epd_rec)) {
         cerr << "error in EPD record, line " << lines << endl;
         continue;
      }
      if (epd_rec.hasError()) {
         cerr << "error in EPD record, line " << lines;
         cerr << ": ";
         cerr << epd_rec.getError();
         cerr << endl;
         continue;
      }
      vector <Move> solution_moves;
      int illegal=0;
      string id, comment;
      for (int i = 0; i < epd_rec.getSize(); i++) {
         string key, val;
         epd_rec.getData(i,key,val);
         if (key == "bm") {
            Move m;
            stringstream s(val);
            while (!s.eof()) {
               string moveStr;
               // skips spaces
               s >> moveStr;
               if (!moveStr.length()) break;
               m = Notation::value(board,board.sideToMove(),Notation::SAN_IN,moveStr);
               if (IsNull(m)) {
                  ++illegal;
               } else {
                  solution_moves.push_back(m);
               }
            }
         }
         else if (key == "id") {
            id = val;
         }
         else if (key == "c0") {
            comment = val;
         }
      }
      if (illegal) {
         cerr << "illegal or invalid solution move(s) for EPD record, line ";
         cerr << lines << endl;
         continue;
      }
      else if (!solution_moves.size()) {
         cerr << "no solution move(s) for EPD record, line " << lines << endl;
         continue;
      }

      // generate root moves
      RootMoveGenerator mg(board);

      // don't search if stalemate, mate, or forced move
      if (mg.moveCount() <= 1) continue;

      Move moves[Constants::MaxMoves];
      int keys = 0;
      vector<Move>::const_iterator it = solution_moves.begin();
      // key moves go first
      while (it != solution_moves.end()) {
         moves[keys++] = *it++;
      }
      Move m;
      int count = keys;
      while ((m=mg.nextMove())!=NullMove) {
         int akey = 0;
         for (int i = 0; i < keys; i++) {
            if (MovesEqual(m,moves[i])) {
               akey++;
               break;
            }
         }
         if (akey) continue;
         moves[count++] = m;
      }

      BoardState state(board.state);
      int best = -Constants::MATE;
      int best_key_value = -Constants::MATE;
      for (int i = 0; i < count; i++) {
         int alpha, beta;
         if (i == 0) {
            alpha = -Constants::MATE;
            beta = Constants::MATE;
         } else {
            alpha = best;
            beta = best + SEARCH_WINDOW;
         }
         board.doMove(moves[i]);
         int value = -search(searcher, board,-beta,-alpha);
         board.undoMove(moves[i],state);
         if (i > 0 && value >= beta && SEARCH_DEPTH > 0) {
            // window was not wide enough
            value = -search(searcher, board,-Constants::MATE,-alpha);
         }
         if (value > best) {
            best = value;
            if (i < keys) {
               best_key_value = value;
            }
         }
         if (i >= keys && value > best_key_value) {
            // non-key move scores higher, give penalty
            penalty += sigmoid(value-best_key_value);
         }
      }
    }
    if (SEARCH_DEPTH < 0) delete scoring;
    cout << "thread " << index << " done" << endl;
    
    return penalty;

}

#ifdef _WIN32
static DWORD WINAPI threadp(void *x)
#else
static void * CDECL threadp(void *x)
#endif
{
    ThreadData *td = (ThreadData*)x;
    if (!td->searcher) td->searcher = new SearchController();
//    cout << "thread " << td->index << " starting" << endl;
    errors[td->index] = computeError(td->searcher,td->index);
    return 0;
}

static double computeLsqError() {
    // prepare threads
#ifndef _WIN32
   if (pthread_attr_init (&stackSizeAttrib)) {
      perror("pthread_attr_init");
      return 0.0;
   }
   size_t stackSize;
   if (pthread_attr_getstacksize(&stackSizeAttrib, &stackSize)) {
        perror("pthread_attr_getstacksize");
        return 0.0;
   }
   if (stackSize < THREAD_STACK_SIZE) {
      if (pthread_attr_setstacksize (&stackSizeAttrib, THREAD_STACK_SIZE)) {
         perror("error setting thread stack size");
         return 0.0;
      }
   }
//   cout << "initialized" << endl;
#endif
   
    for (int i = 0; i < cores; i++) {
        THREAD thread_id;
        threadDatas[i].index = i;
#ifdef _WIN32
        DWORD id;
        thread_id = CreateThread(NULL,8*1024*1024,
                                 threadp,threadDatas+i,
                                 0,
                                 &id);

        if (thread_id == NULL) {
            cerr << "thread creation failed" << endl;
        }
#else
        if (pthread_create(&(threadDatas[i].thread_id), &stackSizeAttrib, threadp, (void*)(threadDatas+i))) {
            perror("thread creation failed");
        }
#endif
//        cout << "thread " << i << " created." << endl;
    }
    // Wait for threads to complete
#ifdef _WIN32
    HANDLE handles[MAX_THREADS];
    for (int i = 0; i<cores; i++) {
        handles[i] = threadDatas[i].thread_id;
    }
    WaitForMultipleObjects(cores,handles,TRUE,INFINITE);
#else
    for (int i = 0; i < cores; i++) {
       void *status;
       if (pthread_join(threadDatas[i].thread_id,&status)) {
          perror("pthread_join");
       }
    }
    if (pthread_attr_destroy(&stackSizeAttrib)) {
       perror("pthread_attr_destroy");
    }
#endif        
    // total errors from the threads
    double total = 0;
    for (int i = 0; i < cores; i++) {
        total += errors[i];
    }
    cout << "result: " << setprecision(8) << total << endl;
    return total;
}

/*----------------------------------------*/
/*               The problem              */
/*----------------------------------------*/
class My_Evaluator : public NOMAD::Evaluator {
public:
   My_Evaluator  ( const NOMAD::Parameters & p ) :
      NOMAD::Evaluator ( p ) {}

   ~My_Evaluator ( void ) {}

   bool eval_x ( NOMAD::Eval_Point   & x          ,
                 const NOMAD::Double & h_max      ,
                 bool                & count_eval   ) const 
      {
         for ( int i = 0 ; i < Scoring::NUM_PARAMS ; i++ ) 
         {
            Scoring::params[i].current = x[i].round();
         }
         Scoring::initParams();
         cout << "computing" << endl;
         double quality = computeLsqError();
         cout << "quality= " << quality << endl;
         
         NOMAD::Double q = quality;
         
         x.set_bb_output  ( 0 , q  ); // objective value

         count_eval = true; // count a black-box evaluation
 
         return true;       // the evaluation succeeded
      }


       void update_iteration ( NOMAD::success_type  success,
                                             const NOMAD::Stats &  stats,
                                             const NOMAD::Evaluator_Control &  ev_control,
                                             const NOMAD::Barrier &  true_barrier,
                                             const NOMAD::Barrier &  sgte_barrier,
                                             const NOMAD::Pareto_Front &  pareto_front,
                               bool &  stop )  
      {
         cout << "iterations = " << stats.get_iterations() << 
            ", black box evals = " << stats.get_bb_eval() << endl;
      }

};

int CDECL main(int argc, char **argv)
{
    Bitboard::init();
    initOptions(argv[0]);
    Attacks::init();
    Scoring::init();
    if (!initGlobals(argv[0], false)) {
        cleanupGlobals();
        exit(-1);
    }
    atexit(cleanupGlobals);
    delayedInit();
    options.search.hash_table_size = 0;
    if (EGTBMenCount) {
        cerr << "Initialized tablebases" << endl;
    }
    options.book.book_enabled = options.log_enabled = 0;

    if (argc < 2) {
        cerr << "not enough arguments" << endl;
        return -1;
    }
    else {
        Statistics stats;
        int arg = 1;
        while (arg < argc && argv[arg][0] == '-') {
           if (strcmp(argv[arg],"-c")==0) {
              ++arg;
              cores = atoi(argv[arg]);
           }
           else if (strcmp(argv[arg],"-p")==0) {
              ++arg;
              SEARCH_DEPTH = atoi(argv[arg]);
           }
           ++arg;
        }
        
        for (int i = 0; i < cores; i++) {
           threadDatas[i].searcher = NULL;
        }
        
        string paramFile = argv[arg++];
        fen_file = argv[arg];

        cout << "plies=" << SEARCH_DEPTH << " cores=" << cores << " param file=" << paramFile << " tune file=" << fen_file << (flush) << endl;
        

        try {
           
        NOMAD::Display out ( std::cout );
        out.precision ( NOMAD::DISPLAY_PRECISION_STD );

        NOMAD::begin ( argc-arg+1 , argv+arg-1 );

        srand((unsigned)(getCurrentTime() % (1<<31)));
        NOMAD::RNG::set_seed(rand() % 12345);

        NOMAD::Parameters p(out);
        cout << "reading parameter file " << paramFile << endl;
        p.read(paramFile);
        // parameters validation:
        p.check();
        cout << "parameter check passed" << endl;
        const vector<NOMAD::Point * > x0s = p.get_x0s();
        for (vector<NOMAD::Point *>::const_iterator it =  x0s.begin();
             it != x0s.end();
             it++) {
           cout << *(*it) << endl;
        }       

        // custom evaluator creation:
        My_Evaluator ev   ( p );

        // algorithm creation and execution:
        NOMAD::Mads mads ( p , &ev );
        mads.run();
        
        }
        catch ( exception & e ) {
           cerr << "\nNOMAD has been interrupted (" << e.what() << ")\n\n";
        }
    }
    for (int i = 0; i < cores; i++) {
        delete threadDatas[i].searcher;
    }
    return 0;
}
