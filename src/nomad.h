#ifndef _NOMAD_H
#define _NOMAD_H

#include "optbase.h"

class Nomad : public OptBase {

 public:

    Nomad(int d, const std::vector<double> &x0, int eval_limit);

    virtual ~Nomad();

    //    void setParameters(double initialSd, int initial_exp) {
    //    }

    virtual void setBoxConstraints(const std::vector<double> &lower,
                        const std::vector<double> &upper);

    virtual void optimize(double (*func)(const std::vector<double> &theta),
                         void (*update)(double obj, const std::vector<double> &theta));

 private:

    std::vector<double> lower, upper;
};

#endif


