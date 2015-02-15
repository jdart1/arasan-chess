// Copyright 2010-2015 by Jon Dart. All Rights Reserved.
#include "board.h"
#include "notation.h"
#include "legal.h"
#include "hash.h"
#include "globals.h"
#include "chessio.h"
#include "util.h"
#include "search.h"
#ifdef NOMAD
#include "nomad.hpp"
#endif
#include <iostream>
#include <fstream>
#include <iomanip>
#include <map>
extern "C" {
#include <math.h>
#include <ctype.h>
#include <semaphore.h>
};

#define CMAES

#ifdef CMAES
#include "cmaes.h"
using namespace libcmaes;
#endif

static int cores = 1;

static int SEARCH_DEPTH = 1;

static int FV_WINDOW = 256;

static int SEARCH_WINDOW = PAWN_VALUE/2;

static const int MAX_THREADS = 64;

static const int THREAD_STACK_SIZE = 8*1024*1024;

static string fen_file;

static vector<string> * positions = NULL;

static bool terminated = false;

enum Strategy { MMTO, Texel };

static const Strategy strategy = Texel;

// per-thread data
static struct ThreadData {
    SearchController *searcher;
    int index; 
    size_t offset;
    size_t size;
    double penalty;
    sem_t sem;
    sem_t done;
    THREAD thread_id;
} threadDatas[MAX_THREADS];

static pthread_attr_t stackSizeAttrib;


static int search(SearchController* searcher, const Board &board, int alpha, int beta, int depth) 
{
   int value;
   Statistics stats;
   searcher->findBestMove(board,
                          FixedDepth,
                          999999,
                          0,
                          depth, false, false,
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

double PARAM1 = 2.5;
double PARAM2 = 2.0;

static double sign(double x) 
{
   if (x == 0.0)
      return 0.0;
   else if (x > 0.0)
      return 1.0;
   else
      return -1.0;
}


static double texelSigmoid(double val) {
   //double s = -PARAM1*(-0.5+1.0/(1.0+pow(10.0,PARAM2*val/100.0)));
   double s = PARAM2*(-0.5+1.0/(1.0+exp(-PARAM1*val/400)));
    if (s < -1.0) return -1.0;
    if (s > 1.0) return 1.0;
    return s;
}

static double scale(double val, int i) 
{
   return (val-(double)Scoring::params[i].min_value)/double(Scoring::params[i].max_value-Scoring::params[i].min_value);
}

static double unscale(double val, int i) 
{
  return double(Scoring::params[i].max_value-Scoring::params[i].min_value)*val+(double)Scoring::params[i].min_value;
}

static double computeErrorMMTO(SearchController *searcher, const string &pos, uint64 line) 
{
   double penalty = 0.0;
   stringstream stream(pos);
   EPDRecord epd_rec;
   Board board;
   if (!ChessIO::readEPDRecord(stream,board,epd_rec)) {
      cerr << "error in EPD record, line " << line << endl;
      return 0.0;
   }
   if (epd_rec.hasError()) {
      cerr << "error in EPD record, line " << line;
      cerr << ": ";
      cerr << epd_rec.getError();
      cerr << endl;
      return 0.0;
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
      cerr << line << endl;
      return 0.0;
   }
   else if (!solution_moves.size()) {
      cerr << "no solution move(s) for EPD record, line " << line << endl;
      return 0.0;
   }

   // generate root moves
   RootMoveGenerator mg(board);

   // don't search if stalemate, mate, or forced move
   if (mg.moveCount() <= 1) {
      return 0.0;
   }

   Move best;
   int value = -Constants::MATE;

   Move keyMove;
   int best_key_value = -Constants::MATE;
   int i = 0;
   BoardState state(board.state);
   for (vector<Move>::const_iterator it = solution_moves.begin();
        it != solution_moves.end(); it++, i++) {
      keyMove = *it;
      int alpha, beta;
      if (i == 0) {
         alpha = -Constants::MATE;
         beta = Constants::MATE;
      } else {
         alpha = best_key_value;
         beta = best_key_value + SEARCH_WINDOW;
      }
      board.doMove(keyMove);
      int value = -search(searcher, board,-beta,-alpha,SEARCH_DEPTH);
      board.undoMove(keyMove,state);
      if (i > 0 && value >= beta && SEARCH_DEPTH > 0) {
         // window was not wide enough
         value = -search(searcher, board,-Constants::MATE,-alpha,SEARCH_DEPTH);
      }
      if (value > best_key_value) {
         best_key_value = value;
      }
   }
   int best_value = best_key_value;
   Move m;
   while (!IsNull(m = mg.nextMove())) {
      int solution = 0;
      for (vector<Move>::const_iterator it = solution_moves.begin();
           it != solution_moves.end();
           it++) {
         if (MovesEqual(m,*it)) {
            ++solution;
            break;
         }
      }
      if (solution) continue;
      board.doMove(m);
      int try_value = -search(searcher, board,-best_value-SEARCH_WINDOW,-best_value,SEARCH_DEPTH);
      board.undoMove(m,state);
      if (try_value > best_value) {
         if (try_value >= best_value + SEARCH_WINDOW) {
            // window was not wide enough
            try_value = -search(searcher, board,-Constants::MATE,-best_value,SEARCH_DEPTH);
         }
         if (try_value > best_value) {
            best_value = try_value;
         }
      }
   }
   if (best_value > best_key_value) {
      penalty += sigmoid(best_value-best_key_value);
   }
   return penalty;
}

static double computeErrorTexel(SearchController *searcher,const string &pos,uint64 line)
{
   
   double err = 0.0;
// This is a large object so put it on the heap:
   Scoring *scoring = NULL;
   if (SEARCH_DEPTH < 0) scoring = new Scoring();
   size_t lines = 0;
    
   Board b;

//   if (b.getMaterial(White).materialLevel() + 
//    b.getMaterial(Black).materialLevel() < 24 ) return 0.0;
   size_t split = pos.find_last_of(" ");
   string fen;
   int result = 0;
   if (split == string::npos) {
      cerr << "No result? line " << line << endl;
      return 0.0;
   } else {
      fen = pos.substr(0,split);
      string res = pos.substr(split+1);
      std::size_t last  = res.find_last_of(" \n");
      if (last != string::npos) {
         res = res.substr(0,last);
      }
      
      if (res == "0-1")
         result = -1;
      else if (res == "1-0")
         result = 1;
      else if (res == "1/2-1/2")
         result = 0;
      else if (res == "*")
         return 0.0;
      else {
         cerr << "Missing or unrecognized result, line " << line << " (" << res << ")" << endl;
         return 0.0;
      }
   }
        
   stringstream s(fen);
   s >> b;
   if (s.fail()) {
      cerr << "error on FEN string: " << fen << ", line " << line << endl;
      return 0.0;
   }
   int value;
   if (SEARCH_DEPTH < 0) {
      value = scoring->evalu8(b);
   } else if (SEARCH_DEPTH == 0) {
//      value = searcher->quiesce(-Constants::MATE,Constants::MATE,0,0);
   } else {
      value = search(searcher,b,-5*PAWN_VALUE,5*PAWN_VALUE,SEARCH_DEPTH);
   }
   if (b.sideToMove() == Black) value = -value;
   double predict = texelSigmoid(value);

//   cout << " value=" << value << " predict=" << predict << " result=" << result << " err = " <<  ((double)result - predict)*((double)result - predict) << endl;
        
   err += ((double)result - predict)*((double)result - predict);

   if (SEARCH_DEPTH < 0)
      delete scoring;

   return err;
}

// compute a part of the objective in a single thread
static double computeError(SearchController *searcher, int index, size_t offset,
   size_t size) {
   string buf;
   double penalty = 0.0;
   uint64 done = 0;
   uint64 lines = 0;
   for (vector<string>::const_iterator it = positions->begin()+offset;
        it != positions->end() && done < size;
        it++, done++, lines++) {
      if (strategy == MMTO) {
         penalty += computeErrorMMTO(searcher,*it,lines);
      } else {
         penalty += computeErrorTexel(searcher,*it,lines);
      }
   }
    
//   cout << "thread " << index << " done" << endl;
    
   return penalty;
}

static void * CDECL threadp(void *x)
{
   ThreadData *td = (ThreadData*)x;

   // set stack size
   size_t stackSize;
   if (pthread_attr_getstacksize(&stackSizeAttrib, &stackSize)) {
        perror("pthread_attr_getstacksize");
        return 0;
   }
   if (stackSize < THREAD_STACK_SIZE) {
      if (pthread_attr_setstacksize (&stackSizeAttrib, THREAD_STACK_SIZE)) {
         perror("error setting thread stack size");
         return 0;
      }
   }

   // allocate controller in the thread
   try {
      td->searcher = new SearchController();
   } catch(std::bad_alloc) {
      cerr << "out of memory, thread " << td->index << endl;
      return 0;
   }
   
   while (!terminated) {
      // wait until signalled
      sem_wait(&td->sem);
      td->penalty = computeError(td->searcher,td->index,td->offset,td->size);
      // tell parent we are done
      sem_post(&td->done);
   }
   delete td->searcher;
   return 0;
}
   
static void initThreads() 
{
    // prepare threads
    if (pthread_attr_init (&stackSizeAttrib)) {
       perror("pthread_attr_init");
       return;
    }
    for (int i = 0; i < cores; i++) {
        THREAD thread_id;
        threadDatas[i].index = i;
        threadDatas[i].searcher = NULL;
        sem_init(&threadDatas[i].sem,0,0);
        sem_init(&threadDatas[i].done,0,0);
        if (pthread_create(&(threadDatas[i].thread_id), &stackSizeAttrib, threadp, (void*)&(threadDatas[i]))) {
            perror("thread creation failed");
        }
        cout << "thread " << i << " created." << endl;
    }
}

static double computeLsqError() {
   
   for (int i = 0; i < cores; i++) {
      threadDatas[i].searcher->clearHashTables();
      threadDatas[i].penalty = 0.0;
      // signal searchers to start
      sem_post(&threadDatas[i].sem);
   }
   // wait for all searchers done
   for (int i = 0; i < cores; i++) {
      sem_wait(&threadDatas[i].done);
   }
   cout << "all searchers done" << endl;

   // total errors from the threads
   double total = 0.0;
   for (int i = 0; i < cores; i++) {
      total += threadDatas[i].penalty;
   }
   return total;
}

#ifdef CMAES

static FitFunc evaluator = [](const double *x, const int dim) 
{
   for ( int i = 0 ; i < Scoring::NUM_PARAMS ; i++ ) 
   {
      Scoring::params[i].current = round(unscale(x[i],i));
   }
   Scoring::initParams();
   double err = computeLsqError()/positions->size();
   cout << "objective=" << err << endl;
   return err;
   
};

static ProgressFunc<CMAParameters<GenoPheno<pwqBoundStrategy>>,CMASolutions> progress = [](const CMAParameters<GenoPheno<pwqBoundStrategy>> &cmaparams, const CMASolutions &cmasols)
{
   
   std::cout << cmasols.niter() << " iterations, " << 
   cmasols.fevals() << " function evals, elapsed time: " << cmasols.elapsed_last_iter() << " sigma=" << cmasols.sigma() << endl;
   std::cout << "best solution: " << cmasols << std::endl;
   vector <double> x0 = cmasols.best_candidate().get_x();
   cout << "denormalized solution: " << endl;
   int i = 0;
        
   for (vector<double>::const_iterator it = x0.begin();
        it != x0.end();
        it++,i++) {
      cout << " " << Scoring::params[i].name << ": " << round(unscale(*it,i)) << endl;
   }
   cout << endl;
   
   return 0;
};


#else // NOMAD

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
         double quality = computeLsqError()/positions->size();
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
#endif

static uint64 readTrainingFile() {
   cout << "reading training file ..." << endl;
   positions = new vector<string>();
   uint64 lines = (uint64)0;
   ifstream pos_file( fen_file.c_str(), ios::in);
   string buf;
   while (pos_file.good()) {
      std::getline(pos_file,buf);
      ++lines;
      positions->push_back(buf);
   }
   cout << "training file read, " << lines << " lines" << endl;
   return lines;
}

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
    options.search.hash_table_size = 64000;
//    if (EGTBMenCount) {
//        cerr << "Initialized tablebases" << endl;
//    }
    options.book.book_enabled = options.log_enabled = 0;
    options.learning.position_learning = false;
    options.search.use_tablebases = false;
    options.search.easy_plies = 0;

    if (argc < 2) {
        cerr << "not enough arguments" << endl;
        return -1;
    }
    else {
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
        
#ifdef NOMAD
        string paramFile = argv[arg++];
#endif
        fen_file = argv[arg];
        cout << "plies=" << SEARCH_DEPTH << " cores=" << cores;
#ifdef NOMAD
        cout << " param file=" << paramFile;
#endif
        cout << " tune file=" << fen_file << (flush) << endl;

        uint64 lines = readTrainingFile();
        
        initThreads();

        uint64 chunk = lines / cores;

        cout << "chunk size=" << chunk << endl;

        uint64 off = (uint64)0;

        for (int i = 0; i < cores; i++) {
           threadDatas[i].index = i;
           threadDatas[i].penalty = 0.0;
           threadDatas[i].offset = (size_t)off;
           uint64 size = chunk;
           size = (i==cores-1) ? (lines-off) : chunk;
           threadDatas[i].size = size;
           off += chunk;
        }
        
#ifdef CMAES
        int dim = Scoring::NUM_PARAMS;
        vector<double> x0;
        double sigma = 0.05;
        // use variant with box bounds
        double lbounds[Scoring::NUM_PARAMS],
           ubounds[Scoring::NUM_PARAMS];
        for (int i = 0; i < Scoring::NUM_PARAMS; i++) {
           // initialize & normalize
           x0.push_back(scale(double(Scoring::params[i].current),i));
           lbounds[i] = (double)Scoring::params[i].min_value;
           ubounds[i] = (double)Scoring::params[i].max_value;
        }
        GenoPheno<pwqBoundStrategy> gp(lbounds,ubounds,dim);
        CMAParameters<GenoPheno<pwqBoundStrategy>> cmaparams(dim,&x0.front(),sigma,-1,0,gp);
        CMASolutions cmasols = cmaes<GenoPheno<pwqBoundStrategy>>(evaluator,cmaparams,progress);
#else
        
        
        // NOMAD
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
#endif
    }
    for (int i = 0; i < cores; i++) {
        delete threadDatas[i].searcher;
    }
    return 0;
}
