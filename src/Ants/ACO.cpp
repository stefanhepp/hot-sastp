#include "ACO.h"

#include "Ants/Ant.h"

#include <algorithm>

using namespace std;

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
    
    SatisfactionList satisfaction;
    satisfaction.reserve(ants.size());
    
    do {
	
	bool printStep = env.setPrintSteps(false);
	
	deltaSatisfaction = -instance.getTotalSatisfaction();
	
	double bestSatisfaction = instance.getTotalSatisfaction();
	
	// perform steps and daemon actions with all ants
	for (size_t k = 0; k < ants.size(); k++) {
	    Ant* ant = ants[k];
	    
	    // perform ant step
	    ant->findTour(PM);
	    
	    // perform daemon action
	    localSearch.reset(ant->getInstance());
	    localSearch.run();

	    Instance& localInstance = localSearch.getInstance();
	    
	    double localSatisfaction = localInstance.getTotalSatisfaction();
	    satisfaction[k] = std::make_pair(k, localSatisfaction);
	   
	    // update the best instance
	    if (localSatisfaction > bestSatisfaction) {
		instance = localInstance;
		bestSatisfaction = localSatisfaction;
	    }
	    
	    if (!localInstance.isValid()) {
		satisfaction[k].second = -1;
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

void ACO::updatePheromones(SatisfactionList &satisfaction)
{
    // sort ants by satisfaction, use first m ants to update pheromones
    sort(satisfaction.begin(), satisfaction.end(), 
	  [](pair<size_t,double> a, pair<size_t,double> b) {
	      return a.second > b.second;
	  });
    
    int w = std::min(ants.size(), (size_t)env.getConfig().getNumUpdateBestAnts());
    
    for (int i = 0; i < w; i++) {
    }
    
    // evaporate pheromones
    PM.evaporate();
}
