// Copyright 2010-2015 by Jon Dart. All Rights Reserved.
#include "board.h"
#include "boardio.h"
#include "notation.h"
#include "legal.h"
#include "hash.h"
#include "globals.h"
#include "chessio.h"
#include "util.h"
#include "search.h"
#include "tune.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <map>
extern "C" {
#include <math.h>
#include <ctype.h>
#include <semaphore.h>
};

#include "Rockstar.hpp"
#include <Eigen/Core>

static int cores = 1;

static int SEARCH_DEPTH = 1;

static int FV_WINDOW = 256;

static int SEARCH_WINDOW = PAWN_VALUE/2;

static const int MAX_THREADS = 64;

static const int THREAD_STACK_SIZE = 8*1024*1024;

static string fen_file;

static vector<string> * positions = NULL;

static bool terminated = false;

static string out_file_name="params.cpp";

static string x0_file_name="x0";

enum Strategy { MMTO, Texel };

static const Strategy strategy = Texel;

static double min_cost = 1.0;

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

double PARAM1 = 2.69044;
double PARAM2 = 1.82372;

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

static double scale(const tune::TuneParam &t) 
{
   return double(t.current-t.min_value)/double(t.max_value-t.min_value);
}

static int unscale(double val, const tune::TuneParam &t)
{
   return round(double(t.max_value-t.min_value)*val+(double)t.min_value);
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
        
   if (!BoardIO::readFEN(b,fen)) {
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
   double predict = texelSigmoid(value/10.0);

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
   uint64 lines = (uint64)offset;
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
      td->searcher->clearHashTables();
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


static double evaluate(const Eigen::VectorXd &x, const int dim) 
{
   int i;
   for (i = 0; i < tune::NUM_TUNING_PARAMS; i++) 
   {
      tune::tune_params[i].current = unscale(x(i),tune::tune_params[i]);
   }
   tune::initParams();
   double err = computeLsqError()/positions->size();
   return err;
};

static void writeProgress(Eigen::VectorXd &theta)
{
   if (x0_file_name.length()) {
      ofstream x0_out(x0_file_name,ios::out | ios::trunc);
      tune::writeX0(x0_out);
   } else {
      tune::writeX0(cout);
   }
   if (out_file_name.length()) {
      ofstream param_out(out_file_name,ios::out | ios::trunc);
      Scoring::Params::write(param_out);
      param_out << endl;
   } else {
      Scoring::Params::write(cout);
      cout << endl;
   }
};

static uint64 readTrainingFile() {
   cout << "reading training file ..." << endl;
   positions = new vector<string>();
   uint64 lines = (uint64)0;
   ifstream pos_file( fen_file.c_str(), ios::in);
   string buf;
   while (pos_file.good()) {
      std::getline(pos_file,buf);
      if (buf.length()) {
         ++lines;
         positions->push_back(buf);
      }
   }
   cout << "training file read, " << lines << " lines" << endl;
   return lines;
}

static void usage() 
{
   cerr << "Usage: tuner -i <input objective file> -c <threads> -p <search depth>" << endl;
   cerr << "-o <output parameter file> -x <output objective file> training_file" << endl;
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
    options.search.hash_table_size = 0;

//    if (EGTBMenCount) {
//        cerr << "Initialized tablebases" << endl;
//    }
    options.book.book_enabled = options.log_enabled = 0;
    options.learning.position_learning = false;
#if defined(GAVIOTA_TBS) || defined(NALIMOV_TBS)
    options.search.use_tablebases = false;
#endif
    options.search.easy_plies = 0;

    string input_file;
    
   cout << "writing initial solution" << endl;
   tune::initParams();
   ofstream param_out(out_file_name,ios::out | ios::trunc);
   Scoring::Params::write(param_out);
   param_out << endl;


    if (argc < 2) {
       usage();
       return -1;
    }
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
       else if (strcmp(argv[arg],"-i")==0) {
          ++arg;
          input_file = argv[arg];
       }
       else if (strcmp(argv[arg],"-o")==0) {
          ++arg;
          out_file_name = argv[arg];
       }
       else if (strcmp(argv[arg],"-x")==0) {
          ++arg;
          x0_file_name = argv[arg];
       } else {
          cerr << "invalid option: " << argv[arg] << endl;
          usage();
          exit(-1);
       }
       ++arg;
    }
        
    fen_file = argv[arg];
    cout << "plies=" << SEARCH_DEPTH << " cores=" << cores;
    cout << " tune file=" << fen_file;
    if (input_file.length()) {
       cout << " input file=" << input_file;
    }
    cout << (flush) << endl;

    if (input_file.length()) {
       ifstream is(input_file);
       if (is.good()) {
          tune::readX0(is);
//          tune::checkParams();
       }
       else {
          cerr << "warning: cannot open input file " << input_file << endl;
       }
    }           

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

    Eigen::VectorXd x0 = Eigen::VectorXd::Zero(tune::NUM_TUNING_PARAMS,1);
    // initialize & normalize
    for (int i = 0; i < tune::NUM_TUNING_PARAMS; i++) {
       x0(i) = scale(tune::tune_params[i]);
    }
    Eigen::VectorXd Initial_StandardDeviation = Eigen::VectorXd::Ones(tune::NUM_TUNING_PARAMS,1) * 0.05;
    Eigen::VectorXd theta = Eigen::VectorXd::Zero(tune::NUM_TUNING_PARAMS,1);

    int initial_exp = 2;
    rockstar::Rockstar optimizer(x0,Initial_StandardDeviation, initial_exp);

    for(int i=0; i<1200; i++){
      optimizer.getNextTheta2Evaluate(theta);
      double cost = evaluate(theta,tune::NUM_TUNING_PARAMS);
      optimizer.setTheCostFromTheLastTheta(cost);

      std::cout<<optimizer.getRolloutNumber()<<"th rollout, this cost is   "<<cost<<", sigma is "<<optimizer.getSigma()<<std::endl;
      if (cost < min_cost) {
         writeProgress(theta);
         min_cost = cost;
      }
    }

    for (int i = 0; i < cores; i++) {
       delete threadDatas[i].searcher;
    }
    return 0;
}
