#include "Neighborhood.h"

#include <set>
#include <algorithm>

#include <iostream>
using namespace std;

Neighborhood::Neighborhood(Environment& env)
: env(env)
{
}

unsigned OneOPT::findRandomMethod (Instance& instance, unsigned nodeToUpdate, unsigned spot, unsigned method, double &deltaSatisfaction)
{
    //check all methods for the first suitable 
    //method which gives some improvement, if found return the index
    unsigned numMethods = instance.getProblem().getSpot(spot).getMethods().size();
    
    for ( unsigned i = 0; i < numMethods; i++) 
    {
	unsigned m = (method + i) % numMethods;
	
        TourNode newNode(spot, m);
        TourValues diff = instance.getUpdateDeltaValues(nodeToUpdate,newNode);
        
        if (instance.isValid(diff)) {
	    deltaSatisfaction = diff.satisfaction;
            return m;
	}
        
    }
    //this means that none of the methods gives any improvement
    return method;
}

void OneOPT::performReplaceNode (Instance& instance, unsigned int nodeToUpdate, unsigned spotToInsert, unsigned int method)
{
    instance.updateNode(nodeToUpdate, spotToInsert, method);
}


bool OneOPT::performStep (Instance& instance, Config::StepFunction stepFunction, bool alwaysApply)
{
    if (stepFunction == Config::SF_RANDOM) {

        return performRandomStep(instance,alwaysApply);

    } else if (stepFunction == Config::SF_NEXT) {
        
        return performNextStep(instance, alwaysApply);
    
    } else {
        
        return performMaxStep(instance, alwaysApply);
    }
}

bool OneOPT::performNextStep(Instance& instance, bool alwaysApply){
 
    unsigned maxk = instance.getProblem().getSpots().size() - 1;

    //for each node in the tour
    for (unsigned index : instance.getRatioSortedNodes()) {

        NearestSpotList nearest = env.getSpotSearch().findNearestSpots (instance, index, maxk, true);
        //check for all unused spots for the one which gives the best improvement
        for (auto & nearSpot : nearest) {
                
            unsigned spotId = nearSpot.second;
            const Spot& nearestspot = env.getProblem().getSpot(spotId);
        
            unsigned m = 0;
            for (auto & metod : nearestspot.getMethods()) {
            
                TourNode n (spotId, m);
                TourValues diff = instance.getUpdateDeltaValues (index , n);
                if (instance.isValid (diff) && diff.satisfaction > 0) {
                   // if we found some improvement, make the node exchange and return 
                    performReplaceNode(instance,index,n.spot,m);
                    return true;
                }
                ++m;
            }
        }        
    }
   
    return false;
}

bool OneOPT::performRandomStep(Instance& instance, bool alwaysApply){
    set<unsigned> unusedSpots = instance.getUnusedSpotIDs();

    unsigned spotWhereToInsert;
    // repeat
    while (unusedSpots.size() >= 1) {
	// pick random spot where to insert
	spotWhereToInsert = rand() % instance.getTourLength();

	// spot = find random unused spot
	unsigned unusedRandomSpot = rand() % unusedSpots.size();
	set<unsigned>::iterator it (unusedSpots.begin());
	advance (it, unusedRandomSpot);
	// method = find random method of spot
	unsigned m = rand() % instance.getProblem().getSpot (*it).getMethods().size();
	
	// find feasible solution
	double deltaSatisfaction;
	unsigned otherMethods = findRandomMethod (instance, spotWhereToInsert, *it, m, deltaSatisfaction);

	if (m == otherMethods) {
	    //this means none of the methods is feasable
	    unusedSpots.erase (it);

	} else {
	    
	    if ( alwaysApply || (deltaSatisfaction) > 0.0) {
		//this means we incereased the total satisfaction
		// remove old spot from tour
		// insert new spot + method into tour
		performReplaceNode(instance,spotWhereToInsert,*it, m);
		return true;

	    } else {
		// if satisfaction did not increase and we do not want worse solutions, continue random search
		return false;
	    }
	}
	
    }
    // repeat select random spot until unusedSpots.size()

    return false;
    
}
bool OneOPT::performMaxStep(Instance& instance, bool alwaysApply){
    double maxSatisfaction = 0;
    unsigned maxWhatToRemove = -1;
    unsigned maxWhatToInsert = -1;
    unsigned method = 0;

    unsigned maxk = instance.getProblem().getSpots().size() - 1;
    
    //for each node in the tour
    unsigned tourLength = instance.getTourLength();
    for (unsigned index = 0; index < tourLength; index++) {

        NearestSpotList nearest = env.getSpotSearch().findNearestSpots (instance, index, maxk, true);
        //check for all unused spots for the one which gives the best improvement
        for (auto & nearSpot : nearest) {
                
            unsigned spotId = nearSpot.second;
            unsigned m = 0;
            const Spot& nearestspot = env.getProblem().getSpot(spotId);
        
            for (auto & meth : nearestspot.getMethods()) {
            
                TourNode n (spotId, m);
                TourValues diff = instance.getUpdateDeltaValues (index , n);
                if (instance.isValid(diff) && (diff.satisfaction > maxSatisfaction || maxWhatToInsert == -1)) {
                    //keep track of the maximal improvement
                    maxSatisfaction = diff.satisfaction;
                    maxWhatToInsert = n.spot;
                    maxWhatToRemove = index;
                    method = m;

                }
                ++m;
            }
        }
    }

    // return false if no spot with an improvement was found
    if ((!alwaysApply && maxSatisfaction < 0) || maxWhatToInsert == -1 || maxWhatToRemove == -1)
        return false;
    else {
        //some improvement was found
        //remove the spot from the tour, and introduce the new one with the method
        performReplaceNode(instance,maxWhatToRemove,maxWhatToInsert,method);
        return true;
    }
}


bool EdgeTwoOPT::performStep (Instance& instance, Config::StepFunction stepFunction, bool alwaysApply)
{
    int tourLength = instance.getTourLength();

    if ( tourLength < 3 ) return false;

    if (stepFunction == Config::SF_RANDOM) {
	
	// First thing we need to do is to build up a list of all valid candidates, so that we can randomly choose a 
	// *valid* exchange. The list should not be that long for a nearly-optimal solution anyway.
	
	std::vector<std::pair<unsigned,unsigned>> candidates;
	
	for (int firstEdge = 0; firstEdge < tourLength - 1; firstEdge++) {
	    unsigned lastEdge = (firstEdge == 0) ? tourLength : tourLength + 1;
	    for (int secondEdge = firstEdge + 2; secondEdge < lastEdge; secondEdge++) {
		
		double deltaSatisfaction;
		
		if (isValidEdgeExchange(instance, firstEdge, secondEdge, deltaSatisfaction)) {
		    
		    // TODO if !alwaysApply and deltaSatisfaction < 0, do not add?
		    //      but this would not be consistent with the definition of the algorithms
		    if (!alwaysApply && deltaSatisfaction < 0) continue;
		    		    
		    candidates.push_back( make_pair(firstEdge, secondEdge) );
		}
	    }
	}
	
	if (candidates.empty()) return false;

	// Pick a random pair of edges
	int r = rand() % candidates.size();
	pair<unsigned,unsigned> c = candidates[r];
	unsigned firstEdge = c.first;
	unsigned secondEdge = c.second;
	
	// We only have valid pairs, no need to check again if not necessary
	double deltaSatisfaction = 1.0;
	if (alwaysApply || isValidEdgeExchange(instance, firstEdge, secondEdge, deltaSatisfaction)) {
	    if (deltaSatisfaction >= 0) {
		performEdgeExchange(instance, firstEdge, secondEdge);
		return true;
	    }
	}
	
    } else if (stepFunction == Config::SF_NEXT) {
	
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

bool EdgeTwoOPT::isValidEdgeExchange(Instance& instance, int firstEdge, int secondEdge, double& deltaSatisfaction)
{
    TourValues delta = instance.getCrossOverDeltaValues(firstEdge, secondEdge);
    deltaSatisfaction = delta.satisfaction;
    
    return instance.getTotalTime() + delta.tourTime < instance.getProblem().getMaxTime();
}

void EdgeTwoOPT::performEdgeExchange(Instance& instance, int firstEdge, int secondEdge)
{
    instance.crossOverEdges(firstEdge, secondEdge);
}
