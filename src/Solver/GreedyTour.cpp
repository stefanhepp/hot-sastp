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
	unsigned spotid = ps.second;
	
	const Spot& nearestspot = problem.getSpot(spotid);
	
	
	for (const auto& m : nearestspot.getMethods()) {
	    
	    
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
