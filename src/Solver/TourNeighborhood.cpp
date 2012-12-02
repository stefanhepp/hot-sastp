#include "TourNeighborhood.h"

#include <vector>

using namespace std;

ConsecutiveNodeInserter::ConsecutiveNodeInserter(unsigned int maxk, bool insertUsed)
: NodeInserter(maxk, insertUsed)
{
}

RandomNodeInserter::RandomNodeInserter(unsigned int maxk, bool insertUsed)
: NodeInserter(maxk, insertUsed)
{
}





NearestNodesNeighborhood::NearestNodesNeighborhood(Environment& env, NodeInserter& nodeInserter)
: Neighborhood(env), nodeInserter(nodeInserter)
{
}


NearestTourExchange::NearestTourExchange(Environment& env, unsigned int maxRemove, NodeInserter& nodeInserter)
: NearestNodesNeighborhood(env, nodeInserter), maxRemove(maxRemove)
{
    removedNodes.reserve(maxRemove);
    insertNodes.reserve(maxRemove*2);
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

    if (stepFunction == Config::SF_RANDOM) {
	unsigned removeLength = rand() % min(maxRemove,tourLength);
	unsigned removeFirst = rand() % (tourLength - maxRemove + 1);
	
	
	
    } else {
	
    }
    
    
    return false;
}



TwoNodesTourExchange::TwoNodesTourExchange(Environment& env, NodeInserter& nodeInserter)
: NearestNodesNeighborhood(env, nodeInserter)
{
    removedNodes.resize(2);
    insertNodes.reserve(5);
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
