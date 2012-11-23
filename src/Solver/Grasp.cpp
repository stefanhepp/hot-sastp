#include "Grasp.h"

#include "Support/Environment.h"
#include "Support/Instance.h"
#include "Solver/AbstractSearch.h"
#include "Solver/Neighborhood.h"


Grasp::Grasp(Environment& env, AbstractSearch& localSearch, const Instance& init)
: AbstractSearch(env), localSearch(localSearch), instance(env.getProblem()), initInstance(init)
{
}

void Grasp::reset(const Instance& init)
{
    initInstance = init;
    instance.clear();
}

void Grasp::run()
{
    while (!shouldStop()) {
	
	// Perform randomized greedy heuristic 
	// TODO implement 
	Instance randomized = initInstance;
	
	// Perform local search on result
	localSearch.reset( randomized );
	localSearch.run();
	
	if (localSearch.getInstance().getTotalSatisfaction() <= instance.getTotalSatisfaction()) {
	    instance = localSearch.getInstance();
	}
    }
}

