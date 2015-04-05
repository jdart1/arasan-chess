// Copyright 2015 by Jon Dart. All Rights Reserved.
#ifndef _RSPSA_H
#define _RSPSA_H
#include "optbase.h"

class RSpsa : public OptBase {

 public:
    RSpsa(int d, const Eigen::VectorXd &x0, int eval_limit);

    virtual ~RSpsa();

    virtual void setBoxConstraints(const Eigen::VectorXd &lower,
                        const Eigen::VectorXd &upper);

    void setParameters(double delta0, double rho, double weight1,
                       double weight2, double delta_min, double delta_max) {
      options.delta0 = delta0;
      options.rho = rho;
      options.weight1 = weight1;
      options.weight2 = weight2;
      options.delta_min = delta_min;
      options.delta_max = delta_max;
    }

    virtual void setEvaluationLimit(int limit) {
      eval_limit = limit;
      iterations = limit/2;
    }
    virtual void optimize(double (*func)(const Eigen::VectorXd &theta),
                          void (*update)(double obj, const Eigen::VectorXd &theta));

 private:
    struct Options {
      double delta0;
      double rho; // delta scale/step_size
      double weight1, weight2; // 0 < weight1 < 1 < weight2
      double delta_min;
      double delta_max;
    } options;

    int iterations;
    Eigen::VectorXd lower, upper;
};
#endif
