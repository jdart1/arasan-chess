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
#include "spsa.h"
#include "rspsa.h"
#include "hill.h"
#include "rockstar.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
extern "C" {
#include <math.h>
#include <ctype.h>
#include <unistd.h>
};

static int iterations = 40;

static int test = 0;

string algorithm = "Rockstar";

// script to run matches
static const char * MATCH_PATH="/home/jdart/tools/match.py";

static bool terminated = false;

static string out_file_name="params.cpp";

static string x0_file_name="x0";

static int first_index = 0;

static int last_index = tune::NUM_TUNING_PARAMS-1;

static int games = 1000;

static const int NUM_SUPPORTED_ALGORITHMS = 5;

static const int TEST_DIMENSIONS = 10;

enum Algorithm { SPSA, RSPSA, ROCKSTAR, NOMAD, UNKNOWN };

static const string supported_algorithms[NUM_SUPPORTED_ALGORITHMS] = {
   string("Spsa"),
   string("RSpsa"),
   string("Rockstar"),
   string("Nomad")
};

static Algorithm find_algo(const string &alg) 
{
   for (int i = 0; i < NUM_SUPPORTED_ALGORITHMS; i++) {
      if (alg == supported_algorithms[i]) {
         return (Algorithm)i;
       }
   }
   return UNKNOWN;
}


// best objective so far
static double best = 10000.0;

static double scale(const tune::TuneParam &t)
{
   return double(t.current-t.min_value)/double(t.max_value-t.min_value);
}

static int unscale(double val, const tune::TuneParam &t)
{
   return round(double(t.max_value-t.min_value)*val+(double)t.min_value);
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
   for (int i = first_index; i <= last_index; i++) {
      s << ' ' << tune::tune_params[i].name << ' ';
      s << tune::tune_params[i].current;
   }
   s << '\0';
   string cmd = s.str();
   // Call external script to run the matches and return a rating.
   // Optimizer minimizes, so optimize 5000-rating.
   double obj = double(5000-exec(cmd.c_str()));
   if (obj < best) {
      best = obj;
      if (x0_file_name.length()) {
         ofstream x0_out(x0_file_name,ios::out | ios::trunc);
         tune::writeX0(x0_out);
      } else {
         tune::writeX0(cout);
      }
      // apply current tune_params to Scoring module:
      tune::applyParams();
      if (out_file_name.length()) {
         ofstream param_out(out_file_name,ios::out | ios::trunc);
         Scoring::Params::write(param_out);
         param_out << endl;
      } else {
         Scoring::Params::write(cout);
         cout << endl;
      }
   }
   return obj;
}

static double update_best(const std::vector<double> &x, double obj) 
{
/*
   for (int i = 0; i < x.size(); i++) {
      if (x[i] < 0.0 || x[i] > 1.0) {
         cerr << "warning: x is infeasible" << endl;
      }
   }
*/
   if (obj < best) {
      best = obj;
   }
}

static double test_func1(const std::vector<double> &x)
{
   // simple parabolic function, minimum value 1800
   double sum = 0;
   for (int i = 0; i < x.size(); i++) {
      double factor = 0.8;
      if (i % 2 == 0) factor = 1.1;
      sum += factor*pow(x[i],2.0);
   }
   double eval = 200.0*sqrt(sum)+1800.0;
   update_best(x,eval);
   return eval;
}

static double test_func2(const std::vector<double> &x)
{
   // Somewhat harder function, minimum value about 2164.5
   double sum = 0;
   for (int i = 0; i < x.size(); i++) {
      double factor = 0.8;
      if (i % 2 == 0) factor = 1.1;
      sum += factor*pow(x[i],2.0);
      if (i % 3 == 0) {
         double adj = 50.0*exp(-pow((x[i]-0.5),2.0)/0.02)/0.2506;
         sum += adj;
      }
   }
   double eval = 200.0*sqrt(sum)+1800.0;
   update_best(x,eval);
   return eval;
}

static double rosen(const std::vector<double> &x) {
   // Rosenbrock function
   double sum = 0.0;
   for (int i = 0; i < TEST_DIMENSIONS-1; i++) {
     double a=x[i]*x[i]-x[i+1];
     double b=1.-x[i];
     sum+=(100.*a*a)+(b*b);      
   }
   update_best(x,sum);
   return sum;
}

static double evaluate(const std::vector<double> &x) {
   for (int i = first_index; i <= last_index; i++)
   {
      tune::tune_params[i].current = round(unscale(x[i-first_index],tune::tune_params[i]));
   }
   tune::applyParams();
   double err = computeLsqError();
   return err;
}

void update(double obj, const std::vector<double> &x)
{
//   cout << "objective=" << obj << endl;
}


static OptBase * allocate_optimizer(const string &algorithm,
                                    int dim,
                                    const std::vector<double> &x0,
                                    int eval_limit)
{
   OptBase *s = NULL;
   Algorithm alg = find_algo(algorithm);
   switch (alg) {
   case SPSA:
      s = (OptBase*)(new Spsa(dim,x0,eval_limit)); break;
   case RSPSA:
      s = (OptBase*)(new RSpsa(dim,x0,eval_limit)); break;
   case ROCKSTAR:      
      s = (OptBase*)(new Rockstar(dim,x0,eval_limit)); break;
   case NOMAD:      
      s = (OptBase*)(new Rockstar(dim,x0,eval_limit)); break;
   case UNKNOWN:
      cerr << "unknown algorithm: " << algorithm << endl;
   }
   if (s == NULL) {
      cerr << "optimizer allocation failed!" << endl;
      exit(-1);
   }
   return s;
}


static void usage()
{
   cerr << "Usage: tuner -i <input objective file> -a <algorithm>" << endl;
   cerr << "-o <output parameter file> -x <output objective file>" << endl;
   cerr << "-f <first_parameter_name> -s <last_parameter_name>" << endl;
   cerr << "-n <iterations> -t <for test>" << endl;
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
   tune::applyParams();
   ofstream param_out(out_file_name,ios::out | ios::trunc);
   Scoring::Params::write(param_out);
   param_out << endl;
   ofstream x0_out(x0_file_name,ios::out | ios::trunc);
   tune::writeX0(x0_out);

    int arg = 1;
    string first_param, last_param;

    while (arg < argc && argv[arg][0] == '-') {
       if (strcmp(argv[arg],"-a")==0) {
          ++arg;
          algorithm = argv[arg];
          if (find_algo(algorithm) == UNKNOWN) {
             cerr << "unknown algorithm: " << algorithm << endl;
             exit(-1);
          }
       }
       else if (strcmp(argv[arg],"-i")==0) {
          ++arg;
          input_file = argv[arg];
       }
       else if (strcmp(argv[arg],"-g")==0) {
          ++arg;
          games = atoi(argv[arg]);
       }
       else if (strcmp(argv[arg],"-o")==0) {
          ++arg;
          out_file_name = argv[arg];
       }
       else if (strcmp(argv[arg],"-f")==0) {
          ++arg;
          first_param = argv[arg];
       }
       else if (strcmp(argv[arg],"-s")==0) {
          ++arg;
          last_param = argv[arg];
       }
       else if (strcmp(argv[arg],"-t")==0) {
          ++test;
       }
       else if (strcmp(argv[arg],"-x")==0) {
          ++arg;
          x0_file_name = argv[arg];
       }
       else if (strcmp(argv[arg],"-n")==0) {
          ++arg;
          iterations = atoi(argv[arg]);
       } else {
          cerr << "invalid option: " << argv[arg] << endl;
          usage();
          exit(-1);
       }
       ++arg;
    }

    if (first_param.length()) {
       first_index = tune::findParamByName(first_param);
       if (first_index == -1) {
          cerr << "Error: Parameter named " << first_param << " not found." << endl;
          exit(-1);
       }
    }
    if (last_param.length()) {
       last_index = tune::findParamByName(last_param);
       if (last_index == -1) {
          cerr << "Error: Parameter named " << last_param << " not found." << endl;
          exit(-1);
       }
    }

    const int dim = last_index - first_index + 1;
    if (dim<=0) {
       cerr << "Error: 2nd named parameter is before 1st!" << endl;
       exit(-1);
    }
    cout << "dimension = " << dim << endl;
    cout << "games per core per iteration = " << games << endl;


    if (test) {
       int num_iters[3] = { 50, 250, 1250 };
       static const char *functions[3] =
       {"func1", "func2", "Rosenbrock"
       };
       for (int func = 0; func < 3; func++) {
          for (int n = 0; n < 3; n++) {
             double avg = 0.0;
             int max_tries = 5;
             for (int tries = 0; tries < max_tries; tries++) {
                std::vector<double> x0(TEST_DIMENSIONS);
                x0.assign(TEST_DIMENSIONS,0.8);
                best = 1e10;
                OptBase *s = allocate_optimizer(algorithm,TEST_DIMENSIONS,x0,num_iters[n]);
                switch (func) {
                case 0:
                   s->optimize(test_func1,update); break;
                case 1:
                   s->optimize(test_func2,update); break;
                case 2:
                   s->optimize(rosen,update); break;
                default: break;
                }
                delete s;
                cout << "try " << tries+1 << " best=" << best << endl;
                avg += best;
             }
             cout << algorithm << "/" << functions[func] << " average: (" << num_iters[n] << " evals): " << avg/max_tries << endl;
          }
       }
    }
    else {
       std::vector<double> x0(dim);
       // initialize and scale
       for (int i = 0; i < dim; i++) {
          x0[i] = scale(tune::tune_params[i+first_index]);
       }
       OptBase *s = allocate_optimizer(algorithm,dim,x0,iterations);
       s->optimize(evaluate,update);
       delete s;
    }

    return 0;
}
