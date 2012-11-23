#ifndef GRASP_H
#define GRASP_H

#include "Support/Environment.h"
#include "Support/Instance.h"
#include "Solver/AbstractSearch.h"
#include "Solver/Neighborhood.h"

class Grasp : public AbstractSearch
{
    Instance instance;
    Instance initInstance;

    AbstractSearch& localSearch;
    
public:
    Grasp(Environment& env, AbstractSearch& localSearch, const Instance& init);

    virtual Instance& getInstance() { return instance; };
    
    virtual void reset(const Instance& init);
    
    virtual void run();

};

#endif // GRASP_H
