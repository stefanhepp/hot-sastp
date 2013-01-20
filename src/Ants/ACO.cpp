#include "ACO.h"

#include "Ants/Ant.h"
#include "Framework/SASTProblem.h"

#include <algorithm>
#include <iostream>

using namespace std;

ACO::ACO(Environment& env, AbstractSearch &localSearch, AbstractSearch* optionalSearch, unsigned optionalSearchSteps)
: AbstractSearch(env, env.getConfig().getMaxStepsNoChange()),
  instance(env.getProblem()), bestAnt(0), PM(env), 
  localSearch(localSearch), optionalSearch(optionalSearch), optionalSearchSteps(optionalSearchSteps),
  maxSteps(env.getConfig().getNumberOfSteps())
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
    
    std::vector<AbstractSearch*> localSearches;
    for (int i = 0; i < ants.size(); i++) {
	localSearches.push_back(localSearch.clone());
    }
    
    do {
	
	bool printStep = env.setPrintSteps(false);
	
	deltaSatisfaction = -instance.getTotalSatisfaction();
	
	double bestSatisfaction = instance.getTotalSatisfaction();
	
	int bestAntIdx = -1;
	int numAnts = ants.size();
	
	if (debug) {
	    PM.print(cerr);
	}
	
	// perform steps and daemon actions with all ants
	#pragma omp parallel for
	for (size_t k = 0; k < numAnts; k++) {

	    Ant* ant = ants[k];
	
	    if (debug) cerr << "ACO: finding tour of ant " << k << " in step " << getCurrentStep() << endl;
	    
	    // perform ant step
	    ant->findTour();
	    
	    if (debug) cerr << "ACO: running local search of ant " << k << endl;
	    
	    // perform daemon action
	    localSearches[k]->reset(ant->getInstance());
	    localSearches[k]->run();
	}
	
	for (size_t k = 0; k < numAnts; k++) {
	    Instance& localInstance = localSearches[k]->getInstance();
	    
	    double localSatisfaction = localInstance.getTotalSatisfaction();
	    satisfaction[k].antId = k;
	    satisfaction[k].antSatisfaction = ants[k]->getInstance().getTotalSatisfaction();
	    satisfaction[k].optSatisfaction = localSatisfaction;
	   
	    // update the best instance
	    if (localSatisfaction > bestSatisfaction) {
		instance = localInstance;
		bestSatisfaction = localSatisfaction;
		bestAntIdx = k;
	    }
	    
	    if (!localInstance.isValid()) {
		satisfaction[k].optSatisfaction = -1;
	    } else if (improveAntSolution) {
		ants[k]->setInstance(localInstance);
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
        // TODO update minTau
	PM.setMaxTau(bestAnt->getInstance().getTotalSatisfaction());
    }
}

void ACO::updatePheromones(SatisfactionList &satisfaction, unsigned round)
{
    // evaporate pheromones
    PM.evaporate();

    // sort ants by satisfaction, use first m ants to update pheromones
    sort(satisfaction.begin(), satisfaction.end(), 
	  [](AntResults a, AntResults b) {
	      return a.optSatisfaction > b.optSatisfaction;
	  });
    
    // For Rank-based AS use:
    // - updatewithGlobalBest = 1, w > 1
    // For Min-Max use:
    // - updateWithGlobalBest = 0, w = 1
    
    int w = std::min(ants.size(), (size_t)env.getConfig().getNumUpdateBestAnts());
    
    bool updateBestAnt = updateWithGlobalBest && (round % updateWithGlobalBest) == 0 && bestAnt;
    
    bool debug = env.getConfig().doPrintBestAnts();
    
    if (debug) {
	cerr << "ACO: Updating ants:" << endl;
	if (updateBestAnt) {
	    cerr << "     - [U] Best Ant #" << bestAnt->getAntNumber() << " (K: " << bestAnt->getNeighborhood().getMaxNearestK() 
	         << ", sat: " << bestAnt->getInstance().getTotalSatisfaction() << ")" << endl;
	}
	for (int i = 0; i < ants.size(); i++) {
	  int k = satisfaction[i].antId;
	  cerr << "     - " << (i < (updateBestAnt ? w - 1 : w) ? "[U] " : "[-] ");
	  cerr << "Ant #" << k << " (K: " << ants[k]->getNeighborhood().getMaxNearestK() << ", sat: " << satisfaction[i].antSatisfaction 
	            << ", opt sat: " << satisfaction[i].optSatisfaction << ")" << endl;
	}
    }
    
    if (updateBestAnt) {
	bestAnt->addPheromones(w);
	w--;
    }
    
    for (int i = 0; i < w; i++) {
	int k = satisfaction[i].antId;
	ants[k]->addPheromones(w - i);
    }
}
