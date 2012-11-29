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

void OneOPT::performReplaceNode (Instance& instance, unsigned int whereToInsert, unsigned whatToInsert, unsigned int method)
{
    instance.deleteNode (whereToInsert);
    instance.insertNode (whereToInsert, whatToInsert, method);
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
 
    set<unsigned> unusedSpots = instance.getUnusedSpotIDs();
    
    //for each node in the tour
    unsigned index = 0;
    for (auto & node : instance.getTour()) {

        NearestSpotList nearest = env.getSpotSearch().findNearestSpots (instance, index, unusedSpots.size());
        //check for all unused spots for the one which gives the best improvement
        for (auto & nearSpot : nearest) {
                
            unsigned spotId = nearSpot.second;
            const Spot& nearestspot = env.getProblem().getSpot(spotId);
        
            unsigned m = 0;
            for (auto & metod : nearestspot.getMethods()) {
            
                TourNode n (spotId, m);
                TourValues diff = instance.getInsertDeltaValues (index , n) - instance.getDeleteDeltaValues (index);
                if (instance.isValid (diff) && diff.satisfaction > 0) {
                   // if we found some improvement, make the node exchange and return 
                    performReplaceNode(instance,index,n.spot,m);
                    return true;
                }
                ++m;
            }
        }
    ++index;
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
            // check if solution is feasible

            TourNode newNode (*it, m);

            TourValues diff = instance.getInsertDeltaValues (spotWhereToInsert, newNode);
            diff -= instance.getDeleteDeltaValues (spotWhereToInsert);

            if (instance.isValid (diff)) {

                if ( (diff.satisfaction) > 0.0) {
                    //this means we incereased the total satisfaction
                    // remove old spot from tour
                    // insert new spot + method into tour
                    performReplaceNode(instance,spotWhereToInsert,*it, m);
                    return true;

                } else
                    // if satisfaction did not increase and we do not want worse solutions, continue random search
                    if (!alwaysApply && diff.satisfaction < 0) return false;


            } else {

                unsigned otherMethods = findRandomMethod (instance, spotWhereToInsert, *it, m);
                std::set<unsigned>::iterator tmp;
                // check other methods if any one is feasible
                if (m == otherMethods) {
                    //this means none of the methods is feasable
                    tmp = it;
                    ++tmp;
                    unusedSpots.erase (it);
                    it = tmp;

                } else {
                    performReplaceNode(instance,spotWhereToInsert,*it,otherMethods);
                    return true;
                }
            }
        }
        // repeat select random spot until unusedSpots.size()

        return false;
    
}
bool OneOPT::performMaxStep(Instance& instance, bool alwaysApply){
    //set of unused spotID's
    set<unsigned> unusedSpots = instance.getUnusedSpotIDs();
    
    double maxSatisfaction = 0;
    unsigned maxWhatToRemove = 1;
    unsigned maxWhatToInsert = 1;
    unsigned method = 0;
    //for each node in the tour
    unsigned index = 0;
    for (auto & node : instance.getTour()) {

        NearestSpotList nearest = env.getSpotSearch().findNearestSpots (instance, index, unusedSpots.size());
        //check for all unused spots for the one which gives the best improvement
        for (auto & nearSpot : nearest) {
                
            unsigned spotId = nearSpot.second;
            unsigned m = 0;
            const Spot& nearestspot = env.getProblem().getSpot(spotId);
        
            for (auto & meth : nearestspot.getMethods()) {
            
                TourNode n (spotId, m);
                TourValues diff = instance.getInsertDeltaValues (index , n) - instance.getDeleteDeltaValues (index);
                if (instance.isValid (diff) && diff.satisfaction > maxSatisfaction) {
                    //keep track of the maximal improvement
                    maxSatisfaction = diff.satisfaction;
                    maxWhatToInsert = n.spot;
                    maxWhatToRemove = index;
                    method = m;

                }
                ++m;
            }
        }
    ++index;
    }

    // return false if no spot with an improvement was found
    if (maxSatisfaction == 0 || maxWhatToInsert == 0 || maxWhatToRemove == 0)
        return false;
    else {
        //some improvement was found
        //remove the spot from the tour, and introduce the new one with the method
        performReplaceNode(instance,maxWhatToRemove,maxWhatToInsert,method);
        return true;
    }
}

bool TwoOPT::performStep (Instance& instance, Config::StepFunction stepFunction, bool alwaysApply)
{
    if (instance.getTourLength() < 3) return false;

    if (stepFunction == Config::SF_RANDOM) {

        int firstEdge = rand() % (instance.getTourLength() - 1);
        int secondEdge = rand() % (instance.getTourLength() - firstEdge) + firstEdge + 1;

        double deltaSatisfaction;
        if (!isValidEdgeExchange (instance, firstEdge, secondEdge, deltaSatisfaction)) {
            // TODO should we continue a random search to find a valid solution?
            // but how do we keep track of invalid solutions efficiently??
            return false;
        }

        if (alwaysApply || deltaSatisfaction >= 0) {
            performEdgeExchange (instance, firstEdge, secondEdge);
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
                if (isValidEdgeExchange (instance, firstEdge, secondEdge, deltaSatisfaction)) {

                    if (alwaysApply || deltaSatisfaction >= 0) {

                        performEdgeExchange (instance, firstEdge, secondEdge);

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

                if (isValidEdgeExchange (instance, firstEdge, secondEdge, deltaSatisfaction)) {

                    if (deltaSatisfaction > bestSatisfaction || (bestFirst == bestSecond)) {
                        bestSatisfaction = deltaSatisfaction;
                        bestFirst = firstEdge;
                        bestSecond = secondEdge;
                    }

                }
            }
        }

        if (alwaysApply || bestSatisfaction >= 0) {

            performEdgeExchange (instance, bestFirst, bestSecond);

            return true;
        }
    }


bool TwoOPT::performStep (Instance& instance, Config::StepFunction stepFunction, bool alwaysApply)
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
