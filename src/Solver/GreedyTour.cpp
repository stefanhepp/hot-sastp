#include "GreedyTour.h"

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
    int lastSpot = instance.empty() ? -1 : instance.getTour().back().spot;

    // find k nearest spots to last tour node
    NearestSpotList nearest = spotsearch.findNearestSpots(instance, lastSpot, maxk); 
    
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

NearestSpotList getRestrictedCandidates(NearestSpotList candidates){
    
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
    
    //get the spot from the problem
    const Spot& randomSpot = problem.getSpot(spotId);
    
    //pick randomly a valid method for that spot 
    unsigned methodId = 0; 
    unsigned randomMethod = rand() % randomSpot.getMethods().size();
    
    unsigned bestInset; 
    double deltaTour = helper.getInsertDeltaTourLength(instance, tourNode, randomSpot, insertMode, bestInset);
    _random.spot = spotId;
    _random.method = methodId;
    insertAt = bestInset;
    //return that node 
    return _random;
}


unsigned int GreedyRandomHeuristic::insertSpot()
{
    int lastSpot = instance.empty() ? -1 : instance.getTour().back().spot;
    
    //find the k nearest spots to the tour
    //in our GRASP settings this is the candidates list 
    NearestSpotList candidateList = spotsearch.findNearestSpots(instance, maxk);
    //compute the restricted candidates list 
    NearestSpotList restrictedCandidates = getRestrictedCandidates(candidateList);
    
    //pick a randomly from the restrictedCandidates one spot, 
    
    unsigned insertAt;
    TourNode random = selectRandomTourNode(restrictedCandidates, insertAt);
    return instance.insertNode(insertAt, random);
}
