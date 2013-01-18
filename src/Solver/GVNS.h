#ifndef GVNS_H
#define GVNS_H

#include "Support/Environment.h"
#include "Support/Instance.h"
#include "Solver/AbstractSearch.h"
#include "Solver/Neighborhood.h"
#include "Solver/VND.h"

#include <vector>

class GVNS : public AbstractSearch
{
    VND& vnd;
    Instance instance;
    
    std::vector<Neighborhood*> neighborhoods;
    
public:
    GVNS(Environment& env, VND& vnd, const Instance& init);
    
    virtual Instance& getInstance() { return instance; };
    
    void addNeighborhood(Neighborhood& nb) { neighborhoods.push_back(&nb); }
    
    virtual void reset(const Instance& init);
    
    virtual void run();
    
    virtual AbstractSearch* clone() const { return new GVNS(*this); }
    
};

#endif // GVNS_H
