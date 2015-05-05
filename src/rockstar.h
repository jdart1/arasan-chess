#ifndef _ROCKSTAR_H
#define _ROCKSTAR_H

#include "optbase.h"

class Rockstar : public OptBase {

 public:

    Rockstar(int d, const std::vector<double> &x0, int eval_limit);

    virtual ~Rockstar();

    void setParameters(double initialSd, int initial_exp) {
      options.initialSd = initialSd;
      options.initial_exp = initial_exp;
    }

    virtual void setBoxConstraints(const std::vector<double> &lower,
                        const std::vector<double> &upper);

    virtual void optimize(double (*func)(const std::vector<double> &theta),
                         void (*update)(double obj, const std::vector<double> &theta));

 private:

    struct Options {
      double initialSd;
      int initial_exp;
      double constraint_penalty;
    } options;
    std::vector<double> lower, upper;
};

#endif


