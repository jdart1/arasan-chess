// Based on example code packaged with Nomad
#include "nomad.h"
#include <nomad.hpp>
#include "types.h"
using namespace std;

Nomad::Nomad(int d, const std::vector<double> &x0, int eval_limit):
   OptBase(d) 
{
   setEvaluationLimit(eval_limit);
   
   setInitialPoints(x0);
   // We currently assume values are scaled to the unit hypercube,
   // by default
   lower.resize(dim);
   lower.assign(dim,0.0);
   upper.resize(dim);
   upper.assign(dim,1.0);
}

Nomad::~Nomad() 
{
}

static int local_dim;

static double (*local_func)(const std::vector<double> &theta);

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
         std::vector<double> ex(local_dim);
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

void Nomad::setBoxConstraints(const std::vector<double> &lower,
                              const std::vector<double> &upper) 
{
   this->lower = lower;
   this->upper = upper;
}

void Nomad::optimize(double (*func)(const std::vector<double> &theta),
                     void (*update)(double obj, const std::vector<double> &theta))
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

    srand((unsigned)(getCurrentTime() % (1<<31)));
    NOMAD::RNG::set_seed(rand() % 12345);

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

    NOMAD::Point px0(dim);
    // convert our input vector to NOMAD Point:
    for (int i = 0; i < dim; i++) {
       px0[i] = NOMAD::Double(initial_theta[i]);
       p.set_LOWER_BOUND(i,0.0);
       p.set_UPPER_BOUND(i,1.0);
    }
    
    p.set_X0 ( px0 );

    std::list<NOMAD::bb_input_type> bbit;
    for (int i = 0; i < dim; i++) {
       // all params are real
       bbit.push_back(NOMAD::CONTINUOUS);
    }
    p.set_BB_INPUT_TYPE ( bbit);
    
    p.set_MAX_BB_EVAL (eval_limit);
//    p.set_SNAP_TO_BOUNDS(true);
    p.set_INITIAL_POLL_SIZE(NOMAD::Double(0.25));
    p.set_DISPLAY_DEGREE("2222");
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
