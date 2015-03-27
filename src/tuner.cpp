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
#include <unistd.h>
};

#define ROCKSTAR

#ifdef ROCKSTAR
#include "Rockstar.hpp"
#include <Eigen/Core>
static int min_cost = 5000;
#else
#include "cmaes.h"
using namespace libcmaes;
#endif

// script to run matches
static const char * MATCH_PATH="/home/jdart/tools/match.py";

static bool terminated = false;

static string out_file_name="params.cpp";

static string x0_file_name="x0";

static int first_index = 0;
 
static int last_index = tune::NUM_TUNING_PARAMS-1;

static int games = 1000;

static int best = 0;

static double scale(const tune::TuneParam &t) 
{
   return double(t.current-t.min_value)/double(t.max_value-t.min_value);
}

static int unscale(double val, const tune::TuneParam &t)
{
   return round(double(t.max_value-t.min_value)*val+(double)t.min_value);
}

// transform range 0..1 into log space
static double logXForm(double val) 
{
   if (val <= 0.0) val = 0.001;
   if (val >= 1.0) val = 0.999;
   return log(val/(1.0-val));
}

// transform out of log space
static double logUnXForm(double val) 
{
   return 1.0/(1+exp(-val));
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
   if (result > best) {
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
   }
   
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
   return double(5000-exec(cmd.c_str()));
}

#ifdef ROCKSTAR
static double evaluate(const Eigen::VectorXd &x, const int dim) 
{
   int i;
   for (i = 0; i < tune::NUM_TUNING_PARAMS; i++) 
   {
      tune::tune_params[i].current = unscale(logUnXForm(x(i)),tune::tune_params[i]);
   }
   tune::initParams();
   double err = computeLsqError();
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

#else

static FitFunc evaluator = [](const double *x, const int dim) 
{
   int i;
   for (i = first_index; i <= last_index; i++) 
   {
      tune::tune_params[i].current = unscale(x[i-first_index],tune::tune_params[i]);
   }
   tune::initParams();
   double err = computeLsqError();
   cout << "objective=" << err << endl;
   return err;
   
};

static ProgressFunc<CMAParameters<GenoPheno<pwqBoundStrategy>>,CMASolutions> progress = [](const CMAParameters<GenoPheno<pwqBoundStrategy>> &cmaparams, const CMASolutions &cmasols)
{
   
   std::cout << "best solution: " << cmasols << std::endl;
   vector <double> x0 = cmasols.best_candidate().get_x();
/*
   cout << "denormalized solution: " << endl;
   int i = 0;
        
   for (vector<double>::const_iterator it = x0.begin();
        it != x0.end();
        it++,i++) {
      cout << " " << tune_params[i].name << ": " << round(unscale(*it,i)) << endl;
   }
   cout << endl;
*/   
   return 0;
};

#endif

static void usage() 
{
   cerr << "Usage: tuner -i <input objective file>" << endl;
   cerr << "-o <output parameter file> -x <output objective file>" << endl;
   cerr << "-f <first_parameter_name> -s <last_parameter_name>" << endl;
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
   ofstream x0_out(x0_file_name,ios::out | ios::trunc);
   tune::writeX0(x0_out);

    int arg = 1;
    string first_param, last_param;
    
    while (arg < argc && argv[arg][0] == '-') {
       if (strcmp(argv[arg],"-i")==0) {
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
    
    int dim = last_index - first_index;
    if (dim<=0) {
       cerr << "Error: 2nd named parameter is before 1st!" << endl;
       exit(-1);
    }
    cout << "dimension = " << dim << endl;
    cout << "games per core per iteration = " << games << endl;
    
#ifdef ROCKSTAR
    Eigen::VectorXd x0 = Eigen::VectorXd::Zero(tune::NUM_TUNING_PARAMS,1);
    // scale and transform initial solution
    for (int i = 0; i < tune::NUM_TUNING_PARAMS; i++) {
       x0(i) = logXForm(scale(tune::tune_params[i]));
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
#else
    vector<double> x0;
    double sigma = 0.05;
    // use variant with box bounds
    double lbounds[tune::NUM_TUNING_PARAMS],
       ubounds[tune::NUM_TUNING_PARAMS];
    // initialize & normalize
    for (int i = first_index; i <= last_index; i++) {
       x0.push_back(scale(tune::tune_params[i]));
       lbounds[i-first_index] = 0.0;
       ubounds[i-first_index] = 1.0;
    }
    GenoPheno<pwqBoundStrategy> gp(lbounds,ubounds,dim);
    CMAParameters<GenoPheno<pwqBoundStrategy>> cmaparams(dim,&x0.front(),sigma,-1,0,gp);
    cmaparams.set_algo(sepaBIPOP_CMAES);
    CMASolutions cmasols = cmaes<GenoPheno<pwqBoundStrategy>>(evaluator,cmaparams,progress);
#endif

    return 0;
}
