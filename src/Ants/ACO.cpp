#include "ACO.h"

#include "Ants/Ant.h"

ACO::ACO(Environment& env, AbstractSearch &localSearch)
: AbstractSearch(env, env.getConfig().getNumberOfSteps()), env(env),
  instance(env.getProblem()), PM(env), 
  localSearch(localSearch)
{
    initAnts(env.getConfig().getNumberOfAnts());
}

void ACO::reset(const Instance& init)
{
    instance = init;
    PM.reset(env.getConfig().getInitialTau());
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
	Ant *ant;
	
	if (env.getConfig().getAntHeuristics() == Config::AH_NEAREST) {
	    ant = new AntNearest(env, i);
	} else {
	    ant = new AntInsert(env, i);
	}
	
	ants.push_back( ant );
    }
}

void ACO::updatePheromones(DoubleList &satisfaction)
{
    // TODO sort ants by satisfaction, use first m ants to update pheromones
    
    
    // TODO evaporate pheromones
    
    
}
