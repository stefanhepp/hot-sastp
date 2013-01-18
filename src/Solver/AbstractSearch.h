#ifndef ABSTRACTSEARCH_H
#define ABSTRACTSEARCH_H

#include "Framework/SASTProblem.h"
#include "Support/Environment.h"
#include "Support/Instance.h"

class AbstractSearch
{
public:
    AbstractSearch(Environment& env, int stepsToStop);

    virtual ~AbstractSearch() {}
    
    virtual Instance& getInstance() =0;
    
    virtual void reset(const Instance& init) =0;
    
    virtual void run() =0;
    
    int getCurrentStep() const { return _currStep; }
    
    int getLastImprovementStep() const { return _lastImprovementStep; }
    
    virtual AbstractSearch* clone() const =0;
    
protected:
    int _noImprovement;
    int _maxSteps;
    int _currStep;
    int _lastImprovementStep;
    
    Environment& env;
    SASTProblem& problem;
    
    Config::StepFunction stepFunction;
    
    void start() { _noImprovement =0; _lastImprovementStep = 0; _currStep = 0; }
    
    bool shouldStop(double improvement);
    
};

// Search implementation that does not modify the instance.
class DummySearch : public AbstractSearch {
    Instance instance;
public:
    DummySearch(Environment &env) : AbstractSearch(env, 0), instance(env.getProblem()) {}
    
    virtual Instance& getInstance() { return instance; }
    
    virtual void reset(const Instance& init) { instance = init; }
    
    virtual void run() {}    
    
    virtual AbstractSearch* clone() const { return new DummySearch(*this); }
};

#endif // ABSTRACTSEARCH_H
