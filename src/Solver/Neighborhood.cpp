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
	    
	    for (unsigned newSpot : env.getSpotSearch().findNearestSpots(instance, spot, unusedSpots.size())) {
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

}
