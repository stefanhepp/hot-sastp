#include "ACO.h"

#include "Ants/Ant.h"
#include "Framework/SASTProblem.h"

#include <algorithm>
#include <iostream>

using namespace std;

ACO::ACO(Environment& env, AbstractSearch &localSearch)
: AbstractSearch(env, env.getConfig().getMaxStepsNoChange()),
  instance(env.getProblem()), bestAnt(0), PM(env), 
  localSearch(localSearch), maxSteps(env.getConfig().getNumberOfSteps())
{
    improveAntSolution = env.getConfig().doImproveAntSolution();
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

    bool debug = env.getConfig().isDebug();
    
    double deltaSatisfaction = 0;
    
    SatisfactionList satisfaction;
    satisfaction.resize(ants.size());
    
    // just to be on the safe side, initialize best ant with empty tour
    setBestAnt(ants[0]);
    
    do {
	
	bool printStep = env.setPrintSteps(false);
	
	deltaSatisfaction = -instance.getTotalSatisfaction();
	
	double bestSatisfaction = instance.getTotalSatisfaction();
	
	int bestAntIdx = -1;
	int numAnts = ants.size();
	
	// perform steps and daemon actions with all ants
	#pragma omp parallel for
	for (size_t k = 0; k < numAnts; k++) {

	    Ant* ant = ants[k];
	
	    if (debug) cerr << "ACO: finding tour of ant " << k << " in step " << getCurrentStep() << endl;
	    
	    // perform ant step
	    ant->findTour();
	    
	    if (debug) cerr << "ACO: running local search\n";
	    
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
	    } else if (improveAntSolution) {
		ant->setInstance(localInstance);
	    }
	    
	    // TODO track satisfaction per ant over steps
	}
	
	if (bestAntIdx >= 0) {
	    setBestAnt(ants[bestAntIdx]);
	}
	
	if (debug) cerr << "ACO: updating pheromones\n";
	
	// update pheromone matrix
	updatePheromones(satisfaction, getCurrentStep());
	
	// calculate deltaSatisfaction, print solution
	deltaSatisfaction += instance.getTotalSatisfaction();
	
	env.setPrintSteps(printStep);
	env.printStepResult(instance);
	
    } while (!shouldStop(deltaSatisfaction) && getCurrentStep() <= maxSteps);

}

void ACO::initAnts(int numAnts)
{
    for (int k = 0; k < numAnts; k++) {
	Ant *ant;
	
	AntNeighborhood *nb = new AntNeighborhood(env, k);
	
	if (env.getConfig().getAntHeuristics() == Config::AH_NEAREST) {
	    ant = new AntNearest(env,PM, *nb, k);
	} else {
	    ant = new AntInsert(env,PM, *nb, k);
	}
	
	ants.push_back( ant );
    }
}

void ACO::setBestAnt(Ant* ant)
{
    if (bestAnt) delete bestAnt;
    bestAnt = ant->clone();
    if (env.getConfig().getTauMax() < 0) {
	PM.setMaxTau(bestAnt->getInstance().getTotalSatisfaction());
    }
}

void ACO::updatePheromones(SatisfactionList &satisfaction, unsigned round)
{
    // evaporate pheromones
    PM.evaporate();

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
    
    bool updateBestAnt = updateWithGlobalBest && (round % updateWithGlobalBest) == 0 && bestAnt;
    
    bool debug = env.getConfig().doPrintBestAnts();
    
    
    
    if (updateBestAnt) {
	bestAnt->addPheromones(w);
	w--;
    }
    
    for (int i = 0; i < w; i++) {
	int k = satisfaction[i].first;
	ants[k]->addPheromones(w - i);
    }    
}
