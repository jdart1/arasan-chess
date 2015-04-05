// Copyright 2015 by Jon Dart. All Rights Reserved
#ifndef _OPTBASE_H
#define _OPTBASE_H

#include <Eigen/Dense>
using namespace std;
extern "C" {
#include <limits.h>
};

// base class for black-box optimizers for real-valued functions
class OptBase {

 public:
  OptBase(int d) :
  dim(d),eval_limit(INT_MAX),evals_done(0),terminate(0) {
   initial_theta.Zero(dim);
  }

  virtual ~OptBase() {
  }

  virtual void setInitialPoints(const Eigen::VectorXd &x) {
    initial_theta = x;
  }

  virtual void setBoxConstraints(const Eigen::VectorXd &lower,
                                 const Eigen::VectorXd &upper) = 0;

  virtual void setEvaluationLimit(int limit) {
    eval_limit = limit;
  }

  virtual void optimize(double (*func)(const Eigen::VectorXd &theta),
        void (*update)(double obj, const Eigen::VectorXd &best)) = 0;

 protected:

  // helper function that checks termination
  double eval(const Eigen::VectorXd &theta,double (*func)(const Eigen::VectorXd &theta),void (*update)(double obj, const Eigen::VectorXd &best));

  int dim;
  int eval_limit;
  int evals_done;
  int terminate;
  Eigen::VectorXd initial_theta;

};

#endif
