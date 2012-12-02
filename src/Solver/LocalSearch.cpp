#include "LocalSearch.h"

#include "Support/Environment.h"
#include "Support/Instance.h"
#include "Solver/AbstractSearch.h"
#include "Solver/Neighborhood.h"


LocalSearch::LocalSearch(Environment& env, Neighborhood& nb, const Instance& init)
: AbstractSearch(env, env.getConfig().getMaxStepsNoChange()), neighborhood(nb), instance(init)
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
	
	env.printStepResult(instance);
	
	// Choose an x' in N(x), apply if f(x') <= f(x)
	
	// Note: in order not to copy the whole instance in every step, we 
	//       let the neighborhood handle the update. The neighborhood 
	//       may evaluate the choosen step before modifying the instance.
	
	deltaSatisfaction = -instance.getTotalSatisfaction();
	
	neighborhood.performStep(instance, stepFunction, false);
	
	deltaSatisfaction += instance.getTotalSatisfaction();
	
	// No need to continue if we use a deterministic step function..
	if (deltaSatisfaction <= 0.0 && stepFunction != Config::SF_RANDOM) break;
	
    } while (!shouldStop(deltaSatisfaction));
}
