#include "AbstractSearch.h"

AbstractSearch::AbstractSearch(Environment& env, int stepsToStop)
: problem(env.getProblem()), _steps(stepsToStop)
{
    stepFunction = env.getConfig().getStepFunction();
}

bool AbstractSearch::shouldStop(double improvement)
{
    // TODO define some simple generic stop criteria (based on running time, # of iterations without true improvement, ..)
    // TODO add some arguments to support this
    
    return false;
}

