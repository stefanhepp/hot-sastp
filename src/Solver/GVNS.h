#ifndef GVNS_H
#define GVNS_H

#include "Support/Environment.h"
#include "Support/Instance.h"
#include "Solver/AbstractSearch.h"
#include "Solver/Neighborhood.h"

#include <vector>

class GVNS : public AbstractSearch
{
    Instance instance;
    
public:
    GVNS(Environment& env, const Instance& init);
    
    virtual Instance& getInstance() { return instance; };
    
    virtual void reset(const Instance& init);
    
    virtual void run();
};

#endif // GVNS_H
