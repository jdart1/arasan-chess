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

static int cores = 8;

static int plies = 1;

static const char * MATCH_PATH="/home/jdart/tools/match.py";

static const int MAX_THREADS = 64;

static const int THREAD_STACK_SIZE = 8*1024*1024;

static const int games=250;

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

const double PARAM1 = 2.0;
const double PARAM2 = 0.38;

static double sigmoid(double val) {
    double s = -PARAM1*(-0.5+1.0/(1.0+pow(10.0,PARAM2*val)));
    if (s < -1.0) return -1.0;
    if (s > 1.0) return 1.0;
    return s;
}

static int exec(const char* cmd) {
   
   cout << "executing " << cmd << endl;

   FILE* pipe = popen(cmd, "r");
   if (!pipe) {
      cerr << "perror failed" << endl;
      return -1;
   }
   char buffer[128];
   int result = -1;
   while(!feof(pipe)) {
      if(fgets(buffer, 128, pipe) != NULL) {
         cout << buffer << endl;
         if (strlen(buffer)>7 && strncmp(buffer,"rating=",7)==0) {
            result = atoi(buffer+7);
         }
      }
   }
   pclose(pipe);
   return result;
}


static double computeError(SearchController *searcher, int index) {
    double err = 0.0;
	// This is a large object so put it on the heap:
    Scoring *scoring = NULL;
    if (plies < 0) scoring = new Scoring();
    size_t lines = 0;
    
    for (size_t i = index; i < fens.size(); i += cores) {
       ++lines;
        Board b;
        string fen(fens[i].fen);
        stringstream s(fen);
        s >> b;
        if (s.fail()) {
//            cerr << "error on FEN string: " << fen << endl;
            continue;
        }
        int value;
        if (plies < 0) {
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
    if (plies < 0) delete scoring;
    cout << "thread " << index << " done" << endl;
    
    return err;

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

   stringstream s;
   s << MATCH_PATH;
   s << ' ';
   s << games;
   for (int i = 0; i < Scoring::NUM_PARAMS; i++) {
      s << ' ' << Scoring::params[i].name << ' ';
      s << Scoring::params[i].current;
   }
   s << '\0';
   string cmd = s.str();
   return double(5000-exec(cmd.c_str()));
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
//         cout << "quality= " << quality << endl;
         
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
    options.search.hash_table_size = 0;
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
        int arg = 1;
/*
        while (arg < argc && argv[arg][0] == '-') {
           if (strcmp(argv[arg],"-c")==0) {
              ++arg;
              cores = atoi(argv[arg]);
              ++arg;
           }
           else if (strcmp(argv[arg],"-p")==0) {
              ++arg;
              plies = atoi(argv[arg]);
              ++arg;
           }
        }
        
        for (int i = 0; i < cores; i++) {
           threadDatas[i].searcher = NULL;
        }
*/        
        string paramFile = argv[arg++];
        //string tuneFile = argv[arg];

        try {
           
        NOMAD::Display out ( std::cout );
        out.precision ( NOMAD::DISPLAY_PRECISION_STD );

        NOMAD::begin ( argc-2 , argv+2 );

        srand((unsigned)(getCurrentTime() % (1<<31)));
        NOMAD::RNG::set_seed(rand() % 12345);

        NOMAD::Parameters p(out);
        cout << "reading parameters" << endl;
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
/*

        cout << "reading training file" << endl;
        ifstream fen_file(tuneFile.c_str(), ios::in);
        if (!fen_file.good()) {
           cerr << "could not open file " << tuneFile << endl;
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
                 std::size_t last  = result.find_last_of(" \n");
                 if (last != string::npos) {
                    result = result.substr(0,last);
                 }
                 if (result == "0-1")
                    pos.result = -1;
                 else if (result == "1-0")
                    pos.result = 1;
                 else if (result == "1/2-1/2")
                    pos.result = 0;
                 else
                    continue;
                 ++lines;
                 fens.push_back(pos);
              }
           }
           fen_file.close();
           cout << "position file read: " << lines << " lines" << endl;
        }
        int err = 10000;
        for (double p = 0.1; p < 0.4; p+=0.02) {
           PARAM2 = p;
           double err2 = computeLsqError();
           if (err2 < err) {
              err = err;
           } else {
              break;
           }
        }
        cout << "optimal param2 = " << PARAM2 << endl;*/

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
