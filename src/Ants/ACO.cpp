#include "ACO.h"

#include "Ants/Ant.h"

ACO::ACO(Environment& env, AbstractSearch &localSearch)
// TODO get stepsToStop from config, set initial tau value to something meaningful
: AbstractSearch(env, 100), instance(env.getProblem()), PM(env.getProblem(), 0), 
  localSearch(localSearch)
{
    // TODO get numAnts from config
    initAnts(10);
}

void ACO::reset(const Instance& init)
{
    // Does not do much though, not used to initialize pheromones or anything.
    instance = init;
    PM.reset(0);
}

void ACO::run()
{
    start();

    double deltaSatisfaction = 0;
    
    DoubleList satisfaction;
    
    do {
	
	bool printStep = env.setPrintSteps(false);
	
	deltaSatisfaction = -instance.getTotalSatisfaction();
	
	double bestSatisfaction = instance.getTotalSatisfaction();
	
	// perform steps and daemon actions with all ants
	for (size_t i = 0; i < ants.size(); i++) {
	    Ant* ant = ants[i];
	    
	    // perform ant step
	    ant->findTour(PM);
	    
	    // perform daemon action
	    localSearch.reset(ant->getInstance());
	    localSearch.run();

	    Instance& localInstance = localSearch.getInstance();
	    
	    satisfaction[i] = localInstance.getTotalSatisfaction();
	   
	    // update the best instance
	    if (satisfaction[i] > bestSatisfaction) {
		instance = localInstance;
		bestSatisfaction = satisfaction[i];
	    }
	    
	    if (!localInstance.isValid()) {
		satisfaction[i] = -1;
	    } else if (!improveForFitnessOnly) {
		ant->setInstance(localInstance);
	    }
	    
	    // TODO track satisfaction per ant over steps
	}
	
	// update pheromone matrix
	updatePheromones(satisfaction);
	
	// calculate deltaSatisfaction, print solution
	deltaSatisfaction += instance.getTotalSatisfaction();
	
	env.setPrintSteps(printStep);
	env.printStepResult(instance);
		
    } while (!shouldStop(deltaSatisfaction));

}

void ACO::initAnts(int numAnts)
{
    for (int i = 0; i < numAnts; i++) {
	// TODO create ants depending on configuration (different heuristics and PM update method)
	ants.push_back( new Ant(env, i) );
    }
}

void ACO::updatePheromones(DoubleList &satisfaction)
{

}
