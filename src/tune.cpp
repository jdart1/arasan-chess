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
#include <sstream>
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

static int best = 1000;

std::vector<PosInfo> fens;

static int plies = 1;

static const char * MATCH_PATH="/home/jdart/tools/match.py";

static const int MAX_THREADS = 64;

static const int THREAD_STACK_SIZE = 8*1024*1024;

static int games=200;

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
   char buffer[2048];
   int result = -1;
   while(!feof(pipe)) {
      if(fgets(buffer, 2048, pipe) != NULL) {
         cout << buffer << endl;
         if (strlen(buffer)>7 && strncmp(buffer,"rating=",7)==0) {
            result = atoi(buffer+7);
         }
      }
   }
   pclose(pipe);
   if (result > best) best = result;
   return result;
}

static double computeLsqError() {

   stringstream s;
   s << MATCH_PATH;
   s << ' ';
/*
   s << "-best ";
   s << best;
   s << ' ';
*/
   s << games;
   for (int i = 0; i < Search::NUM_PARAMS; i++) {
      s << ' ' << Search::params[i].name << ' ';
      s << Search::params[i].current;
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
         for ( int i = 0 ; i < Search::NUM_PARAMS ; i++ ) 
         {
            Search::params[i].current = x[i].round();
         }
         Search::initParams();
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
        int arg = 1;
        if (string(argv[1]) == "-games") {
           stringstream s(argv[2]);
           s >> games;
           cout << "games=" << games << endl;
           arg += 2;
        }
        
        string paramFile = argv[arg++];
        //string tuneFile = argv[arg];

        try {
           
        NOMAD::Display out ( std::cout );
        out.precision ( NOMAD::DISPLAY_PRECISION_STD );

        NOMAD::begin ( argc-arg+1 , argv+arg-1 );

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
    return 0;
}
