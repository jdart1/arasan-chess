// Copyright 2015 by Jon Dart. All Rights Reserved.
extern "C" {
#include <math.h>
#include <stdlib.h>
#include <assert.h>
};
#include "rspsa.h"
#include <iostream>

// Implements the enhanced SPSA with RPROP described in
// Levente Kocsis, Csaba Szepesv ́and Mark H.M. Winands,
// "RSPSA: Enhanced Parameter Optimization in Games"
// ACG'05 Proceedings of the 11th international conference on Advances
// in Computer Games (2006), pp. 39-56
//#define TRACE
#define MONITOR_THETA

RSpsa::RSpsa(int d, const Eigen::VectorXd &x0, int eval_limit):
   OptBase(d) 
{
   setEvaluationLimit(eval_limit);
   
   setInitialPoints(x0);
   
   // The following 2 values are suggested in the RSPSA paper:
   options.weight1 = 0.7;
   options.weight2 = 1.2;

   options.delta0 = 0.1;
   options.rho = 1.0;
   options.delta_min = 0.005;
   options.delta_max = 0.1;
   // We currently assume values are scaled to the unit hypercube,
   // by default
   lower.resize(dim);
   lower.fill(0.0);
   upper.resize(dim);
   upper.fill(1.0);
}

RSpsa::~RSpsa() 
{
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

void RSpsa::setBoxConstraints(const Eigen::VectorXd &lower,
                              const Eigen::VectorXd &upper) 
{
   this->lower = lower;
   this->upper = upper;
}

void RSpsa::optimize(
                   double (*func)(const Eigen::VectorXd &theta),
                   void (*update)(double obj, const Eigen::VectorXd &theta))
{
   Eigen::VectorXd theta = initial_theta;
#ifdef MONITOR_THETA
   (void)eval(initial_theta,func,update);
#endif
   Eigen::VectorXd previous_gsgn(dim);
   Eigen::VectorXd theta_plus(dim);
   Eigen::VectorXd theta_minus(dim);
   // step sizes (small delta in paper):
   Eigen::VectorXd deltas(dim);
   Eigen::VectorXi delta_dirs(dim);
   deltas.fill(options.delta0);
#ifdef TRACE
   printArray("deltas before loop",deltas);
#endif   
   for (int iteration = 0; iteration < iterations; iteration++) {
      for (int i = 0; i < dim; i++) {
         // compute random deltas for each dimension, scaled to the
         // step size
         const int delta_dir = 2*round((float)rand()/(float)RAND_MAX) - 1;
         assert(delta_dir);
         delta_dirs(i) = delta_dir;
         double big_delta = delta_dir*options.rho*deltas(i);
#ifdef TRACE
         cout << " big delta=" << big_delta << endl;
#endif
         theta_plus(i) = theta(i) + big_delta;
         theta_minus(i) = theta(i) - big_delta;
      }
#ifdef MONITOR_THETA
      // Note: we generally do not have an evaluation of the
      // objective directly at theta during execution. Periodically
      // do this, so we can evaluate progress and store the best
      // theta.
      if ((iteration+1) % 10 == 0) {
         //cout << "special eval pass using current theta" << endl;
         (void)eval(theta,func,update);
      }
#endif
      if (terminate || evals_done+2 > eval_limit) break;

      double eval_plus = eval(theta_plus,func,update);
      double eval_minus = eval(theta_minus,func,update);
      const double diff = eval_plus - eval_minus;
      // sign of gradient estimate:
      for (int i = 0; i < dim; i++) {
         double gsgn = sgn(diff*delta_dirs[i]);
         double delta = options.delta0;
         if (iteration > 0) {
#ifdef TRACE
            cout << " delta was=" << deltas(i) << endl;
#endif
            // we have the previous gradient estimate, check
            // for sign change
            const int x = sgn(previous_gsgn(i)*gsgn);
            if (x > 0) {
               // adjust step size up:
               delta = options.weight2*deltas(i);
               if (delta > options.delta_max) {
                  delta = options.delta_max;
               }
            }
            else if (x < 0) {
               // gradients point in opposite directions, so
               // adjust step size down.
               delta = options.weight1*deltas(i);
               if (delta < options.delta_min) {
                  delta = options.delta_min;
               }
               gsgn = 0.0;
            }
            else {
               // do not adjust step size
               delta = deltas(i);
            }
         }
         // store abs value for next iteration
         deltas(i) = delta;
#ifdef TRACE
         cout << " small delta=" << delta << endl;
#endif
         // adjust theta by computed step size in opposite direction
         // of gradient (because we are minimizing).
         theta(i) -= gsgn*delta;
         // apply constraints
         if (theta(i) < lower(i)) theta(i) = lower(i);
         if (theta(i) > upper(i)) theta(i) = upper(i);
         previous_gsgn(i) = gsgn;
      }
#ifdef TRACE
      printArray("new theta",theta);
#endif
   }
}

