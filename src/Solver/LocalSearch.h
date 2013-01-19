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

    LocalSearch(const LocalSearch& obj) 
    : AbstractSearch(obj), instance(obj.instance), neighborhood(*obj.neighborhood.clone()) {}
    
    virtual Instance& getInstance() { return instance; };

    virtual void reset(const Instance& init);
    
    virtual void run();
    
    virtual AbstractSearch* clone() const { return new LocalSearch(*this); }
    
};

#endif // LOCALSEARCH_H
