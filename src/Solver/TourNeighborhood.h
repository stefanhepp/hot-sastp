#ifndef TOURNEIGHBORHOOD_H
#define TOURNEIGHBORHOOD_H

#include "Support/Instance.h"
#include "Support/Environment.h"
#include "Solver/Neighborhood.h"

/*
 * This file contains neighborhoods that work on whole subtours
 */

/**
 * This neighborhood removes up to k nodes from a tour (either consecutive or not) from a tour and
 * inserts as many nearest tour neighbors as possible back into the tour.
 */
class NearestTourNodes : public Neighborhood
{
    unsigned maxRemove;
    bool removeConsecutive;
    unsigned maxk;
    bool insertConsecutive;
    
public:
    NearestTourNodes(Environment& env, unsigned maxRemove, bool removeConsecutive, unsigned maxk, bool insertConsecutive);
    
    virtual std::string getName() const;
    
    virtual bool performStep(Instance& instance, Config::StepFunction stepFunction, bool alwaysApply);

};

#endif // TOURNEIGHBORHOOD_H
