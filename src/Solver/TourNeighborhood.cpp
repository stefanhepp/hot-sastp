#include "TourNeighborhood.h"

#include <vector>

using namespace std;

NearestNodesNeighborhood::NearestNodesNeighborhood(Environment& env, unsigned int maxk, bool insertConsecutive, bool insertUsed)
: Neighborhood(env), maxk(maxk), insertConsecutive(insertConsecutive), insertUsed(insertUsed)
{
}


NearestTourExchange::NearestTourExchange(Environment& env, unsigned int maxRemove, unsigned int maxk, bool insertConsecutive, bool insertUsed)
: NearestNodesNeighborhood(env, maxk, insertConsecutive, insertUsed), maxRemove(maxRemove)
{
}

std::string NearestTourExchange::getName() const
{
    // TODO add option values to name
    return "Nearest-Tour-exchange"; 
}

bool NearestTourExchange::performStep(Instance& instance, Config::StepFunction stepFunction, bool alwaysApply)
{
    unsigned tourLength = instance.getTourLength();
    if (tourLength < 1) return false;

    
    
    
    return false;
}



TwoNodesTourExchange::TwoNodesTourExchange(Environment& env, unsigned int maxk, bool insertConsecutive, bool insertUsed)
: NearestNodesNeighborhood(env, maxk, insertConsecutive, insertUsed)
{
}

std::string TwoNodesTourExchange::getName() const
{
    // TODO add option values to name
    return "2-Nodes-exchange";
}

bool TwoNodesTourExchange::performStep(Instance& instance, Config::StepFunction stepFunction, bool alwaysApply)
{
    unsigned tourLength = instance.getTourLength();
    if (tourLength < 2) return false;
    
    if (stepFunction == Config::SF_RANDOM) {
	
	// pick two nodes in the tour to replace
	unsigned firstNodeId = rand() % (tourLength);
	unsigned secondNodeId = rand() % (tourLength - 1);
	if (firstNodeId == secondNodeId) secondNodeId++;
	
	// pick new nodes to insert (depending on how to insert)
	if (insertConsecutive) {
	    // pick node where to insert
	    unsigned insertBefore = rand() % (tourLength + 1);
	    
	    // pick from k nearest nodes at insertion point
	    
	} else {
	    // pick from k nearest nodes, insert at nearest node
	    
	    
	}
	
	// pick methods to insert
	
	
	
    } else {
	
	for (unsigned firstNodeId = 0; firstNodeId < tourLength - 1; firstNodeId++) {
	 
	    for (unsigned secondNodeId = firstNodeId+1; secondNodeId < tourLength; secondNodeId++) {
		
		// select where to insert new nodes
		
		
		
	    }
	    
	}
	
	if (stepFunction == Config::SF_BEST) {
	 
	    
	}
	
    }
    
    return false;
}
