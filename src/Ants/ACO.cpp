#include "ACO.h"

#include "Ants/Ant.h"
#include "Framework/SASTProblem.h"

#include <algorithm>

using namespace std;

ACO::ACO(Environment& env, AbstractSearch &localSearch)
: AbstractSearch(env, env.getConfig().getNumberOfSteps()),
  instance(env.getProblem()), bestAnt(0), PM(env), 
  localSearch(localSearch)
{
    improveForFitnessOnly = env.getConfig().doImproveForFitnessOnly();
    updateWithGlobalBest = env.getConfig().getUpdateWithGlobalBest();
    
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
    
    unsigned round = 0;
    
    do {
	
	bool printStep = env.setPrintSteps(false);
	
	deltaSatisfaction = -instance.getTotalSatisfaction();
	
	double bestSatisfaction = instance.getTotalSatisfaction();
	
	int bestAntIdx = -1;
	
	// perform steps and daemon actions with all ants
	for (size_t k = 0; k < ants.size(); k++) {
	    Ant* ant = ants[k];
	    
	    // perform ant step
	    ant->findTour();
	    
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
		bestAntIdx = k;
	    }
	    
	    if (!localInstance.isValid()) {
		satisfaction[k].second = -1;
	    } else if (!improveForFitnessOnly) {
		ant->setInstance(localInstance);
	    }
	    
	    // TODO track satisfaction per ant over steps
	}
	
	if (bestAntIdx >= 0) {
	    setBestAnt(ants[bestAntIdx]);
	}
	
	// update pheromone matrix
	updatePheromones(satisfaction, round);
	
	// calculate deltaSatisfaction, print solution
	deltaSatisfaction += instance.getTotalSatisfaction();
	
	env.setPrintSteps(printStep);
	env.printStepResult(instance);
	
	round++;
	
    } while (!shouldStop(deltaSatisfaction));

}

void ACO::initAnts(int numAnts)
{
    for (int i = 0; i < numAnts; i++) {
	Ant *ant;
	
	if (env.getConfig().getAntHeuristics() == Config::AH_NEAREST) {
	    ant = new AntNearest(env,PM, i);
	} else {
	    ant = new AntInsert(env,PM, i);
	}
	
	ants.push_back( ant );
    }
}

void ACO::setBestAnt(Ant* ant)
{
    if (bestAnt) delete bestAnt;
    bestAnt = ant->clone();
}

void ACO::updatePheromones(SatisfactionList &satisfaction, unsigned round)
{
    // sort ants by satisfaction, use first m ants to update pheromones
    sort(satisfaction.begin(), satisfaction.end(), 
	  [](pair<size_t,double> a, pair<size_t,double> b) {
	      return a.second > b.second;
	  });
    
    // For Rank-based AS use:
    // - updatewithGlobalBest = 1, w > 1
    // For Min-Max use:
    // - updateWithGlobalBest = 0, w = 1
    
    int w = std::min(ants.size(), (size_t)env.getConfig().getNumUpdateBestAnts());
    
    if (updateWithGlobalBest && (round % updateWithGlobalBest) == 0 && bestAnt) {
	bestAnt->addPheromones( w);
	w--;
    }
    
    for (int i = 0; i < w; i++) {
	int k = satisfaction[i].first;
	ants[k]->addPheromones(w - i);
    }
    
    // evaporate pheromones
    PM.evaporate();
}
