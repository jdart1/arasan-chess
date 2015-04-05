// Copyright 2015 by Jon Dart. All Rights Reserved.
extern "C" {
#include <math.h>
#include <stdlib.h>
#include <assert.h>
};
#include "spsa.h"
#include <iostream>
#include <stdexcept>

//#define TRACE
#define MONITOR_THETA

// Implementation based on J. Spall, "An Overview of the
// Simultaneous Perturbation Method for Efficient Optimization",
// Johns Hopkins APL Technical Digest 19 (1998), pp 482-492.

Spsa::Spsa(int d, const Eigen::VectorXd &x0, int eval_limit): OptBase(d)
{
   // The following two default constants are from Spall, 1998:
   options.alpha = 0.602;
   options.gamma = 0.101;
   options.c = 0.1; //0.05;
   // The magnitude of the objective is much greater than theta (which
   // is scaled 0..1), so this constant must be small:
   options.a = 0.002;
   options.delta_min = 0.005;
   options.delta_max = 0.1;

   setEvaluationLimit(eval_limit);
   iterations = eval_limit/2;
   options.A = double(iterations)/10;

   setInitialPoints(x0);

   // We currently assume values are scaled to the unit hypercube,
   // by default
   lower.resize(dim);
   lower.fill(0.0);
   upper.resize(dim);
   upper.fill(1.0);
}

Spsa::~Spsa() 
{
}

void Spsa::setParameters(double alpha, double gamma, double A, double c,
                         double a,
                         double delta_min, double delta_max) 
{
   options.alpha = alpha;
   options.gamma = gamma;
   options.A = A;
   options.c = c;
   options.a = a;
   options.delta_min = delta_min;
   options.delta_max = delta_max;
}

static int sgn(double val) 
{
   return val == 0.0 ? 0 : ((val > 0.0) ? 1 : -1);
}

#ifdef TRACE
static void printArray(const string &name, const Eigen::VectorXd &x) 
{
   cout << name << " ( ";
   for (int i = 0; i < x.size(); i++ ) cout << x(i) << " ";
   cout << " )" << endl;
}
#endif

void Spsa::setBoxConstraints(const Eigen::VectorXd &lower,
                             const Eigen::VectorXd &upper) 
{
   this->lower = lower;
   this->upper = upper;
}

void Spsa::optimize(
                   double (*func)(const Eigen::VectorXd &theta),
                   void (*update)(double obj, const Eigen::VectorXd &theta))
{
   Eigen::VectorXd theta = initial_theta;
#ifdef MONITOR_THETA
   double best = eval(initial_theta,func,update);
#endif
   
   Eigen::VectorXd theta_plus(dim);
   Eigen::VectorXd theta_minus(dim);
   Eigen::VectorXi deltas(dim);
   for (int iteration = 0; iteration < iterations; iteration++) {
      const double ak = options.a/pow(iteration + 1 + options.A,options.alpha);
      const double ck = options.c/pow(iteration + 1, options.gamma);
      for (int i = 0; i < dim; i++) {
         const int delta = 2*round((float)rand()/(float)RAND_MAX) - 1;
         assert(delta);
         deltas(i) = delta;
         theta_plus(i) = theta(i) + ck*delta;
         theta_minus(i) = theta(i) - ck*delta;
      }
#ifdef TRACE
      printArray("theta_minus",theta_minus);
      printArray("theta_plus",theta_plus);
#endif    
#ifdef MONITOR_THETA
      // Note: we generally do not have an evaluation of the
      // objective directly at theta during execution. Periodically
      // do this, so we can evaluate progress and store the best
      // theta.
      if ((iteration+1) % 10 == 0) {
         //cout << "special eval pass using current theta" << endl;
         (void)eval(initial_theta,func,update);
      }
#endif
      if (terminate || evals_done+2 > eval_limit) break;
      double eval_plus = eval(theta_plus,func,update);
      double eval_minus = eval(theta_minus,func,update);
#ifdef TRACE
      cout << "eval_plus=" << eval_plus << " eval_minus=" << eval_minus << endl;
#endif
      const double diff = eval_plus - eval_minus;
      for (int i = 0; i < dim; i++) {
         // estimate of the gradient:
         double ghat = diff/( 2.0*ck*deltas(i) );
         double delta;
         delta = fabs(ak*ghat);
         // restrict amount of theta change per step.
         if (delta > options.delta_max) {
            delta = options.delta_max;
         }
         theta(i) -= sgn(ghat)*delta;
         // apply constraints
         if (theta(i) < lower(i)) theta(i) = lower(i);
         if (theta(i) > upper(i)) theta(i) = upper(i);
      }
#ifdef TRACE
      printArray("new theta",theta);
#endif
   }
}

