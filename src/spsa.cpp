extern "C" {
#include <math.h>
#include <stdlib.h>
#include <assert.h>
};
#include "spsa.h"
#include <iostream>

Spsa::Spsa(int d):
   dim(d) 
{
}

static int sgn(double val) 
{
   return val == 0.0 ? 0 : ((val > 0.0) ? 1 : 0);
}

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
    const double alpha = 0.602; // from Spall, 1998
    const double gamma = 0.101;
    const double A = float(iterations)/10;
    const double c = 0.05;
    const double a = 0.5;
    for (int i = 0; i < iterations; i++) {
       singleStep(i, theta, func, update, a, c, A, alpha, gamma);
    }
}

static void printArray(const string &name, vector<double> x) 
{
   cout << name << " ( ";
   for (int i = 0; i < x.size(); i++ ) cout << x[i] << " ";
   cout << " )" << endl;
}


void Spsa::singleStep(int iteration, vector<double> &theta, 
                      double (*func)(const vector<double> &theta),
                      void (*update)(double obj, const vector<double> &theta),
                      double a, double c, double A, double alpha, double gamma) {
    const double ak = a/pow(iteration + 1 + A,alpha);
    const double ck = c/pow(iteration + 1, gamma);
    const double tolerance_theta = 0.05;
    
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
    printArray("theta_plus",theta_plus);
    printArray("theta_minus",theta_minus);
    
    double eval_plus = func(theta_plus);
    update(eval_plus,theta_plus);
    double eval_minus = func(theta_minus);
    update(eval_minus,theta_minus);
    const double diff = eval_plus - eval_minus;
    cout << "new theta (";
    for (int i = 0; i < dim; i++) {
       double ghat = diff/( 2.0*ck*deltas[i] );
       double adj = ak*ghat;
       // restrict amount of theta change per step
       if (fabs(adj) > tolerance_theta) {
          adj = sgn(adj)*tolerance_theta;
       }
       theta[i] -= adj;
       // apply constraints
       if (theta[i] < 0.0) theta[i] = 0.0;
       if (theta[i] > 1.0) theta[i] = 1.0;
       cout << theta[i] << " ";
    }
    cout << " )" << endl;
    
    //double eval_new = func(theta);
    //update(eval_new,theta);
/*
    cout << "new theta: " << " (";
    for (int i = 0; i < dim; i++) {
      cout << theta[i] << ' ';
    }
    cout << " )" << endl;
*/
}

