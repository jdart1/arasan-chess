// Copyright 2015 by Jon Dart. All Rights Reserved

#include "optbase.h"

double OptBase::eval(const Eigen::VectorXd &theta,double (*func)(const Eigen::VectorXd &theta),void (*update)(double obj, const Eigen::VectorXd &best)) 
{
   double best = func(theta);
   update(best,theta);
   ++evals_done;
   if (evals_done >= eval_limit) terminate++;
   return best;
}

