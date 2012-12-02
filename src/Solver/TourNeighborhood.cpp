#include "TourNeighborhood.h"

#include <vector>

using namespace std;

void NodeInserter::insertNodes(Instance& instance, bool insertBestStep)
{
    instance.insertNodes( insertBestStep ? bestNewNodes : newNodes );
}

void NodeInserter::storeAsBestStep()
{
    bestNewNodes = newNodes;
}



ConsecutiveNodeInserter::ConsecutiveNodeInserter(Environment& env, unsigned int maxk, bool insertUsed)
: NodeInserter(env, maxk, insertUsed)
{
}

void ConsecutiveNodeInserter::prepareStep(Instance& instance, Config::StepFunction stepFunction)
{
}

double ConsecutiveNodeInserter::findRandomInsertNodes(Instance& instance, const TourNodeIndexList& removedNodes)
{

}

double ConsecutiveNodeInserter::findInsertNodes(Instance& instance, const TourNodeIndexList& removedNodes, bool findBestStep)
{

}



RandomNodeInserter::RandomNodeInserter(Environment& env, unsigned int maxk, bool insertUsed)
: NodeInserter(env, maxk, insertUsed)
{
}

void RandomNodeInserter::prepareStep(Instance& instance, Config::StepFunction stepFunction)
{
    nearestSpots = env.getSpotSearch().findNearestSpots(instance, maxk);
}

double RandomNodeInserter::findRandomInsertNodes(Instance& instance, const TourNodeIndexList& removedNodes)
{

}

double RandomNodeInserter::findInsertNodes(Instance& instance, const TourNodeIndexList& removedNodes, bool findBestStep)
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

    nodeInserter.prepareStep(instance, stepFunction);
    
    if (stepFunction == Config::SF_RANDOM) {
	unsigned removeLength = rand() % min(maxRemove,tourLength);
	unsigned removeFirst = rand() % (tourLength - maxRemove + 1);
	
	removedNodes.clear();
	for (int i = removeFirst; i < removeFirst + removeLength; i++) {
	    removedNodes.push_back(make_pair(i, instance.getNode(i)));
	}
	
	instance.deleteNodes(removedNodes);
	
	// find nodes to insert
	double deltaSatisfaction = nodeInserter.findRandomInsertNodes(instance, removedNodes);
	
	// update tour if satisfaction increased
	if (alwaysApply || deltaSatisfaction > 0.0) {
	    nodeInserter.insertNodes(instance, false);
	    return true;
	} else {
	    instance.insertNodes(removedNodes);
	    return false;
	}
	
    } else {
	
	unsigned maxLength = min(maxRemove,tourLength);

	double bestSatisfaction = 0;
	double bestLength = 0;
	double bestFirst = 0;
	
	for (unsigned removeLength = 1; removeLength < maxLength; removeLength++) {
	    
	    for (unsigned removeFirst = 0; removeFirst < tourLength - removeLength + 1; removeFirst++) {
		
		removedNodes.clear();
		for (int i = removeFirst; i < removeFirst + removeLength; i++) {
		    removedNodes.push_back(make_pair(i, instance.getNode(i)));
		}
		
		instance.deleteNodes(removedNodes);
		
		// find nodes to insert
		double deltaSatisfaction = nodeInserter.findInsertNodes(instance, removedNodes, stepFunction == Config::SF_BEST);
		
		if (stepFunction == Config::SF_BEST) {
		    if (deltaSatisfaction > 0.0) {
			nodeInserter.insertNodes(instance, false);
			return true;
		    }
		} else {
		    if (deltaSatisfaction > bestSatisfaction || bestLength == 0) {
			bestFirst = removeFirst;
			bestLength = removeLength;
			bestSatisfaction = deltaSatisfaction;
			nodeInserter.storeAsBestStep();
		    }
		}
		
		instance.insertNodes(removedNodes);
	    }
	    
	}
	
	if (stepFunction == Config::SF_BEST && (alwaysApply || bestSatisfaction > 0)) {
	    for (unsigned i = 0; i < bestLength; i++) {
		instance.deleteNode(bestFirst);
	    }
	    nodeInserter.insertNodes(instance, true);
	    return true;
	}
    }
    
    
    return false;
}



TwoNodesTourExchange::TwoNodesTourExchange(Environment& env, NodeInserter& nodeInserter)
: NearestNodesNeighborhood(env, nodeInserter)
{
    removedNodes.resize(2);
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
    
    nodeInserter.prepareStep(instance, stepFunction);
    
    if (stepFunction == Config::SF_RANDOM) {
	
	// pick two nodes in the tour to replace
	unsigned firstNodeId = rand() % (tourLength);
	unsigned secondNodeId = rand() % (tourLength - 1);
	if (firstNodeId == secondNodeId) secondNodeId++;
	
	if (firstNodeId > secondNodeId) {
	    unsigned tmp = secondNodeId;
	    secondNodeId = firstNodeId;
	    firstNodeId = tmp;
	}
	
	removedNodes[0] = make_pair(firstNodeId,  instance.getNode(firstNodeId));
	removedNodes[1] = make_pair(secondNodeId, instance.getNode(secondNodeId));
	
	instance.deleteNodes(removedNodes);
	
	// find nodes to insert
	double deltaSatisfaction = nodeInserter.findRandomInsertNodes(instance, removedNodes);
	
	// update tour if satisfaction increased
	if (alwaysApply || deltaSatisfaction > 0.0) {
	    nodeInserter.insertNodes(instance, false);
	    return true;
	} else {
	    instance.insertNodes(removedNodes);
	    return false;
	}	
	
    } else {
	
	double bestSatisfaction = 0.0;
	unsigned bestFirst = 0;
	unsigned bestSecond = 0;
	
	for (unsigned firstNodeId = 0; firstNodeId < tourLength - 1; firstNodeId++) {
	 
	    for (unsigned secondNodeId = firstNodeId+1; secondNodeId < tourLength; secondNodeId++) {
		
		// select where to insert new nodes
		removedNodes[0] = make_pair(firstNodeId,  instance.getNode(firstNodeId));
		removedNodes[1] = make_pair(secondNodeId, instance.getNode(secondNodeId));
		
		instance.deleteNodes(removedNodes);
		
		// find nodes to insert
		double deltaSatisfaction = nodeInserter.findInsertNodes(instance, removedNodes, stepFunction == Config::SF_BEST);
		
		if (stepFunction == Config::SF_BEST) {
		    if (deltaSatisfaction > 0.0) {
			nodeInserter.insertNodes(instance, false);
			return true;
		    }
		} else {
		    if (deltaSatisfaction > bestSatisfaction || bestFirst == bestSecond) {
			bestFirst = firstNodeId;
			bestSecond = secondNodeId;
			bestSatisfaction = deltaSatisfaction;
			nodeInserter.storeAsBestStep();
		    }
		}
		
		instance.insertNodes(removedNodes);
	    }
	    
	}
	
	if (stepFunction == Config::SF_BEST && (alwaysApply || bestSatisfaction > 0)) {
	    instance.deleteNode(bestSecond);
	    instance.deleteNode(bestFirst);
	    nodeInserter.insertNodes(instance, true);
	    return true;
	}
	
    }
    
    return false;
}
