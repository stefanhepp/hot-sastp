#include "Neighborhood.h"

#include <set>
#include <algorithm>

#include <iostream>
using namespace std;

Neighborhood::Neighborhood(Environment& env)
: env(env)
{
}

unsigned OneOPT::findRandomMethod (Instance& instance, unsigned where, unsigned what, unsigned method)
{
    //check from the method position until the end of the methods for the first suitable 
    //method which gives some improvement, if found return the index
    for ( unsigned i = method+1 ; i < instance.getProblem().getSpot(what).getMethods().size(); i++) 
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
	
        unsigned spotWhereToInsert;
	// repeat
	while(unusedSpots.size() >= 1){
             //std::cout<< unusedSpots.size()<<std::endl;
            // pick random spot where to insert 
            spotWhereToInsert = rand() % instance.getTourLength();
           
            // spot = find random unused spot
            unsigned unusedRandomSpot = rand() % unusedSpots.size();
            set<unsigned>::iterator it(unusedSpots.begin());
            advance(it, unusedRandomSpot);
            //std::cout<<*it<<endl;
            // method = find random method of spot
            unsigned m = rand() % instance.getProblem().getSpot(*it).getMethods().size();
            // check if solution is feasible
            
            TourNode newNode(*it,m);
            
            TourValues diff = instance.getInsertDeltaValues(spotWhereToInsert,newNode);
            diff -= instance.getDeleteDeltaValues(spotWhereToInsert);
                       
            if (instance.isValid(diff)) {
	   
                if((diff.satisfaction) > 0.0){
                //this means we incereased the total satisfaction   
                // remove old spot from tour
                // insert new spot + method into tour
                 instance.deleteNode(spotWhereToInsert);
                 instance.insertNode(spotWhereToInsert,*it, m);
                    
                 return true;

                } else     
                    // if satisfaction did not increase and we do not want worse solutions, continue random search
                    if (!alwaysApply && diff.satisfaction < 0) return false;
	    
	    
            } else {
                
	    unsigned otherMethods = findRandomMethod(instance, spotWhereToInsert, *it, m);
            std::set<unsigned>::iterator tmp;
            // check other methods if any one is feasible
	    if (m == otherMethods){
                //this means none of the methods is feasable 
                tmp = it;
                ++tmp;
                unusedSpots.erase(it);
                it = tmp;
                
            }
                else {
                    instance.deleteNode(spotWhereToInsert);
                    instance.insertNode(spotWhereToInsert, *it, otherMethods);
                    return true;
                }
            }
        }
        // repeat select random spot until unusedSpots.size() 
       
	return false;
	
    } else if (stepFunction == Config::SF_NEXT) {
	
	set<unsigned> unusedSpots = instance.getUnusedSpotIDs();
	
	vector<unsigned> sortedNodes;
	sortedNodes.reserve(instance.getTour().size());
	
	vector<double> ratios;
	ratios.reserve(instance.getTour().size());
	
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
                
              for( unsigned m = 0 ; m < env.getProblem().getSpot(newSpot.first).getMethods().size(); m++) {
                 
                  TourNode newNode(newSpot.first, m);
                  
                  TourValues diff = instance.getInsertDeltaValues(spot,newNode) - instance.getDeleteDeltaValues(spot);
		
                  if (instance.isValid(diff)) {
		    
		     if (!alwaysApply && diff.satisfaction < 0) return false;
		    
                     instance.deleteNode(spot);
                     instance.insertNode(spot, newSpot.first, m);
                     return true;
		    // remove old spot from tour
		    // insert new spot + method into tour
		    // return true
		    
		}
              }
	    }
	    
	}
	
	return false;
    } else {
        //set of unused spotID's 
	set<unsigned> unusedSpots = instance.getUnusedSpotIDs();
	

        double maxSatisfaction = 0; 
        unsigned maxWhatToRemove = -1;
        unsigned maxWhatToInsert = -1;
        
        //for each node in the tour
        for (auto& node : instance.getTour()) {
	    
            NearestSpotList nearest = env.getSpotSearch().findNearestSpots(instance, node.spot, unusedSpots.size());
            //check for all unused spots for the one which gives the best improvement
            for (auto& nearSpot : nearest){
                int tournode = nearSpot.first;
                unsigned spotId = nearSpot.second;
                unsigned m = 0;
                const Spot& nearestspot = env.getProblem().getSpot(spotId);
                for( auto& meth : nearestspot.getMethods()){
                    
                    TourNode n(spotId, m);

                  //  TourValues diff = instance.getInsertDeltaValues( ,n) - instance.getDeleteDeltaValues(node.spot);
                 /*   if( instance.isValid(diff) ) 

                    TourValues diff = instance.getInsertDeltaValues(node.spot,n) - instance.getDeleteDeltaValues(node.spot);
                    if( instance.isValid(diff) ) 

                    {
                        //just to test how bad we crash 
                       instance.deleteNode(node.spot);
                       instance.insertNode(node.spot, n.spot, m);
                        return true;
                        

                    }*/
                    m++;
                }
            }
            
            //keep track of the maximal improvement 
	    // find first unused spot that is near to spot and gives an satisfaction improvement
	    
	    //for (unsigned newSpot : env.getSpotSearch().findNearestSpots(instance, spot, unusedSpots.size())) {
		//TourValues diff = instance.getInsertDeltaValues(newSpot) - instance.getDeleteDeltaValues(oldSpot);
		
		//if (instance.isValid(diff)) {
		    
		    
		//}
	    //}
	    
	}
    
	// if satisfaction did not increase and we do not want worse solutions, continue random search
	// if (!alwaysApply && diff.satisfaction < 0) return false;
	
         //remove the spot from the tour, and introduce the new one with the method 
	
	// return false if no spot with an improvement was found
	if(maxSatisfaction== 0 || maxWhatToInsert == 0 || maxWhatToRemove == 0)

            return false;
        else 
            return true;
    }
}

bool TwoOPT::performStep (Instance& instance, Config::StepFunction stepFunction, bool alwaysApply)
{
    if ( instance.getTourLength() < 3 ) return false;
    
    if (stepFunction == Config::SF_RANDOM) {
	
	int firstEdge = rand() % (instance.getTourLength() - 1);
	int secondEdge = rand() % (instance.getTourLength() - firstEdge) + firstEdge + 1;
	
	double deltaSatisfaction;
	if (!isValidEdgeExchange(instance, firstEdge, secondEdge, deltaSatisfaction)) {
	    // TODO should we continue a random search to find a valid solution?
	    // but how do we keep track of invalid solutions efficiently??
	    return false;
	}
	
	if (alwaysApply || deltaSatisfaction >= 0) {
	    performEdgeExchange(instance, firstEdge, secondEdge);
	    return true;
	}
	
    } else if (stepFunction == Config::SF_NEXT) {
	
	int tourLength = instance.getTourLength();
	int maxDist = (tourLength + 1) / 2;
	
	// iterate first over the first edge, then the *distance* between the edges
	for (int distance = 2; distance <= maxDist; distance++) {
	    
	    unsigned numEdges = (tourLength % 2 && distance == maxDist) ? (tourLength + 1) / 2 : tourLength + 1;
	    
	    for (int firstEdge = 0; firstEdge < tourLength; firstEdge++) {
		int secondEdge = (firstEdge + distance) % (tourLength + 1);
		
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
	    unsigned lastEdge = (firstEdge == 0) ? tourLength : tourLength + 1;
	    for (int secondEdge = firstEdge + 2; secondEdge < lastEdge; secondEdge++) {
		
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
    TourValues delta = instance.getCrossOverDeltaValues(firstEdge, secondEdge);
    deltaSatisfaction = delta.satisfaction;
    
    return instance.getTotalTime() + delta.tourTime < instance.getProblem().getMaxTime();
}

void TwoOPT::performEdgeExchange(Instance& instance, int firstEdge, int secondEdge)
{
    instance.crossOverEdges(firstEdge, secondEdge);
}
