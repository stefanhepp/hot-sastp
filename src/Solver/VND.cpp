#include "VND.h"

#include "Support/Environment.h"
#include "Support/Instance.h"
#include "Solver/AbstractSearch.h"
#include "Solver/Neighborhood.h"

#include <vector>

using namespace std;

VND::VND(Environment& env, const Instance& init)
// TODO get number of steps without improveent from config
: AbstractSearch(env, 10), instance(init)
{
}

void VND::reset(const Instance& init)
{
    instance = init;
}

void VND::run()
{
    
}

