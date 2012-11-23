#ifndef LOCALSEARCH_H
#define LOCALSEARCH_H

#include "Framework/SASTProblem.h"
#include "Support/Environment.h"
#include "Support/Instance.h"
#include "Solver/AbstractSearch.h"
#include "Solver/Neighborhood.h"

class LocalSearch : public AbstractSearch
{
    Instance instance;
    Neighborhood& neighborhood;
    
public:
    LocalSearch(Environment& env, Neighborhood& nb, const Instance& init);

    virtual Instance& getInstance() { return instance; };

    virtual void reset(const Instance& init);
    
    virtual void run();
    
};

#endif // LOCALSEARCH_H