// Copyright 2015 by Jon Dart. All Rights Reserved.
#include "rockstar.h"
#include "Rockstar.hpp"

// A wrapper around the Rock* algorithm, based on
// "Reward Optimization with compact kernels and natural gradient
// descent, 2015", Jemin Hwangbo

#define TRACE

Rockstar::Rockstar(int d, const Eigen::VectorXd &x0, int eval_limit) 
   : OptBase(d)
{
   setInitialPoints(x0);
   setEvaluationLimit(eval_limit);
   // defaults
   options.initial_exp = 2;
   options.initialSd = 0.05;
}

Rockstar::~Rockstar() 
{
}

void Rockstar::setBoxConstraints(const Eigen::VectorXd &lower,
                                 const Eigen::VectorXd &upper) 
{
   cerr << "box constraints not supported!" <<endl;
}

#ifdef TRACE
static void printArray(const string &name, const Eigen::VectorXd &x) 
{
   cout << name << " ( ";
   for (int i = 0; i < x.size(); i++ ) cout << x(i) << " ";
   cout << " )" << endl;
}
#endif

void Rockstar::optimize(double (*func)(const Eigen::VectorXd &theta),
                        void (*update)(double obj, const Eigen::VectorXd &theta)) 
{
   Eigen::VectorXd Initial_StandardDeviation = Eigen::VectorXd::Ones(dim,1) * options.initialSd;
   rockstar::Rockstar optimizer(initial_theta, Initial_StandardDeviation, options.initial_exp);

   Eigen::VectorXd theta(dim);
   theta = initial_theta;
   for(int i=0; i< eval_limit; i++){
      optimizer.getNextTheta2Evaluate(theta);
#ifdef TRACE
      printArray("newTheta",theta);
#endif      
      double cost = eval(theta,func,update);
      optimizer.setTheCostFromTheLastTheta(cost);
   }
}


