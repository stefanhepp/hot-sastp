#ifndef NEIGHBORHOOD_H
#define NEIGHBORHOOD_H

#include "Framework/SASTProblem.h"
#include "Support/Environment.h"
#include "Support/Instance.h"

/**
 * Base class for neighborhoods
 */
class Neighborhood
{
public:
    Neighborhood(Environment& env);
    
    /**
     * Perform a single step, using the given step function.
     * - Select a neighbor from the given instance using the given step function
     * - Evaluate the step
     * - If !alwaysApply, perform the step only if the satisfaction value did not decrease.
     * 
     * @param instance the instance to calculate the neighbor for
     * @param stepFunction the step function to use
     * @param alwaysApply if true, perform the step even if the satisfaction decreases
     * @return true if a step has been performed
     */
    virtual bool performStep(Instance& instance, Config::StepFunction stepFunction, bool alwaysApply)=0;
    
protected:
    
    Environment& env;
    
};

/**
 * Exchange one spot with an unused spot and pick a proper method.
 * - SF_RANDOM: pick a random spot and a random method of that spot
 * - SF_NEXT: pick the first node in the tour with lowest satisfaction/time ratio, find nearest spot 
 *   that gives some improvement on exchange
 * - SF_BEST: search all combinations of nodes in the tour and unused nodes to find best spot to replace
 */
class OneOPT: public Neighborhood
{
public :
    OneOPT (Environment& env):Neighborhood(env){};
    
    virtual bool performStep(Instance& instance, Config::StepFunction stepFunction, bool alwaysApply);    
    
protected:
    
    /**
     * Perform the search for the first method such that inserting the selected spot would result in a valid tour.
     * @param instance the instance to calculate the neighbor for
     * @param where index of the spot in the tour chosen for removal 
     * @param what index of the spot to be inserted
     * @param method the index of the randomly chosen method
     */
    unsigned findRandomMethod(Instance& instance, unsigned int where, unsigned int what, unsigned method);
};

/**
 * Exchange two edges in the tour.
 * - SF_RANDOM: pick two random edges to exchange
 * - SF_NEXT: TODO
 * - SF_BEST: search all pairs of edges in the tour for possible improvements
 */
class TwoOPT: public Neighborhood
{
public :
    TwoOPT (Environment& env):Neighborhood(env){};
    
    virtual bool performStep(Instance& instance, Config::StepFunction stepFunction, bool alwaysApply);
    
protected:
    
    bool isValidEdgeExchange(Instance& instance, int firstEdge, int secondEdge, double &deltaSatisfaction);
    
    bool performEdgeExchange(Instance& instance, int firstEdge, int secondEdge);
    
};


#endif // NEIGHBORHOOD_H
