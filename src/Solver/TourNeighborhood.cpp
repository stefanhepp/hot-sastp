#include "TourNeighborhood.h"

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

NodeInserter::NodeInserter(Environment& env, unsigned int maxk, bool insertUsed)
: env(env), spotsearch(env.getSpotSearch()), maxk(maxk), insertUsed(insertUsed)
{
    skipMethodOnly = true;
}


void NodeInserter::insertNodes(Instance& instance, bool insertBestStep)
{
    instance.insertNodes( insertBestStep ? bestNewNodes : newNodes );
}

void NodeInserter::storeAsBestStep()
{
    bestNewNodes = newNodes;
}

unsigned int NodeInserter::addNewNode(const Instance& instance, unsigned int insertAt, TourNode node, bool insertBest)
{
    unsigned insertIndex = 0;
    
    if (insertAt == -1) insertAt = instance.getTourLength();
    
    int index = -1;
    for (auto& node : newNodes) {
	index++;
	
	// TODO be more intelligent, insert in order so that costs are minimized
    
	unsigned nodeIndex = node.first;
	if (nodeIndex > insertAt) break;
	
	insertIndex = index;
    }
    
    newNodes.insert( newNodes.begin() + insertIndex, make_pair(insertAt, node) );
    
    return insertIndex;
}

TourValues NodeInserter::getDeltaInsertValues(const Instance& instance)
{
    TourValues delta;

    // Iterate over newNodes and 'patch' into tour
    
    int lastInsertIndex = -1;
    const Spot* lastInsertSpot = 0;
    
    for (auto& node : newNodes) {
	
	unsigned insertIndex = node.first;
	unsigned insertSpotId = node.second.spot;
	unsigned insertMethodId = node.second.method;
	const Spot& insertSpot = instance.getProblem().getSpot(insertSpotId);
	
	// ok, first check if we need to add an edge from the last inserted node
	if (lastInsertIndex != insertIndex && lastInsertSpot) {
	    delta += instance.getTravelDeltaValues(*lastInsertSpot, instance.getSpot(lastInsertIndex));
	}
	
	// now check if we go from a node in the tour to a new node
	if (lastInsertIndex != insertIndex) {
	    // in this case, remove the edge where we insert the node, add the costs for the travel to new node
	    const Spot& lastSpot = instance.getSpot(insertIndex-1);
	    delta -= instance.getTravelDeltaValues(lastSpot, instance.getSpot(insertIndex));
	    delta += instance.getTravelDeltaValues(lastSpot, insertSpot);
	} else {
	    // add an edge from the last inserted spot to the next inserted spot
	    delta += instance.getTravelDeltaValues(*lastInsertSpot, insertSpot);
	}
	
	// add the costs for the new spot
	delta += insertSpot.getMethod(insertMethodId);
	
	// remember last inserted spot to add next edge, continue
	lastInsertSpot = &insertSpot;
	lastInsertIndex = insertIndex;
    }
    
    // finally add the edge from the last inserted node to its next node
    if (lastInsertSpot) {
	delta += instance.getTravelDeltaValues(*lastInsertSpot, instance.getSpot(lastInsertIndex));
    }
    
    return delta;
}

MethodRatioList NodeInserter::getMethodRatios(const Instance& instance, NearestSpotList& nearestSpots, const TourNodeIndexList& removedNodes)
{
    MethodRatioList ratios;
    ratios.reserve(nearestSpots.size());
    
    vector<unsigned> remove;
    
    unsigned index = -1;
    for (auto& node : nearestSpots) {
	index++;
	
	unsigned nodeId = node.first;
	unsigned spotId = node.second;
	
	const Spot& spot = instance.getProblem().getSpot(spotId);

	unsigned methodId;
	double ratio = instance.getBestMethodRatio(nodeId, spot, methodId);
	
	if (skipNode(removedNodes, spotId, methodId)) {
	    remove.push_back(index);
	} else {
	    ratios.push_back(make_pair(methodId, ratio));
	}
    }
    
    for (int i = remove.size() - 1; i >= 0; --i) {
	nearestSpots.erase( nearestSpots.begin() + remove[i] );
    }
    
    return ratios;
}

unsigned int NodeInserter::findBestNearestNode(const MethodRatioList& ratioList)
{
    double bestRatio = ratioList[0].second;
    unsigned bestIndex = 0;
    
    for (unsigned int i = 1; i < ratioList.size(); i++) {
	if (ratioList[i].second > bestRatio) {
	    bestRatio = ratioList[i].second;
	    bestIndex = i;
	}
    }
    
    return bestIndex;
}

double NodeInserter::selectNewNodes(Instance& instance, NearestSpotList& nearestSpots, MethodRatioList *ratios, 
				      const TourNodeIndexList& removedNodes, bool findBestStep)
{
    // add random nodes to tour as long as it is possible
    TourValues lastDelta;
    
    newNodes.clear();
    
    while (!nearestSpots.empty()) {
	
	unsigned r = ratios ? findBestNearestNode(*ratios) : rand() % nearestSpots.size();
	
	//  pick a method
	unsigned nodeId = nearestSpots[r].first;
	unsigned spotId = nearestSpots[r].second;
	const Spot& spot = instance.getProblem().getSpot(spotId);
	
	unsigned methodId = ratios ? ratios->at(r).first : rand() % spot.getMethods().size();
	
	if (!ratios && skipNode(removedNodes, spotId, methodId)) {
	    if (skipMethodOnly) {
		methodId = (methodId + 1) % spot.getMethods().size();
	    } else {
		continue;
	    }
	}
	
	unsigned pos = addNewNode(instance, nodeId, TourNode(spotId, methodId), false);
	
	TourValues delta = getDeltaInsertValues(instance);
	
	if (instance.isValid( delta )) {
	    nearestSpots.erase( nearestSpots.begin() + r );
	    if (ratios) ratios->erase( ratios->begin() + r );		
	    lastDelta = delta;
	} else {
	    newNodes.erase(newNodes.begin() + pos);
	    
	    // finished inserting nodes, not much we can do to improve for best step, or is there?
	    break;
	}
    }
    
    return lastDelta.satisfaction;
}


bool NodeInserter::skipNode(const TourNodeIndexList& removedNodes, unsigned int spotId, unsigned int methodId)
{
    if (insertUsed) return false;
    
    for (auto& p : removedNodes) {
	if (p.second.spot == spotId) {
	    if (!skipMethodOnly || p.second.method == methodId) return true;
	}
    }
    
    return false;
}



SearchNodeInserter::SearchNodeInserter(Environment& env, AbstractSearch& search)
: NodeInserter(env, 0, true), search(search), newTour(env.getProblem()), bestNewTour(env.getProblem())
{
}

void SearchNodeInserter::prepareStep(Instance& instance, Config::StepFunction stepFunction)
{
}

double SearchNodeInserter::findRandomInsertNodes(Instance& instance, const TourNodeIndexList& removedNodes, double minSatisfaction)
{
    return runSearch(instance);
}

double SearchNodeInserter::findInsertNodes(Instance& instance, const TourNodeIndexList& removedNodes, double minSatisfaction, bool findBestStep)
{
    return runSearch(instance);
}

double SearchNodeInserter::runSearch(Instance& instance)
{
    bool printSteps = env.setPrintSteps(false);
    
    search.reset(instance);
    search.run();
    
    newTour = search.getInstance();
    
    env.setPrintSteps(printSteps);
    
    return newTour.getTotalSatisfaction() - instance.getTotalSatisfaction();
}




ConsecutiveNodeInserter::ConsecutiveNodeInserter(Environment& env, unsigned int maxk, bool insertUsed)
: NodeInserter(env, maxk, insertUsed)
{
}

void ConsecutiveNodeInserter::prepareStep(Instance& instance, Config::StepFunction stepFunction)
{
}

double ConsecutiveNodeInserter::findRandomInsertNodes(Instance& instance, const TourNodeIndexList& removedNodes, double minSatisfaction)
{
    // pick random insertion point
    unsigned insertAt = rand() % (instance.getTourLength() + 1) - 1;
    
    NearestSpotList nearestSpots = spotsearch.findNearestSpots(instance, insertAt, maxk, true);
    
    return selectNewNodes(instance, nearestSpots, NULL, removedNodes, false);
}

double ConsecutiveNodeInserter::findInsertNodes(Instance& instance, const TourNodeIndexList& removedNodes, double minSatisfaction, bool findBestStep)
{
    int tourLength = instance.getTourLength();
    
    // first: best method-id of spot, second: satisfaction/time
    MethodRatioList ratios;
    ratios.reserve(maxk);
    
    double bestSatisfaction = 0;
    TourNodeIndexList localBestNewNodes;
    
    for (int insertAt = -1; insertAt < tourLength; insertAt++) {
	
	NearestSpotList nearestSpots = spotsearch.findNearestSpots(instance, insertAt, maxk, true);
	
	// get best method and ratios per nearest spots
	ratios = getMethodRatios(instance, nearestSpots, removedNodes);
	
	double deltaSatisfaction = selectNewNodes(instance, nearestSpots, &ratios, removedNodes, findBestStep);
	
	if (!findBestStep && deltaSatisfaction > minSatisfaction) {
	    return deltaSatisfaction;
	} else if (findBestStep && (insertAt == -1 || deltaSatisfaction > bestSatisfaction)) {
	    localBestNewNodes = newNodes;
	    bestSatisfaction = deltaSatisfaction;
	}
    }
    
    if (findBestStep) {
	newNodes = localBestNewNodes;
	return bestSatisfaction;
    }
    
    return 0;
}



RandomNodeInserter::RandomNodeInserter(Environment& env, unsigned int maxk, bool insertUsed)
: NodeInserter(env, maxk, insertUsed)
{
}

void RandomNodeInserter::prepareStep(Instance& instance, Config::StepFunction stepFunction)
{
    // Does not work that way as nodes are removed from the instance, would require
    // lots of special handling (fix up node indices, handle removed nodes, ..)
    //nearestSpots = env.getSpotSearch().findNearestTourSpots(instance, maxk, true);
}

double RandomNodeInserter::findRandomInsertNodes(Instance& instance, const TourNodeIndexList& removedNodes, double minSatisfaction)
{
    NearestSpotList nearestSpots = env.getSpotSearch().findNearestTourSpots(instance, maxk, true);
    
    return selectNewNodes(instance, nearestSpots, NULL, removedNodes, false);
}

double RandomNodeInserter::findInsertNodes(Instance& instance, const TourNodeIndexList& removedNodes, double minSatisfaction, bool findBestStep)
{
    int tourLength = instance.getTourLength();
    
    // first: best method-id of spot, second: satisfaction/time
    MethodRatioList ratios;
    ratios.reserve(maxk);
    
    NearestSpotList nearest = env.getSpotSearch().findNearestTourSpots(instance, maxk, true);
    
    // get best method and ratios per nearest spots
    ratios = getMethodRatios(instance, nearest, removedNodes);
    
    return selectNewNodes(instance, nearest, &ratios, removedNodes, findBestStep);
}




NearestNodesNeighborhood::NearestNodesNeighborhood(Environment& env, NodeInserter& nodeInserter)
: Neighborhood(env), nodeInserter(nodeInserter)
{
}


NearestTourExchange::NearestTourExchange(Environment& env, unsigned minRemove, unsigned int maxRemove, NodeInserter& nodeInserter)
: NearestNodesNeighborhood(env, nodeInserter), minRemove(minRemove), maxRemove(maxRemove), lastFirstNode(0)
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
    if (tourLength <= minRemove) return false;

    nodeInserter.prepareStep(instance, stepFunction);
    
    if (stepFunction == Config::SF_RANDOM) {
	unsigned removeLength = rand() % min(maxRemove - minRemove + 1, tourLength - minRemove) + minRemove;
	unsigned removeFirst = rand() % max(tourLength - removeLength + 1, 1u);
	
	removedNodes.clear();
	for (int i = removeFirst; i < removeFirst + removeLength; i++) {
	    removedNodes.push_back(make_pair(i, instance.getNode(i)));
	}
	
	double minSatisfaction = instance.getTotalSatisfaction();
	
	instance.deleteNodes(removedNodes);
	
	minSatisfaction -= instance.getTotalSatisfaction() - 0.005;
	
	// find nodes to insert
	double deltaSatisfaction = nodeInserter.findRandomInsertNodes(instance, removedNodes, minSatisfaction);
	
	// update tour if satisfaction increased
	if (alwaysApply || deltaSatisfaction > minSatisfaction) {
	    nodeInserter.insertNodes(instance, false);
	    return true;
	} else {
	    instance.insertNodes(removedNodes);
	    return false;
	}
	
    } else {
	
	unsigned maxLength = min(maxRemove,tourLength);

	double bestSatisfaction = 0;
	double bestMinSatisfaction = 0;
	double bestLength = 0;
	double bestFirst = 0;
	
	for (unsigned removeLength = 1; removeLength < maxLength; removeLength++) {
	    
	    unsigned maxFirst = tourLength - removeLength + 1;
	    unsigned removeFirst = lastFirstNode % maxFirst;
	    for (unsigned i=0; i < maxFirst; i++, removeFirst = (removeFirst+1)%(maxFirst) ) {
		
		removedNodes.clear();
		for (int i = removeFirst; i < removeFirst + removeLength; i++) {
		    removedNodes.push_back(make_pair(i, instance.getNode(i)));
		}
		
		double minSatisfaction = instance.getTotalSatisfaction();
		
		instance.deleteNodes(removedNodes);
		
		minSatisfaction -= instance.getTotalSatisfaction() - 0.005;
		
		// find nodes to insert
		double deltaSatisfaction = nodeInserter.findInsertNodes(instance, removedNodes, minSatisfaction, stepFunction == Config::SF_BEST);
		
		if (stepFunction == Config::SF_NEXT) {
		    if (deltaSatisfaction > minSatisfaction) {
			nodeInserter.insertNodes(instance, false);
			lastFirstNode = removeFirst;
			return true;
		    }
		} else {
		    if (deltaSatisfaction > bestSatisfaction || bestLength == 0) {
			bestFirst = removeFirst;
			bestLength = removeLength;
			bestSatisfaction = deltaSatisfaction;
			bestMinSatisfaction = minSatisfaction;
			nodeInserter.storeAsBestStep();
		    }
		}
		
		instance.insertNodes(removedNodes);
	    }
	    
	}
	
	if (stepFunction == Config::SF_BEST && (alwaysApply || bestSatisfaction > bestMinSatisfaction)) {
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
: NearestNodesNeighborhood(env, nodeInserter), lastFirstNode(0)
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
	
	double minSatisfaction = instance.getTotalSatisfaction();
	
	instance.deleteNodes(removedNodes);
	
	minSatisfaction -= instance.getTotalSatisfaction() - 0.005;
	
	// find nodes to insert
	double deltaSatisfaction = nodeInserter.findRandomInsertNodes(instance, removedNodes, minSatisfaction);
	
	// update tour if satisfaction increased
	if (alwaysApply || deltaSatisfaction > minSatisfaction) {
	    nodeInserter.insertNodes(instance, false);
	    return true;
	} else {
	    instance.insertNodes(removedNodes);
	    return false;
	}	
	
    } else {
	
	double bestSatisfaction = 0;
	double bestMinSatisfaction = 0;
	unsigned bestFirst = 0;
	unsigned bestSecond = 0;
	
	unsigned firstNodeId = lastFirstNode % (tourLength - 1);
	for (unsigned i=0; i < tourLength - 1; i++, firstNodeId = (firstNodeId+1)%(tourLength-1) ) {
	 
	    for (unsigned secondNodeId = firstNodeId+1; secondNodeId < tourLength; secondNodeId++) {
		
		// select where to insert new nodes
		removedNodes[0] = make_pair(firstNodeId,  instance.getNode(firstNodeId));
		removedNodes[1] = make_pair(secondNodeId, instance.getNode(secondNodeId));
		
		double minSatisfaction = instance.getTotalSatisfaction();
		
		instance.deleteNodes(removedNodes);
		
		minSatisfaction -= instance.getTotalSatisfaction() - 0.005;
		
		// find nodes to insert
		double deltaSatisfaction = nodeInserter.findInsertNodes(instance, removedNodes, minSatisfaction, stepFunction == Config::SF_BEST);
		
		if (stepFunction == Config::SF_NEXT) {
		    if (deltaSatisfaction > minSatisfaction) {
			nodeInserter.insertNodes(instance, false);
			return true;
		    }
		} else {
		    if (deltaSatisfaction > bestSatisfaction || bestFirst == bestSecond) {
			bestFirst = firstNodeId;
			bestSecond = secondNodeId;
			bestSatisfaction = deltaSatisfaction;
			bestMinSatisfaction = minSatisfaction;
			nodeInserter.storeAsBestStep();
		    }
		}
		
		instance.insertNodes(removedNodes);
	    }
	    
	}
	
	if (stepFunction == Config::SF_BEST && (alwaysApply || bestSatisfaction > bestMinSatisfaction)) {
	    instance.deleteNode(bestSecond);
	    instance.deleteNode(bestFirst);
	    nodeInserter.insertNodes(instance, true);
	    return true;
	}
	
    }
    
    return false;
}
