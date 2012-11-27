#ifndef ABSTRACTSEARCH_H
#define ABSTRACTSEARCH_H

#include "Framework/SASTProblem.h"
#include "Support/Environment.h"
#include "Support/Instance.h"

class AbstractSearch
{
public:
    AbstractSearch(Environment& env, int stepsToStop);

    virtual Instance& getInstance() =0;
    
    virtual void reset(const Instance& init) =0;
    
    virtual void run() =0;
    
protected:
    int _noImprovement;
    int _maxSteps;
    
    Environment& env;
    SASTProblem& problem;
    
    Config::StepFunction stepFunction;
    void start() { _noImprovement =0; }
    bool shouldStop(double improvement);
};

#endif // ABSTRACTSEARCH_H
