// Copyright 2015 by Jon Dart. All Rights Reserved
#ifndef _OPTBASE_H
#define _OPTBASE_H

#include <Eigen/Dense>
using namespace std;
extern "C" {
#include <limits.h>
};
#include <vector>

// base class for black-box optimizers for real-valued functions
class OptBase {

 public:
  OptBase(int d) :
  dim(d),eval_limit(INT_MAX),evals_done(0),terminate(0) {
    initial_theta.assign(dim,0.0);
  }

  virtual ~OptBase() {
  }

  virtual void setInitialPoints(const std::vector<double> &x) {
    initial_theta = x;
  }

  virtual void setBoxConstraints(const std::vector<double> &lower,
                                 const std::vector<double> &upper) = 0;

  virtual void setEvaluationLimit(int limit) {
    eval_limit = limit;
  }

  virtual void optimize(double (*func)(const std::vector<double> &theta),
        void (*update)(double obj, const std::vector<double> &best)) = 0;

 protected:

  // helper function that checks termination
  double eval(const std::vector<double> &theta,double (*func)(const std::vector<double> &theta),void (*update)(double obj, const std::vector<double> &best));

  int dim;
  int eval_limit;
  int evals_done;
  int terminate;
  std::vector<double> initial_theta;
};

#endif
