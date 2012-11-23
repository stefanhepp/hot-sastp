#include "VND.h"

#include "Support/Environment.h"
#include "Support/Instance.h"
#include "Solver/AbstractSearch.h"
#include "Solver/Neighborhood.h"

#include <vector>

using namespace std;

VND::VND(Environment& env, const Instance& init)
: AbstractSearch(env), instance(init)
{
}

void VND::reset(const Instance& init)
{
    instance = init;
}

void VND::run()
{
    
}

