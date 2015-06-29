// Copyright 2015 by Jon Dart. All Rights Reserved.

#ifndef _TUNE_H
#define _TUNE_H

#include "scoring.h"

#include <string>

namespace tune {

  struct TuneParam {
    int index;
    string name;
    int current;
    int min_value;
    int max_value;
  TuneParam(int i, const string &n, int c, int minv, int maxv) :
    index(i),name(n),current(c),min_value(minv),max_value(maxv) {
    }
  };

  static const int NUM_TUNING_PARAMS = 141;

  extern TuneParam tune_params[NUM_TUNING_PARAMS];

  // initialize scoring parameters from the current tuned parameter
  // set
  extern void applyParams();

  extern void checkParams();

  extern void writeX0(ostream &);

  extern void readX0(istream &);

  // return index for parameter given name, -1 if not not found
  static int findParamByName(const string &name) {
    for (int i = 0; i < NUM_TUNING_PARAMS; i++) {
      if (tune_params[i].name == name) {
        return i;
      }
    }
    return -1;
  }

};

#endif
