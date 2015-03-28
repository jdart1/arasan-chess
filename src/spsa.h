
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
    void singleStep(int iteration, vector<double> &theta, 
                   double (*func)(const vector<double> &theta),
                   void (*update)(double obj, const vector<double> &theta),
                   double a, double c, double A, double alpha, double gamma);

    int dim;
    double best;
    vector<double> best_theta;
};
