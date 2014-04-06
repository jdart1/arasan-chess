// Copyright 2010, 2011, 2012, 2014 by Jon Dart. All Rights Reserved.
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

struct PosInfo {
    string fen;
    int result;
};

std::vector<PosInfo> fens;

static bool eval = false;

static int cores = 1;

static int plies = 1;

static const int MAX_THREADS = 64;

static const int THREAD_STACK_SIZE = 8*1024*1024;

// results from the threads
static double errors[MAX_THREADS];

struct ThreadData {
    SearchController *searcher;
    int index;
    THREAD thread_id;
} threadDatas[MAX_THREADS];

struct TuneHistory {
    int initialValue;
    int finalValue;
    double slope;
    bool slope_set;
    int lastAdjust;
};

#ifndef _WIN32
static sem_t semaphore;
static pthread_attr_t stackSizeAttrib;
#endif

double PARAM1 = 1.6;
double PARAM2 = 0.28;

static double sigmoid(double val) {
    double s = -PARAM1*(-0.5+1.0/(1.0+pow(10.0,PARAM2*val)));
    if (s < -1.0) return -1.0;
    if (s > 1.0) return 1.0;
    return s;
}

static int threadsActive = 0;
LockDefine(activeLock);

static double computeError(SearchController *searcher, int index) {
    double err = 0.0;
	// This is a large object so put it on the heap:
    Scoring *scoring = new Scoring(&(searcher->hashTable));
    size_t lines = 0;
    
    for (size_t i = index; i < fens.size(); i += cores) {
       ++lines;
        Board b;
        string fen(fens[i].fen);
        stringstream s(fen);
        s >> b;
        if (s.fail()) {
            cerr << "error on FEN string: " << fen << endl;
            continue;
        }
        int value;
        if (eval) {
            value = scoring->evalu8(b);
        } else {
            options.search.easy_plies = 0;
            options.learning.position_learning = 0;
            Statistics stats;
            searcher->findBestMove(b,
                FixedDepth,
                999999,
                0,
                plies, false, false,
                stats,
                Silent);
            value = stats.value;
        }
        double val = value/100.0;
        if (b.sideToMove() == Black) val = -val;
        double predict = sigmoid(val);
        err += ((double)fens[i].result - predict)*((double)fens[i].result - predict);
    }
    delete scoring;
//    cout << "thread " << index << " done" << endl;
    
    return err;

}

#ifdef _WIN32
static DWORD WINAPI threadp(void *x)
#else
static void * CDECL threadp(void *x)
#endif
{
    ThreadData *td = (ThreadData*)x;
    td->searcher = new SearchController();
    errors[td->index] = computeError(td->searcher,td->index);
    Lock(activeLock);
    threadsActive--;
    if (!threadsActive) {
       if (sem_post(&semaphore)) {
          perror("sem_post");
       }
    }
    Unlock(activeLock);
    return 0;
}

static double computeLsqError() {
    // prepare threads
   LockInit(activeLock);
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
   if (sem_init(&semaphore, 0, 0)) {
      perror("sem_init");
      return 0.0;
   }
#endif
   Lock(activeLock);
   threadsActive = cores;
   Unlock(activeLock);
   
    for (int i = 0; i < cores; i++) {
        THREAD thread_id;
        threadDatas[i].thread_id = thread_id;
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
        if (pthread_create(&thread_id, &stackSizeAttrib, threadp, (void*)(threadDatas+i))) {
            perror("thread creation failed");
        }
#endif
    }
    // Wait for threads to complete
#ifdef _WIN32
    HANDLE handles[MAX_THREADS];
    for (int i = 0; i<cores; i++) {
        handles[i] = threadDatas[i].thread_id;
    }
    WaitForMultipleObjects(cores,handles,TRUE,INFINITE);
#else
    if (sem_wait(&semaphore)) {
        perror("sem_wait");
    }
    sem_destroy(&semaphore);
    if (pthread_attr_destroy(&stackSizeAttrib)) {
       perror("pthread_attr_destroy");
    }
#endif        
    // total errors from the threads
    double total = 0;
    for (int i = 0; i < cores; i++) {
        total += errors[i];
    }
    cout << "result: " << setprecision(8) << total/fens.size() << endl;
    return total/fens.size();
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
         double quality = computeLsqError();
         x.set_bb_output  ( 0 , quality  ); // objective value
         x.set_bb_output  ( 1 , 0); // objective value
         x.set_bb_output  ( 2 , 0); // objective value

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

static void printResult(const string &solved, float runtime,
                        float runlength, double quality, int seed) 
{
   cout << "Result for ParamILS: " << solved << ", " <<
      runtime << ", " <<
      runlength << ", " << 
      quality << ", " <<
      seed << endl;
}


static void printAbort() 
{
   printResult("ABORT",0,0,0,0);
}


int CDECL main(int argc, char **argv)
{
    Bitboard::init();
    initOptions(argv[0]);
    Attacks::init();
    Scoring::init();
    options.search.hash_table_size = 64*1024*1024;
    if (!initGlobals(argv[0], false)) {
        cleanupGlobals();
        exit(-1);
    }
    atexit(cleanupGlobals);
    delayedInit();
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
        string paramFile = argv[1];
        string tuneFile = argv[2];

        try {
           
        NOMAD::Display out ( std::cout );
        out.precision ( NOMAD::DISPLAY_PRECISION_STD );

        NOMAD::begin ( argc-2 , argv+2 );

        NOMAD::RNG::set_seed(12345);

        NOMAD::Parameters p(out);
        cout << "reading parameters" << endl;
        p.read(paramFile);

        cout << "reading training file" << endl;
        ifstream fen_file(tuneFile.c_str(), ios::in);
        if (!fen_file.good()) {
           cerr << "could not open file " << tuneFile << endl;
           printAbort();
           exit(-1);
        }
        else {
           size_t lines = 0;
           while (!fen_file.eof() && !fen_file.fail()) {
              string line;
              getline(fen_file,line);
              size_t split = line.find_last_of(" ");
              if (split == string::npos) {
                 continue;
              } else {
                 PosInfo pos;
                 pos.fen = line.substr(0,split);
                 string result = line.substr(split+1);
                 pos.result = atoi(result.c_str());
                 ++lines;
                 fens.push_back(pos);
              }
           }
           fen_file.close();
           cout << "position file read: " << lines << " lines" << endl;
        }
        // parameters validation:
        p.check();

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
