#include "GreedyTour.h"

#include <stdlib.h>
#include <iostream>

#include "Framework/SASTProblem.h"
#include "Support/Instance.h"
#include "Support/SpotSearch.h"

GreedyTour::GreedyTour(Environment& env)
: AbstractSearch(env, 1), instance(env.getProblem()), spotsearch(env.getSpotSearch()),
  helper(env.getProblem(), env.getSpotSearch())
{
    maxk = env.getConfig().getMaxKNearestSpots(); 
}

void GreedyTour::reset(const Instance& init)
{
    instance = init;
}

void GreedyTour::run()
{
    size_t numSpots = getInstance().getProblem().getSpots().size();
    
    while (instance.getTourLength() < numSpots) {
	
	// add a new spot into the tour
	unsigned newSpot = insertSpot();
	
	// If inserting the new spot makes the tour too long, do not add it to the tour and stop
	if (!instance.isValid()) {
	    instance.deleteNode(newSpot);
	    break;
	}
	
	env.printStepResult(instance);
    }
    
}

TourNode GreedyTour::selectBestTourNode ( NearestSpotList nearest, unsigned &insertAt, Config::NodeInsertMode insertMode )
{
    TourNode best(-1, 0);
    // start off with something really bad so that we are sure to select the first node
    double bestRatio = -1.0f;
    
    for(const auto& ps : nearest) {
	int tournode = ps.first;
	unsigned spotId = ps.second;
	
	const Spot& nearestspot = problem.getSpot(spotId);
	
	unsigned bestInsert;
	double deltaTour = helper.getInsertDeltaTourLength(instance, tournode, nearestspot, insertMode, bestInsert);
		
	// check all methods of this spot
	unsigned methodId = 0;
	for (const auto& m : nearestspot.getMethods()) {
	    
	    double deltaTime;
	    double ratio = helper.calcInsertSatisfactionTimeRatio(instance.getRemainingStamina(), *m, deltaTour, deltaTime);
	    
	    // check if we have a new best value, but check for time constraints ..
	    if (instance.getTotalTime() + deltaTime <= problem.getMaxTime() && 
		ratio > bestRatio) 
	    {
		// found a new best candidate
		best.spot = spotId;
		best.method = methodId;
		bestRatio = ratio;
		insertAt = bestInsert;
	    }
	    
	    methodId++;
	}
	
    }
    
    // Too bad, we did not find a node that still fits, just return the first nearest one..
    if (best.spot == -1) {
	insertAt = nearest.front().first + 1;
	best.spot = nearest.front().second;
	best.method = 0;
    }
    
    return best;
}



GreedyNearestNeighbor::GreedyNearestNeighbor ( Environment& env ) : GreedyTour ( env )
{
}

unsigned int GreedyNearestNeighbor::insertSpot()
{
    // find k nearest spots to last tour node
    NearestSpotList nearest = spotsearch.findNearestSpots(instance, instance.getTourLength() - 1, maxk); 
    
    // find best spot and method, add it to the tour
    unsigned tmp;
    TourNode best = selectBestTourNode(nearest, tmp);
    
    return instance.addNode(best);
}



GreedyInsertHeuristic::GreedyInsertHeuristic ( Environment& env ) : GreedyTour ( env )
{
    nodeInsertMode = env.getConfig().getNodeInsertMode();
}

unsigned int GreedyInsertHeuristic::insertSpot()
{
    // find k nearest spots to tour nodes
    NearestSpotList nearest = spotsearch.findNearestSpots(instance, maxk);
    
    // find best spot and method, add it to the tour
    unsigned insertAt;
    TourNode best = selectBestTourNode(nearest, insertAt, nodeInsertMode);
        
    return instance.insertNode(insertAt, best);
}

GreedyRandomHeuristic::GreedyRandomHeuristic (Environment& env) : GreedyTour (env)
{

}

GreedyTour::SpotMethodList GreedyTour::getRestrictedCandidates (NearestSpotList candidates)
{

    
    GreedyTour::SpotMethodList RCL;
    double minRatio, maxRatio;
    maxRatio = -10.0;
    minRatio = 10000.0;
    
    RCL.reserve(candidates.size());
    // Anything to do here?
    //yes we have to select all the spots where cost(spot) < cost_min + alpha*(cost_max -cost_min )
    //where alpha is in [0..1].
    double min_cost, max_cost;
   
    for( const auto& c : candidates){
        SpotMethod sm; 
       
        unsigned tournode = c.first;
        unsigned spotId = c.second;
        
        Spot& spot = problem.getSpot(spotId);
       
        sm.first = spotId;
       
        unsigned bestInsert;
        
        double deltaTour = GreedyTour::helper.getInsertDeltaTourLength(instance, tournode, spot, env.getConfig().getNodeInsertMode(), bestInsert);
                
        // check all methods of this spot
        unsigned methodId = 0;
        for (const auto& m : spot.getMethods()) {
            
            double deltaTime;
            double ratio = helper.calcInsertSatisfactionTimeRatio(instance.getRemainingStamina(), *m, deltaTour, deltaTime);
            
            // check if we have a new best value, but check for time constraints ..
            if (instance.getTotalTime() + deltaTime <= problem.getMaxTime() && 
                ratio > maxRatio) 
            {
                // found a new best candidate
                maxRatio = ratio;
            } else 
                if( ratio < minRatio) 
                    minRatio = ratio;
            sm.second = methodId;
            methodId++;
            
            RCL.push_back(sm);
        }
        
    }
    
    
   int eraseMe = 0;
   SpotMethodList CleanRCL;
   CleanRCL.reserve(RCL.size()/2);
   
   
    for ( const auto& ite : RCL){
        Spot& spot = problem.getSpot(ite.first);
        Method m = spot.getMethod(ite.second);
        
        SpotMethod sm ;
        sm.first = ite.first;
        sm.second = sm.second;
        
        unsigned bestInsert;
        double deltaTour = GreedyTour::helper.getInsertDeltaTourLength(instance, candidates[0].first, spot, env.getConfig().getNodeInsertMode(), bestInsert);
        
        double deltaTime;
        double ratio = GreedyTour::helper.calcInsertSatisfactionTimeRatio(instance.getRemainingStamina(), m, deltaTour, deltaTime);
        double st = minRatio + env.getConfig().getAlpha() * (maxRatio-minRatio);
        if( ratio <= st)
            CleanRCL.push_back(sm);
              
        eraseMe++;
    } 
    
    
    return RCL;

}

TourNode GreedyTour::selectRandomTourNode (NearestSpotList nearest, unsigned int& insertAt, Config::NodeInsertMode insertMode)
{
    TourNode _random(-1,0);
    
    //pick a random valid index 
    unsigned int maxIndexNode = nearest.size();
    unsigned int randomChoice = rand() % maxIndexNode;
    
    //get the node in the RCL with that index
    unsigned tourNode = nearest[randomChoice].first;
    unsigned spotId = nearest[randomChoice].second;
    
    getRestrictedCandidates(nearest);
    //get the spot from the problem
    const Spot& randomSpot = problem.getSpot(spotId);
    
    //pick randomly a valid method for that spot  
    unsigned randomMethod = rand() % randomSpot.getMethods().size();
    
    double deltaTour = helper.getInsertDeltaTourLength(instance, tourNode, randomSpot, insertMode, insertAt);
    _random.spot = spotId;
    _random.method = randomMethod;
 
    return _random;
}

TourNode GreedyTour::selectRandomTourNode(SpotMethodList restricted)
{
    TourNode _random(-1,0);
    
    unsigned maxIndexNode = restricted.size();
    unsigned randomChoice;
    if (maxIndexNode != 0 ){
        randomChoice = rand() % maxIndexNode;
    
    
    _random.method = restricted[randomChoice].second;
    _random.spot = restricted[randomChoice].first;
    
    return _random;}
    else {
        return _random;
    }
}

unsigned int GreedyRandomHeuristic::insertSpot()
{
    int lastNode = instance.getTourLength() - 1;
    
    //find the k nearest spots to the tour
    //in our GRASP settings this is the candidates list 
    NearestSpotList candidateList = spotsearch.findNearestSpots(instance, lastNode, maxk);
    //compute the restricted candidates list 
    SpotMethodList restrictedCandidates = getRestrictedCandidates(candidateList);
    
    //pick a randomly from the restrictedCandidates one spot, 
    
    TourNode random = selectRandomTourNode(restrictedCandidates);
    return instance.addNode(random);
}

void GreedyRandomHeuristic::reset (const Instance& inst)
{
    GreedyTour::reset(inst);
}

