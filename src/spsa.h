// Copyright 2015 by Jon Dart. All Rights Reserved.
#ifndef _SPSA_H
#define _SPSA_H
#include "optbase.h"

class Spsa : public OptBase {

 public:

    // Note: SPSA requires an evaluation budget to be set. Each iteration
    // requires two function evals.
    Spsa(int d, const Eigen::VectorXd &x0, int eval_limit);

    virtual ~Spsa();

    // defaults are used if not set
    void setParameters(double alpha, double gamma, double A, double c,
                       double a,
                       double delta_min, double delta_max);

    virtual void setBoxConstraints(const Eigen::VectorXd &lower,
                        const Eigen::VectorXd &upper);

    virtual void optimize(double (*func)(const Eigen::VectorXd &theta),
                         void (*update)(double obj, const Eigen::VectorXd &theta));

 private:
    struct Options {
      double alpha;
      double gamma;
      double A;
      double c;
      double a;
      double delta_min;
      double delta_max;
    } options;

    int iterations;
    Eigen::VectorXd lower, upper;
};
#endif
