#ifndef _ROCKSTAR_H
#define _ROCKSTAR_H

#include "optbase.h"
#include <Eigen/Core>

class Rockstar : public OptBase {

 public:

    Rockstar(int d, const Eigen::VectorXd &x0, int eval_limit);

    virtual ~Rockstar();

    void setParameters(double initialSd, int initial_exp) {
      options.initialSd = initialSd;
      options.initial_exp = initial_exp;
    }

    virtual void setBoxConstraints(const Eigen::VectorXd &lower,
                        const Eigen::VectorXd &upper);

    virtual void optimize(double (*func)(const Eigen::VectorXd &theta),
                         void (*update)(double obj, const Eigen::VectorXd &theta));

 private:

    struct Options {
      double initialSd;
      int initial_exp;
    } options;
    Eigen::VectorXd lower, upper;
};

#endif


