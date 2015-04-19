// Based on example code packaged with Nomad
#include "nomad.h"
#include <nomad.hpp>
using namespace std;

Nomad::Nomad(int d, const Eigen::VectorXd &x0, int eval_limit):
   OptBase(d) 
{
   setEvaluationLimit(eval_limit);
   
   setInitialPoints(x0);
   // We currently assume values are scaled to the unit hypercube,
   // by default
   lower.resize(dim);
   lower.fill(0.0);
   upper.resize(dim);
   upper.fill(1.0);
}

Nomad::~Nomad() 
{
}

static int local_dim;

static double (*local_func)(const Eigen::VectorXd &theta);

class My_Evaluator : public NOMAD::Evaluator {
public:
   My_Evaluator  ( const NOMAD::Parameters & p ) :
      NOMAD::Evaluator ( p ) {}

   ~My_Evaluator ( void ) {}

   bool eval_x ( NOMAD::Eval_Point   & x          ,
                 const NOMAD::Double & h_max      ,
                 bool                & count_eval   ) const
      {

         // convert to Eigen vector
         Eigen::VectorXd ex(local_dim);
         for (int i = 0; i < local_dim; i++) {
            ex[i] = x[i].value();
         }
         // call our function thru a pointer
         double eval = local_func(ex);

         x.set_bb_output  ( 0 , eval  ); // objective value

         count_eval = true; // count a black-box evaluation

         return true;       // the evaluation succeeded
      }
	
};

void Nomad::setBoxConstraints(const Eigen::VectorXd &lower,
                              const Eigen::VectorXd &upper) 
{
   this->lower = lower;
   this->upper = upper;
}

void Nomad::optimize(double (*func)(const Eigen::VectorXd &theta),
                     void (*update)(double obj, const Eigen::VectorXd &theta))
{
  // display:
  NOMAD::Display out ( std::cout );
  out.precision ( NOMAD::DISPLAY_PRECISION_STD );

  try {

    char **argv;
    int argc = 0;
     
    local_dim = dim;
    local_func = func;

    // NOMAD initializations:
    NOMAD::begin ( argc , argv );

    // parameters creation:
    NOMAD::Parameters p ( out );

    p.set_DIMENSION(dim);

    // define output types
    vector<NOMAD::bb_output_type> bbot (3);
    bbot[0] = NOMAD::OBJ;
    bbot[1] = NOMAD::PB;
    bbot[2] = NOMAD::EB;
    p.set_BB_OUTPUT_TYPE ( bbot );

    p.set_DISPLAY_STATS ( "bbe ( sol ) obj" );

    NOMAD::Point px0(dim),pupper(dim),plower(dim);
    // convert our input vector and bounds to NOMAD Point:
    for (int i = 0; i < dim; i++) {
       px0[i] = initial_theta[i];
       pupper[i] = upper[i];
       plower[i] = lower[i];
    }
    
    p.set_X0 ( px0 );

    p.set_LOWER_BOUND ( plower);
    p.set_LOWER_BOUND ( pupper );
    
    p.set_MAX_BB_EVAL (eval_limit);
    p.set_DISPLAY_DEGREE(2);
    //p.set_SOLUTION_FILE("sol.txt");

    // parameters validation:
    p.check();

    // custom evaluator creation:
    My_Evaluator ev   ( p );

    // algorithm creation and execution:
    NOMAD::Mads mads ( p , &ev );
    mads.run();
  }
  catch ( exception & e ) {
    cerr << "\nNOMAD has been interrupted (" << e.what() << ")\n\n";
  }

  NOMAD::Slave::stop_slaves ( out );
  NOMAD::end();
}
