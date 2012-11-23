#include "AbstractSearch.h"

AbstractSearch::AbstractSearch(Environment& env, int stepsToStop)
: problem(env.getProblem()), _maxSteps(stepsToStop)
{
    stepFunction = env.getConfig().getStepFunction();
}

bool AbstractSearch::shouldStop(double improvement)
{
    if(improvement <= 0)
      _noImprovement++;
    if(_maxSteps == _noImprovement)
      return true;
    // TODO define some simple generic stop criteria (based on running time, # of iterations without true improvement, ..)
    // TODO add some arguments to support this
    return false;
}

