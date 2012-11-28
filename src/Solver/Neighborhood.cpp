#include "Neighborhood.h"

#include <set>
#include <algorithm>

using namespace std;

Neighborhood::Neighborhood(Environment& env)
: env(env)
{
}

bool OneOPT::performStep (Instance& instance, Config::StepFunction stepFunction, bool alwaysApply)
{
    if (stepFunction == Config::SF_RANDOM) {
	
	set<unsigned> unusedSpots = instance.getUnusedSpotIDs();
	
	// repeat
	
	// pick random spot where to insert 
	
	// spot = find random unused spot
	
	// method = find random method of spot
	
	// check if solution is feasible
	
	//TourValues diff = instance.getInsertDeltaValues(newSpot) - instance.getDeleteDeltaValues(oldSpot);
	
	//if (instance.isValid(diff)) {
	    
	    // if satisfaction did not increase and we do not want worse solutions, continue random search
	    // if (!alwaysApply && diff.satisfaction < 0) return false;
	    
	    // remove old spot from tour
	    // insert new spot + method into tour
	    // return true
	    
	//} else {
	    // check other methods if any one is feasible
	//}
	
	// otherwise, remove spot from unusedSpots, repeat select random spot until unusedSpots.size() 
	
	return false;
	
    } else if (stepFunction == Config::SF_NEXT) {
	
	set<unsigned> unusedSpots = instance.getUnusedSpotIDs();
	
	vector<unsigned> sortedNodes;
	sortedNodes.reserve(unusedSpots.size());
	
	vector<double> ratios;
	ratios.reserve(unusedSpots.size());
	
	for(auto& node : instance.getTour()) {
	    // get satisfaction / total Time ratio per spot
	    TourValues diff = instance.getStepValues(node.spot);
	
	    double ratio = instance.getSatisfactionPerTotalTimeRatio(diff);
	    
	    // insert spot into sortedNodes according to ratio
	    
	    auto it = upper_bound(ratios.begin(), ratios.end(), ratio);
	    ratios.insert(it, ratio);
	    sortedNodes.insert(sortedNodes.begin() + (it - ratios.begin()), node.spot);
	}
	
	
	for (unsigned spot : sortedNodes) {
	    
	    // find first unused spot that is near to spot and gives an satisfaction improvement
	    
	    for (auto& newSpot : env.getSpotSearch().findNearestSpots(instance, spot, unusedSpots.size())) {
		//TourValues diff = instance.getInsertDeltaValues(newSpot) - instance.getDeleteDeltaValues(oldSpot);
		
		//if (instance.isValid(diff)) {
		    
		    // if satisfaction did not increase and we do not want worse solutions, continue random search
		    // if (!alwaysApply && diff.satisfaction < 0) return false;
		    
		    // remove old spot from tour
		    // insert new spot + method into tour
		    // return true
		    
		//}
	    }
	    
	}
	
	
    } else {
	
	for (auto& node : instance.getTour()) {
	    
	    // find first unused spot that is near to spot and gives an satisfaction improvement
	    
	    //for (unsigned newSpot : env.getSpotSearch().findNearestSpots(instance, spot, unusedSpots.size())) {
		//TourValues diff = instance.getInsertDeltaValues(newSpot) - instance.getDeleteDeltaValues(oldSpot);
		
		//if (instance.isValid(diff)) {
		    
		    
		//}
	    //}
	    
	}
    
	// if satisfaction did not increase and we do not want worse solutions, continue random search
	// if (!alwaysApply && diff.satisfaction < 0) return false;
	
	// remove old spot from tour
	// insert new spot + method into tour
	
	// return false if no spot with an improvement was found
    }
}

bool TwoOPT::performStep (Instance& instance, Config::StepFunction stepFunction, bool alwaysApply)
{
    if (stepFunction == Config::SF_RANDOM) {
	
	int firstEdge = rand() % (instance.getTourLength() - 1);
	int secondEdge = rand() % (instance.getTourLength() - firstEdge) + firstEdge + 1;
	
	
	
    } else if (stepFunction == Config::SF_NEXT) {
	
	int tourLength = instance.getTourLength();
	int maxDist = (tourLength - 1) / 2;
	
	// iterate first over the first edge, then the *distance* between the edges
	for (int distance = 2; distance < maxDist; distance++) {
	    
	    for (int firstEdge = 0; firstEdge < tourLength; firstEdge++) {
		int secondEdge = firstEdge + distance;
		
		
		
		
		double deltaSatisfaction;
		if (isValidEdgeExchange(instance, firstEdge, secondEdge, deltaSatisfaction)) {
		    
		    if (alwaysApply || deltaSatisfaction >= 0) {
			
			performEdgeExchange(instance, firstEdge, secondEdge);

			return true;
		    }
		}
		
	    }
	    
	}
	
    } else {
	int tourLength = instance.getTourLength();
	
	int bestFirst = 0;
	int bestSecond = 0;
	double bestSatisfaction;
	
	for (int firstEdge = 0; firstEdge < tourLength - 1; firstEdge++) {
	    for (int secondEdge = firstEdge + 2; secondEdge < tourLength + 1; secondEdge++) {
		
		double deltaSatisfaction;
		
		if (isValidEdgeExchange(instance, firstEdge, secondEdge, deltaSatisfaction)) {
		    
		    if (deltaSatisfaction > bestSatisfaction || (bestFirst == bestSecond)) {
			bestSatisfaction = deltaSatisfaction;
			bestFirst = firstEdge;
			bestSecond = secondEdge;
		    }
		    
		}
	    }
	}
	
	if (alwaysApply || bestSatisfaction >= 0) {
	 
	    performEdgeExchange(instance, bestFirst, bestSecond);
	    
	    return true;
	}
    }
    
    return false;
}

bool TwoOPT::isValidEdgeExchange(Instance& instance, int firstEdge, int secondEdge, double& deltaSatisfaction)
{

    
    
}

bool TwoOPT::performEdgeExchange(Instance& instance, int firstEdge, int secondEdge)
{

    
}
