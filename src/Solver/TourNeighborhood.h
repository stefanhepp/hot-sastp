#ifndef TOURNEIGHBORHOOD_H
#define TOURNEIGHBORHOOD_H

#include "Support/Instance.h"
#include "Support/Environment.h"
#include "Solver/Neighborhood.h"

/*
 * This file contains neighborhoods that work on whole subtours
 */

/**
 * Base class for nearest nodes exchange neighborhoods, provides some common functions
 */
class NearestNodesNeighborhood : public Neighborhood
{
    
public:
    NearestNodesNeighborhood(Environment& env, unsigned int maxk, bool insertConsecutive);

protected:
    unsigned maxk;
    bool insertConsecutive;
    
};


/**
 * This neighborhood removes up to k nodes from a tour (either consecutive or not) from a tour and
 * inserts as many nearest tour neighbors as possible back into the tour.
 */
class NearestTourExchange : public NearestNodesNeighborhood
{
    unsigned maxRemove;
    
public:
    NearestTourExchange(Environment& env, unsigned int maxRemove, unsigned int maxk, bool insertConsecutive);
    
    virtual std::string getName() const;
    
    virtual bool performStep(Instance& instance, Config::StepFunction stepFunction, bool alwaysApply);
    
private:
    
};


class TwoNodesTourExchange : public NearestNodesNeighborhood
{
    unsigned maxRemove;
    
public:
    TwoNodesTourExchange(Environment& env, unsigned int maxk, bool insertConsecutive);
    
    virtual std::string getName() const;
    
    virtual bool performStep(Instance& instance, Config::StepFunction stepFunction, bool alwaysApply);

private:
    
};


#endif // TOURNEIGHBORHOOD_H
