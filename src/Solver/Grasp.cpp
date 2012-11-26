#include "Grasp.h"

#include "Support/Environment.h"
#include "Support/Instance.h"
#include "Solver/AbstractSearch.h"
#include "Solver/Neighborhood.h"
#include "Solver/GreedyTour.h"

Grasp::Grasp(Environment& env, AbstractSearch& localSearch, const Instance& init)
: AbstractSearch(env, env.getConfig().getMaxStepsNoChange()), localSearch(localSearch), instance(env.getProblem()), initInstance(init)
{
    randGreedy = new GreedyRandomHeuristic(env);
}

void Grasp::reset(const Instance& init)
{
    initInstance = init;
    instance.clear();
}

void Grasp::run()
{
    start();
    
    double deltaSatisfaction = 0.0;
    
    do {
	
	// Perform randomized greedy heuristic
        randGreedy->run();
        Instance randomized = randGreedy->getInstance();
	// TODO implement 
		
	// Perform local search on result
	localSearch.reset( randomized );
	localSearch.run();
	
	deltaSatisfaction = localSearch.getInstance().getTotalSatisfaction() - instance.getTotalSatisfaction();
	if (deltaSatisfaction >= 0.0) {
	    instance = localSearch.getInstance();
	}
	
    } while (!shouldStop(deltaSatisfaction));
}

