#ifndef ABSTRACTSEARCH_H
#define ABSTRACTSEARCH_H

#include "Framework/SASTProblem.h"
#include "Support/Environment.h"
#include "Support/Instance.h"

class AbstractSearch
{
public:
    AbstractSearch(Environment& env);

    virtual Instance& getInstance() =0;
    
    virtual void reset(const Instance& init) =0;
    
    virtual void run() =0;
    
protected:
    SASTProblem& problem;
    
    Config::StepFunction stepFunction;

    bool shouldStop();
    
};

#endif // ABSTRACTSEARCH_H
