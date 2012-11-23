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
    
public:
    VND(Environment& env, const Instance& init);

    void addNeighborhood(Neighborhood& nb) { neighborhoods.push_back(&nb); }
    
    virtual Instance& getInstance() { return instance; }
    
    virtual void reset(const Instance& init);
    
    virtual void run();

};

#endif // VND_H
