#include "AbstractSearch.h"

AbstractSearch::AbstractSearch(Environment& env, int stepsToStop)
: env(env), problem(env.getProblem()), _maxSteps(stepsToStop)
{
    stepFunction = env.getConfig().getStepFunction();
}

bool AbstractSearch::shouldStop(double improvement)
{
    // TODO define some other generic stop criteria (based on running time, # of iterations without true improvement, ..)
    // TODO add some arguments to support this
    if(improvement <= 0)
      _noImprovement++;
    if(_maxSteps == _noImprovement)
      return true;
    
    if (env.getCurrentTime() > env.getConfig().getMaxRuntime()) return true;
    
    return false;
}

