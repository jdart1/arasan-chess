// Copyright 2015 by Jon Dart. All Rights Reserved.
#include "rockstar.h"
#include "Rockstar.hpp"

// A wrapper around the Rock* algorithm implementation by Jemin
// Hwangbo,
// based on: Jemin Hwangbo, Christian Gehring, Hannes Sommer,
// Roland Siegwart, Jonas Buchli
// "Reward Optimization with compact kernels and natural gradient
// descent", Proceedings 2014 IEEE-RAS International Conference on
// Humanoid Robots (2014).

//#define TRACE

Rockstar::Rockstar(int d, const std::vector<double> &x0, int eval_limit) 
   : OptBase(d)
{
   setInitialPoints(x0);
   setEvaluationLimit(eval_limit);
   // defaults
   options.initial_exp = 2;
   options.initialSd = 0.2; // was: 0.05
   options.constraint_penalty = 1000;
   // constraints are 0..1 by default
   lower.assign(dim,0.0);
   upper.assign(dim,1.0);
}

Rockstar::~Rockstar() 
{
}

void Rockstar::setBoxConstraints(const std::vector<double> &lower,
                                 const std::vector<double> &upper) 
{
   this->lower = lower;
   this->upper = upper;
}

#ifdef TRACE
static void printArray(const string &name, const Eigen::VectorXd &x) 
{
   cout << name << " ( ";
   for (int i = 0; i < x.size(); i++ ) cout << x(i) << " ";
   cout << " )" << endl;
}
#endif

void Rockstar::optimize(double (*func)(const std::vector<double> &theta),
                        void (*update)(double obj, const std::vector<double> &theta)) 
{
   Eigen::VectorXd Initial_StandardDeviation(dim);
   Initial_StandardDeviation.setOnes();
   Eigen::VectorXd theta(dim);
   // convert input vector to Eigen vector:
   for (int i=0;i<dim;i++) theta[i] = initial_theta[i];
   rockstar::Rockstar optimizer(theta, Initial_StandardDeviation, options.initial_exp);

   for(int i=0; i< eval_limit; i++){
      optimizer.getNextTheta2Evaluate(theta);
#ifdef TRACE
      printArray("newTheta",theta);
#endif      
      // convert back to std vector for eval
      std::vector<double> x(dim);
      for (int j = 0; j < dim; j++) x[j] = theta[j];
      double cost = eval(x,func,update);
      // apply penalty for constraint violations
      double penalty = 0.0;
      for (int i = 0; i < dim; i++) {
         if (theta[i] < lower[i]) {
            penalty += pow(lower[i]-theta[i],2.0)*options.constraint_penalty;
         } else if (theta[i] > upper[i]) {
            penalty += pow(theta[i]-upper[i],2.0)*options.constraint_penalty;
         }
      }
#ifdef TRACE
      cout << "constraint penalty=" << penalty << endl;
#endif
      cost += penalty;
      optimizer.setTheCostFromTheLastTheta(cost);
   }
}


