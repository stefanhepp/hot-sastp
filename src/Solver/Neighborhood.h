#ifndef NEIGHBORHOOD_H
#define NEIGHBORHOOD_H

#include "Framework/SASTProblem.h"
#include "Support/Environment.h"
#include "Support/Instance.h"

#include <string>

/**
 * Base class for neighborhoods
 */
class Neighborhood
{
public:
    Neighborhood(Environment& env);
    
    virtual std::string getName() const =0;
    
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

    virtual Neighborhood* clone() const =0;
    
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
class SpotOneOPT: public Neighborhood
{
public :
    SpotOneOPT (Environment& env):Neighborhood(env){};
    
    virtual std::string getName() const { return "Spot-1-Opt"; }
    
    virtual bool performStep(Instance& instance, Config::StepFunction stepFunction, bool alwaysApply);    
    
    virtual Neighborhood* clone() const { return new SpotOneOPT(*this); }
    
private:
    
    /**
     * Perform the search for the first method such that inserting the selected spot would result in a valid tour.
     * @param instance the instance to calculate the neighbor for
     * @param nodeToUpdate index of the spot in the tour chosen for removal 
     * @param spotToInsert index of the spot to be inserted
     * @param method the index of the randomly chosen method
     * @param deltaSatisfaction returns the satisfaction difference
     */
    unsigned findRandomMethod(Instance& instance, unsigned int nodeToUpdate, unsigned int spotToInsert, unsigned method, double &deltaSatisfaction);
    
    /**
     * Perform the deletion of the node whereToInsert from the tour
     * Insert the whatToInsert in the tour 
     */
    void performReplaceNode(Instance& instance, unsigned nodeToUpdate, unsigned spotToInsert, unsigned method);
    
    /**
     * returns the diff and initializes method with the appropriate method and nodeIndex with the replacement 
     * node index.
     */
    bool performNextStep(Instance& instance, bool alwaysApply);
    bool performRandomStep(Instance& instance, bool alwaysApply);
    bool performMaxStep(Instance& instance, bool alwaysApply);
    
    
};


/**
 * Exchange two edges in the tour.
 * - SF_RANDOM: pick two random edges to exchange
 * - SF_NEXT: find first possible combination to exchange, search by increasing edge distance
 * - SF_BEST: search all pairs of edges in the tour for possible improvements
 */
class EdgeTwoOPT: public Neighborhood
{
public :
    EdgeTwoOPT (Environment& env):Neighborhood(env), lastFirstEdge(0) {}
    
    virtual std::string getName() const { return "Edge-2-Opt"; }
    
    virtual bool performStep(Instance& instance, Config::StepFunction stepFunction, bool alwaysApply);
    
    virtual Neighborhood* clone() const { return new EdgeTwoOPT(*this); }
    
private:
    
    bool isValidEdgeExchange(Instance& instance, int firstEdge, int secondEdge, double &deltaSatisfaction);
    
    void performEdgeExchange(Instance& instance, int firstEdge, int secondEdge);
    
    unsigned lastFirstEdge;
};


/**
 * Exchange two methods in the tour.
 * - SF_RANDOM: pick two random spots to exchange
 * - SF_NEXT: use first possible combination to exchange
 * - SF_BEST: search all pairs of spots in the tour for possible improvements
 */
class MethodTwoOPT: public Neighborhood
{
public :
    MethodTwoOPT (Environment& env):Neighborhood(env), lastFirstNodeId(0) {}
    
    virtual std::string getName() const { return "Method-2-Opt"; }
    
    virtual bool performStep(Instance& instance, Config::StepFunction stepFunction, bool alwaysApply);
    
    virtual Neighborhood* clone() const { return new MethodTwoOPT(*this); }
        
private:
    
    bool isValidMethodExchange(Instance& instance, 
			       const Method& firstOldMethod, const Method& secondOldMethod,
			       const Method& firstNewMethod, const Method& secondNewMethod, double &deltaSatisfaction);
    
    unsigned lastFirstNodeId;
};



#endif // NEIGHBORHOOD_H
