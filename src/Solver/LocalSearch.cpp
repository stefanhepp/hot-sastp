#include "LocalSearch.h"

#include "Support/Environment.h"
#include "Support/Instance.h"
#include "Solver/AbstractSearch.h"
#include "Solver/Neighborhood.h"


LocalSearch::LocalSearch(Environment& env, Neighborhood& nb, const Instance& init)
: AbstractSearch(env), neighborhood(nb), instance(init)
{
}

void LocalSearch::reset(const Instance& init)
{
    instance = init;
}

void LocalSearch::run()
{
  double deltaSatisfaction;
  start();
    do {
	
	// Choose an x' in N(x), apply if f(x') <= f(x)
	
	// Note: in order not to copy the whole instance in every step, we 
	//       let the neighborhood handle the update. The neighborhood 
	//       may evaluate the choosen step before modifying the instance.
	
	deltaSatisfaction = -instance.getTotalSatisfaction();
	
	bool applied = neighborhood.performStep(instance, stepFunction, false);
	
	deltaSatisfaction += instance.getTotalSatisfaction();
		
    }while(!shouldStop(deltaSatisfaction));
}
