#ifndef TOURNEIGHBORHOOD_H
#define TOURNEIGHBORHOOD_H

#include "Support/Instance.h"
#include "Support/Environment.h"
#include "Solver/Neighborhood.h"

#include <vector>
#include <set>

/*
 * This file contains neighborhoods that work on whole subtours
 */

class NodeInserter {
public:
    NodeInserter(unsigned int maxk, bool insertUsed) : maxk(maxk), insertUsed(insertUsed) {}
    
protected:
    unsigned maxk;
    bool insertUsed;    
};

class ConsecutiveNodeInserter : public NodeInserter {
public:
    ConsecutiveNodeInserter(unsigned int maxk, bool insertUsed);
    
protected:
    
};

class RandomNodeInserter : public NodeInserter {
public:
    RandomNodeInserter(unsigned int maxk, bool insertUsed);
    
protected:
    
};

/**
 * Base class for nearest nodes exchange neighborhoods, provides some common functions
 */
class NearestNodesNeighborhood : public Neighborhood
{
public:
    NearestNodesNeighborhood(Environment& env, NodeInserter& nodeInserter);

protected:
    NodeInserter& nodeInserter;
    
    std::vector<unsigned> removedNodes;
    // pair of <tournode after insert point, tournode to insert>
    std::vector<std::pair<unsigned,TourNode>> insertNodes;
    
    double findRandomInsertNodes(const NearestSpotList& searchSpots);
    
    double findInsertNodes(const NearestSpotList& searchSpots, bool searchAll);
};


/**
 * This neighborhood removes up to k nodes from a tour (either consecutive or not) from a tour and
 * inserts as many nearest tour neighbors as possible back into the tour.
 */
class NearestTourExchange : public NearestNodesNeighborhood
{
    unsigned maxRemove;
    
public:
    NearestTourExchange(Environment& env, unsigned int maxRemove, NodeInserter& nodeInserter);
    
    virtual std::string getName() const;
    
    virtual bool performStep(Instance& instance, Config::StepFunction stepFunction, bool alwaysApply);
    
private:
    
};


class TwoNodesTourExchange : public NearestNodesNeighborhood
{
public:
    TwoNodesTourExchange(Environment& env, NodeInserter& nodeInserter);
    
    virtual std::string getName() const;
    
    virtual bool performStep(Instance& instance, Config::StepFunction stepFunction, bool alwaysApply);

private:
    
};


#endif // TOURNEIGHBORHOOD_H
