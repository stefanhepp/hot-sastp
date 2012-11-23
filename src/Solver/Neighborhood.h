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
    Neighborhood(SASTProblem& problem);
    
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
    SASTProblem& problem;
    
};

class OneOPT: public Neighborhood
{
public :
    OneOPT (SASTProblem& problem):Neighborhood(problem){};
    //TODO implement the perform
    virtual bool performStep(Instance& instance, Config::StepFunction stepFunction, bool alwaysApply);    
protected:
};


class TwoOPT: public Neighborhood
{
public :
    TwoOPT (SASTProblem& problem):Neighborhood(problem){};
    virtual bool performStep(Instance& instance, Config::StepFunction stepFunction, bool alwaysApply);    
protected:
};

#endif // NEIGHBORHOOD_H
