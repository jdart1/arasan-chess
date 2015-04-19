#ifndef _NOMAD_H
#define _NOMAD_H

#include "optbase.h"
#include <Eigen/Core>

class Nomad : public OptBase {

 public:

    Nomad(int d, const Eigen::VectorXd &x0, int eval_limit);

    virtual ~Nomad();

    //    void setParameters(double initialSd, int initial_exp) {
    //    }

    virtual void setBoxConstraints(const Eigen::VectorXd &lower,
                        const Eigen::VectorXd &upper);

    virtual void optimize(double (*func)(const Eigen::VectorXd &theta),
                         void (*update)(double obj, const Eigen::VectorXd &theta));

 private:

    /*
    struct Options {
      double initialSd;
      int initial_exp;
      double constraint_penalty;
      } options;*/
    Eigen::VectorXd lower, upper;
};

#endif


