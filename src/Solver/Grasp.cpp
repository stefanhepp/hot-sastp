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

	// Do not print steps of used searches
	bool printSteps = env.setPrintSteps(false);
	
	// Perform randomized greedy heuristic
        randGreedy->reset(env.getEmptyInstance());
        randGreedy->run();
        
	// Perform local search on result
	localSearch.reset( randGreedy->getInstance() );
	localSearch.run();
	
	deltaSatisfaction = localSearch.getInstance().getTotalSatisfaction() - instance.getTotalSatisfaction();
	if (deltaSatisfaction >= 0.0) {
	    instance = localSearch.getInstance();
	}
	
	env.setPrintSteps(printSteps);
	env.printStepResult(instance);
	
   } while (!shouldStop(deltaSatisfaction));
}

