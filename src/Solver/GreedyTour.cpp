#include "GreedyTour.h"

#include "Framework/SASTProblem.h"
#include "Support/Instance.h"
#include "Support/SpotSearch.h"

GreedyTour::GreedyTour(Environment& env)
: instance(env.getProblem()), problem(env.getProblem()), spotsearch(env.getSpotSearch())
{
    maxk = env.getConfig().getMaxKNearestSpots(); 
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
    }
    
}

TourNode GreedyTour::selectBestTourNode ( NearestSpotList nearest, unsigned &insertAt, Config::NodeInsertMode insertMode )
{
    TourNode best(-1, 0);
    // start off with something really bad so that we are sure to select the first node
    float bestRatio = -1.0f;
    
    for(const auto& ps : nearest) {
	int tournode = ps.first;
	unsigned spotId = ps.second;
	
	const Spot& spot = instance.getSpot(tournode);
	const Spot& nearestspot = problem.getSpot(spotId);
	double dist = problem.getDistance(spot, nearestspot);
	
	unsigned bestInsert;
	double deltaTour;
	
	switch (insertMode) {
	    case Config::NIM_ALWAYS_AFTER: {
		// Insert after the node with the currently nearest spot
		bestInsert = tournode + 1;
		const Spot& next = instance.getSpot(tournode + 1);
		deltaTour = dist + problem.getDistance(nearestspot, next)
		            - problem.getDistance(spot, next);
		break;
	    }
	    case Config::NIM_ALWAYS_BEFORE: {
		// Insert before the node with the currenty nearest spot (except for the origin)
		bestInsert = tournode == -1 ? 0 : tournode; 
		const Spot& prev = instance.getSpot(tournode == -1 ? 0 : tournode - 1);
		deltaTour = dist + problem.getDistance(nearestspot, prev) 
		            - problem.getDistance(spot, prev);
		break;
	    }
	    case Config::NIM_SHORTEST_PATH: {
		
		// if we are at the origin, always insert after the node
		if (tournode == -1) {
		    bestInsert = tournode + 1;
		    const Spot& next = instance.getSpot(tournode + 1);
		    deltaTour = dist + problem.getDistance(nearestspot, next)
				- problem.getDistance(spot, next);
		    break;
		}
		
		// Select to insert either before or after the node, depending on the distance differences
		const Spot& prev = instance.getSpot(tournode - 1);
		const Spot& next = instance.getSpot(tournode + 1);
		
		double deltaBefore = dist + problem.getDistance(nearestspot, prev) 
		                     - problem.getDistance(spot, prev);
		double deltaAfter  = dist + problem.getDistance(nearestspot, next)
		                     - problem.getDistance(spot, next);
		
		bestInsert = deltaBefore < deltaAfter ? tournode : tournode + 1;
		deltaTour = std::min(deltaBefore, deltaAfter);
		break;
	    }
	}
	
	// check all methods of this spot
	unsigned methodId = 0;
	for (const auto& m : nearestspot.getMethods()) {
	    
	    float deltaSatisfaction = m->getSatisfaction() - deltaTour * problem.getAlpha();
	    float deltaTime = m->getTime() + deltaTour / problem.getVelocity();
	   
	    // honor loss of time by reduced stamina, if remaining stamina gets below zero
	    double newStamina = instance.getRemainingStamina() - m->getStamina();
	    if (newStamina < 0.0) {
		deltaTime += -newStamina * problem.getHabitus();
	    }
	    
	    float ratio = deltaSatisfaction / deltaTime;
	    
	    if (ratio > bestRatio) {
		// found a new best candidate
		best.spot = spotId;
		best.method = methodId;
		bestRatio = ratio;
		insertAt = bestInsert;
	    }
	    
	    methodId++;
	}
	
    }
    
    
    return best;
}



GreedyNearestNeighbor::GreedyNearestNeighbor ( Environment& env ) : GreedyTour ( env )
{
}

unsigned int GreedyNearestNeighbor::insertSpot()
{
    int lastSpot = instance.empty() ? -1 : instance.getTour().back().spot;

    // find k nearest spots to last tour node
    NearestSpotList nearest = spotsearch.findNearestSpots(lastSpot, maxk); 
    
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

