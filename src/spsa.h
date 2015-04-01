// Copyright 2015 by Jon Dart. All Rights Reserved.
#include <vector>
using namespace std;

class Spsa {

 public:
    Spsa(int dim);

    int optimize(const vector<double> &initial_theta, 
                 int iterations, 
                 double (*func)(const vector<double> &theta),
                 void (*update)(double obj, const vector<double> &theta));

    void getBest(double &obj, vector<double> &best);

 private:

    int dim;
    double best;
    vector<double> best_theta;
};
