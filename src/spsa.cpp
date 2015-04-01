// Copyright 2015 by Jon Dart. All Rights Reserved.
extern "C" {
#include <math.h>
#include <stdlib.h>
#include <assert.h>
};
#include "spsa.h"
#include <iostream>

// Define to apply the enhanced algorithm described in
// Levente Kocsis, Csaba Szepesv ́and Mark H.M. Winands,
// "RSPSA: Enhanced Parameter Optimization in Games"
// ACG'05 Proceedings of the 11th international conference on Advances
// in Computer Games (2006), pp, 39-56
#define RSPSA
//#define TRACE
#define MONITOR_THETA

Spsa::Spsa(int d):
   dim(d) 
{
}

static int sgn(double val) 
{
   return val == 0.0 ? 0 : ((val > 0.0) ? 1 : -1);
}

#ifdef TRACE
static void printArray(const string &name, vector<double> x) 
{
   cout << name << " ( ";
   for (int i = 0; i < x.size(); i++ ) cout << x[i] << " ";
   cout << " )" << endl;
}
#endif

int Spsa::optimize(const vector<double> &initial_theta, 
                   int iterations, 
                   double (*func)(const vector<double> &theta),
                   void (*update)(double obj, const vector<double> &theta))
{
   vector<double> theta;
   for (int i = 0; i < dim; i++) {
      theta.push_back(initial_theta[i]);
   }
   best = func(initial_theta);
   // The following 2 constants are from J. Spall, "An Overview of the
   // Simultaneous Perturbation Method for Efficient Optimization",
   // Johns Hopkins APL Technical Digest 19 (1998), pp 482-492.
   const double alpha = 0.602;
   const double gamma = 0.101;
   const double A = double(iterations)/10;
   const double c = 0.05;
   const double delta_max = 0.05;
   const double delta_min = 0.005;
   // The magnitude of the objective is much greater than theta (which
   // is scaled 0..1), so this constant must be small:
   const double a = 0.0003;
#ifdef RSPSA
   vector<double> current_ghat;
   vector<double> previous_ghat;
   vector<double> current_delta;
   vector<double> previous_delta;
   const double weight1 = 0.7;
   const double weight2 = 1.2;
#endif
   for (int iteration = 0; iteration < iterations; iteration++) {
#ifdef RSPSA
      current_ghat.clear();
      current_delta.clear();
#endif
      const double ak = a/pow(iteration + 1 + A,alpha);
      const double ck = c/pow(iteration + 1, gamma);
    
      vector<double> theta_plus;
      vector<double> theta_minus;
      vector<int> deltas;
      for (int i = 0; i < dim; i++) {
         const int delta = 2*round((float)rand()/(float)RAND_MAX) - 1;
         assert(delta);
         deltas.push_back(delta);
         theta_plus.push_back(theta[i] + ck*delta);
         theta_minus.push_back(theta[i] - ck*delta);
      }
#ifdef TRACE
      printArray("theta_plus",theta_plus);
      printArray("theta_minus",theta_minus);
#endif    
#ifdef MONITOR_THETA
      // Note: we generally do not have an evaluation of the
      // objective directly at theta during execution. Periodically
      // do this, so we can evaluate progress and store the best
      // theta.
      if ((iteration+1) % 10 == 0) {
         cout << "special eval pass using current theta" << endl;
         double eval = func(theta);
         update(eval,theta);
      }
#endif
      double eval_plus = func(theta_plus);
      update(eval_plus,theta_plus);
      double eval_minus = func(theta_minus);
      update(eval_minus,theta_minus);
      const double diff = eval_plus - eval_minus;
      for (int i = 0; i < dim; i++) {
         // estimate of the gradient:
         double ghat = diff/( 2.0*ck*deltas[i] );
         double delta;
#ifdef RSPSA
         if (iteration > 0) {
            // we have the previous gradient estimate
            const int x = sgn(previous_ghat[i]*ghat);
            if (x > 0) {
               // adjust step size up:
               delta = weight2*previous_delta[i];
               if (delta > delta_max) {
                  delta = delta_max;
               }
            }
            else if (x < 0) {
               // gradients point in opposite directions, so
               // adjust step size down.
               delta = weight1*previous_delta[i];
               if (delta < delta_min) {
                  delta = delta_min;
               }
               ghat = 0.0;
            }
            else {
               // do not adjust step size
               delta = previous_delta[i];
            }
            // store abs value
            current_delta.push_back(delta);
         } else {
            // use SPSA method for first iteration
            delta = fabs(ak*ghat);
            if (delta > delta_max) {
               delta = delta_max;
            }
         }
         current_delta.push_back(delta);
         current_ghat.push_back(ghat);
#else
         delta = fabs(ak*ghat);
         // restrict amount of theta change per step.
         if (delta > delta_max) {
            delta = delta_max;
         }
#endif
         theta[i] -= sgn(ghat)*delta;
         // apply constraints
         if (theta[i] < 0.0) theta[i] = 0.0;
         if (theta[i] > 1.0) theta[i] = 1.0;
      }
#ifdef TRACE
      printArray("new theta",theta);
#endif
#ifdef RSPSA
      previous_ghat = current_ghat;
      previous_delta = current_delta;
#endif
   }
}

