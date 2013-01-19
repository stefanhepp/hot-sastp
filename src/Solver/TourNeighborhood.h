#ifndef TOURNEIGHBORHOOD_H
#define TOURNEIGHBORHOOD_H

#include "Support/Instance.h"
#include "Support/Environment.h"
#include "Solver/Neighborhood.h"
#include "Solver/AbstractSearch.h"

#include <vector>
#include <set>

/*
 * This file contains neighborhoods that work on whole subtours
 */

/**
 * First in pair is method-id, second in pair is Satisfaction/Time ratio
 */
typedef std::vector< std::pair<unsigned, double> > MethodRatioList;

class NodeInserter {
public:
    NodeInserter(Environment& env, unsigned int maxk, bool insertUsed);
        
    /**
     * Initialize search for the next step.
     */
    virtual void prepareStep(Instance& instance, Config::StepFunction stepFunction) =0;
    
    /**
     * Find a set of random nodes to insert.
     * 
     * @param instance the tour to insert nodes to. Does not contain removed nodes.
     * @param removedSpots the tour nodes that have been removed from the tour.
     */
    virtual double findRandomInsertNodes(Instance& instance, const TourNodeIndexList& removedNodes, double minSatisfaction) =0;
    
    /**
     * Find a set of nodes to insert.
     * 
     * @param instance the tour to insert nodes to. Does not contain removed nodes.
     * @param removedSpots the tour nodes that have been removed from the tour.
     * @param findBestStep if true, search for the best step, otherwise stop at the first candidate that gives an improvement.
     */
    virtual double findInsertNodes(Instance& instance, const TourNodeIndexList& removedNodes, double minSatisfaction, bool findBestStep) =0;
    
    /**
     * Insert the found nodes into the tour.
     * @param instance the instance to update, as passed to the find{Random}InsertNodes call.
     * @param insertBestStep if false, insert the nodes found by the last find{Random}InsertNodes call. If true, insert the
     *                       nodes stored by the last storeAsBestStep cal.
     */
    virtual void insertNodes(Instance& instance, bool insertBestStep);
    
    /**
     * Store the last result found by find[Random]InsertNodes as current best result
     */
    virtual void storeAsBestStep();

    virtual NodeInserter* clone() const =0;
    
protected:
    Environment& env;
    SpotSearch& spotsearch;
    unsigned maxk;
    bool insertUsed;
    bool skipMethodOnly;
    
    TourNodeIndexList newNodes;
    TourNodeIndexList bestNewNodes;
    
    /**
     * Insert a node into the newNodes list.
     * @param insertAt the position to insert new node
     * @param node the node to insert
     * @param insertBest if true, allow insertion after insert point
     * @return the index of the node in newNodes
     */
    unsigned addNewNode(const Instance& instance, unsigned insertAt, TourNode node, bool insertBest);
    
    TourValues getDeltaInsertValues(const Instance& instance);
    
    MethodRatioList getMethodRatios(const Instance& instance, NearestSpotList& nearestSpots, const TourNodeIndexList& removedNodes);
    
    unsigned findBestNearestNode(const MethodRatioList& ratioList);

    /**
     * @param instance the instance
     * @param nearestSpots the nearest spots to select from
     * @param ratios the ratios used to select nodes, or select randomly if NULL
     * @param removedNodes nodes that were removed from instance
     * @param findBestStep if true, search all candidates for best improvement
     * @return the satisfaction improvement
     */
    double selectNewNodes(Instance& instance, NearestSpotList& nearestSpots, MethodRatioList *ratios, 
			      const TourNodeIndexList& removedNodes, bool findBestStep);
    
    // TourNodeIndexList getRatioSortedNearestNodes(const Instance& instance, const NearestSpotList& nearest) const;
  
    bool skipNode(const TourNodeIndexList& removedNodes, unsigned spotId, unsigned methodId);
};

/**
 * This node inserter uses another search to finish the current tour. Removed nodes are always reused, all step functions
 * behave the same way.
 */
class SearchNodeInserter : public NodeInserter {
public:
    SearchNodeInserter(Environment& env, AbstractSearch& search);

    SearchNodeInserter(const SearchNodeInserter& obj)
    : NodeInserter(obj), search(*obj.search.clone()), newTour(obj.newTour), bestNewTour(obj.bestNewTour) {}
    
    virtual void prepareStep(Instance& instance, Config::StepFunction stepFunction);
    
    virtual double findRandomInsertNodes(Instance& instance, const TourNodeIndexList& removedNodes, double minSatisfaction);
    
    virtual double findInsertNodes(Instance& instance, const TourNodeIndexList& removedNodes, double minSatisfaction, bool findBestStep);
    
    virtual void insertNodes(Instance& instance, bool insertBestStep) { instance = insertBestStep ? bestNewTour : newTour; }
    
    virtual void storeAsBestStep() { bestNewTour = newTour; }

    virtual NodeInserter* clone() const { return new SearchNodeInserter(*this); }
    
protected:
    AbstractSearch& search;
    
    Instance newTour;
    Instance bestNewTour;
    
    double runSearch(Instance& instance);
};

class ConsecutiveNodeInserter : public NodeInserter {
public:
    ConsecutiveNodeInserter(Environment& env, unsigned int maxk, bool insertUsed);

    virtual void prepareStep(Instance& instance, Config::StepFunction stepFunction);
    
    virtual double findRandomInsertNodes(Instance& instance, const TourNodeIndexList& removedNodes, double minSatisfaction);
    
    virtual double findInsertNodes(Instance& instance, const TourNodeIndexList& removedNodes, double minSatisfaction, bool findBestStep);
    
    virtual NodeInserter* clone() const  { return new ConsecutiveNodeInserter(*this); }
};

class RandomNodeInserter : public NodeInserter {
public:
    RandomNodeInserter(Environment& env, unsigned int maxk, bool insertUsed);
    
    virtual void prepareStep(Instance& instance, Config::StepFunction stepFunction);
    
    virtual double findRandomInsertNodes(Instance& instance, const TourNodeIndexList& removedNodes, double minSatisfaction);
    
    virtual double findInsertNodes(Instance& instance, const TourNodeIndexList& removedNodes, double minSatisfaction, bool findBestStep);
    
    virtual NodeInserter* clone() const { return new RandomNodeInserter(*this); }
};

/**
 * Base class for nearest nodes exchange neighborhoods, provides some common functions
 */
class NearestNodesNeighborhood : public Neighborhood
{
public:
    NearestNodesNeighborhood(Environment& env, NodeInserter& nodeInserter);

    NearestNodesNeighborhood(const NearestNodesNeighborhood& obj) 
     : Neighborhood(obj), nodeInserter(*obj.nodeInserter.clone()) {}

protected:
    NodeInserter& nodeInserter;
    
    /**
     * Must be sorted by tour index
     */
    TourNodeIndexList removedNodes;    
};


/**
 * This neighborhood removes up to k nodes from a tour (either consecutive or not) from a tour and
 * inserts as many nearest tour neighbors as possible back into the tour.
 */
class NearestTourExchange : public NearestNodesNeighborhood
{
    unsigned minRemove;
    unsigned maxRemove;
    unsigned lastFirstNode;
    
public:
    NearestTourExchange(Environment& env, unsigned minRemove, unsigned maxRemove, NodeInserter& nodeInserter);
    
    virtual std::string getName() const;
    
    virtual bool performStep(Instance& instance, Config::StepFunction stepFunction, bool alwaysApply);
    
    virtual Neighborhood* clone() const { return new NearestTourExchange(*this); }    
};


class TwoNodesTourExchange : public NearestNodesNeighborhood
{
public:
    TwoNodesTourExchange(Environment& env, NodeInserter& nodeInserter);
    
    virtual std::string getName() const;
    
    virtual bool performStep(Instance& instance, Config::StepFunction stepFunction, bool alwaysApply);

    virtual Neighborhood* clone() const { return new TwoNodesTourExchange(*this); }

private:
    unsigned lastFirstNode;
    
};


#endif // TOURNEIGHBORHOOD_H
