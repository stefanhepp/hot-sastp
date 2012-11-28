#include "Neighborhood.h"

#include <set>
#include <algorithm>

using namespace std;

Neighborhood::Neighborhood(Environment& env)
: env(env)
{
}

unsigned OneOPT::findRandomMethod (Instance& instance, unsigned where, unsigned what, unsigned method)
{
    //check from the method position until the end of the methods for the first suitable 
    //method which gives some improvement, if found return the index
    for ( unsigned i = method+1 ; i < instance.getSpot(what).getMethods().size(); i++) 
    {
        TourNode newNode(what, i);
        TourValues diff = instance.getInsertDeltaValues(where,newNode);
        diff -= instance.getDeleteDeltaValues(where);
        
        if (instance.isValid(diff))
            return i;
        
    }
    //now check from the index 0 of the methods until method the first one which gives some improvement
    // if one is found return the index
    for ( unsigned i = 0 ; i < method; i++) 
    {
        TourNode newNode(what, i);
        TourValues diff = instance.getInsertDeltaValues(where,newNode);
        diff -= instance.getDeleteDeltaValues(where);
        
        if (instance.isValid(diff))
            return i;
        
    }
    //this means that none of the methods gives any improvement
    return method;
}

bool OneOPT::performStep (Instance& instance, Config::StepFunction stepFunction, bool alwaysApply)
{
    if (stepFunction == Config::SF_RANDOM) {
	
	set<unsigned> unusedSpots = instance.getUnusedSpotIDs();
	
	// repeat
	do{
            // pick random spot where to insert 
            unsigned spotWhereToInsert = rand() % instance.getTourLength();
           
            // spot = find random unused spot
            unsigned unusedRandomSpot = rand() % unusedSpots.size();
            
            // method = find random method of spot
            unsigned m = rand() % instance.getSpot(unusedRandomSpot).getMethods().size();
            // check if solution is feasible
            
            TourNode newNode(unusedRandomSpot,m);
            
            TourValues diff = instance.getInsertDeltaValues(spotWhereToInsert,newNode);
            diff -= instance.getDeleteDeltaValues(spotWhereToInsert);
                       
            if (instance.isValid(diff)) {
	   
                if((diff.satisfaction) > 0.0){
                //this means we incereased the total satisfaction   
                // remove old spot from tour
                // insert new spot + method into tour
                 instance.deleteNode(spotWhereToInsert);
                 instance.insertNode(spotWhereToInsert, unusedRandomSpot, m);
                    
                 return true;

                } else     
                    // if satisfaction did not increase and we do not want worse solutions, continue random search
                    if (!alwaysApply && diff.satisfaction < 0) return false;
	    
	    
            } else {
                
	    unsigned otherMethods = findRandomMethod(instance, spotWhereToInsert, unusedRandomSpot, m);
            // check other methods if any one is feasible
	    if (m == otherMethods)
                //this means none of the methods is feasable 
                unusedSpots.erase(unusedRandomSpot);
                else {
                    instance.deleteNode(spotWhereToInsert);
                    instance.insertNode(spotWhereToInsert, unusedRandomSpot, otherMethods);
                    return true;
                }
            }
        }while(unusedSpots.size() <= 1);
        // repeat select random spot until unusedSpots.size() 
        
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
	
	
	
    } else if (stepFunction == Config::SF_NEXT) {
	
	
	
    } else {
	
	
	
    }
}
