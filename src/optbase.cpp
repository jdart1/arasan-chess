// Copyright 2015 by Jon Dart. All Rights Reserved

#include "optbase.h"

double OptBase::eval(const std::vector<double> &theta,double (*func)(const std::vector<double> &theta),void (*update)(double obj, const std::vector<double> &best)) 
{
   double best = func(theta);
   update(best,theta);
   ++evals_done;
   if (evals_done >= eval_limit) terminate++;
   return best;
}

