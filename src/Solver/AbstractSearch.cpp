#include "AbstractSearch.h"

AbstractSearch::AbstractSearch(Environment& env)
: problem(env.getProblem())
{
    stepFunction = env.getConfig().getStepFunction();
}

bool AbstractSearch::shouldStop()
{
    // TODO define some simple generic stop criteria (based on running time, # of iterations without true improvement, ..)
    // TODO add some arguments to support this
    
    return false;
}

