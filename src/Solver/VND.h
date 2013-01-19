#ifndef VND_H
#define VND_H

#include "Support/Environment.h"
#include "Support/Instance.h"
#include "Solver/AbstractSearch.h"
#include "Solver/Neighborhood.h"

#include <vector>

/**
 * Implementation of the variable neighborhood descent search.
 */
class VND : public AbstractSearch
{
    Instance instance;
    
    std::vector<Neighborhood*> neighborhoods;
    std::vector<Config::StepFunction> stepFunctions;
    
public:
    VND(Environment& env, const Instance& init);

    VND(const VND& obj) 
    : AbstractSearch(obj), instance(obj.instance) 
    {
	for (auto nb : obj.neighborhoods) neighborhoods.push_back(nb->clone());
	stepFunctions.insert(stepFunctions.begin(), obj.stepFunctions.begin(), obj.stepFunctions.end());
    }
    
    void addNeighborhood(Neighborhood& nb) { 
	neighborhoods.push_back(&nb); 
	stepFunctions.push_back(stepFunction);
    }
    
    void addNeighborhood(Neighborhood& nb, Config::StepFunction sf) { 
	neighborhoods.push_back(&nb); 
	stepFunctions.push_back(sf);
    }
    
    virtual Instance& getInstance() { return instance; }
    
    virtual void reset(const Instance& init);
    
    virtual void run();

    virtual VND* clone() const { return new VND(*this); }
    
};

#endif // VND_H
