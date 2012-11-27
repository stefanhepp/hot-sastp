#include "VND.h"

#include "Support/Environment.h"
#include "Support/Instance.h"
#include "Solver/AbstractSearch.h"
#include "Solver/Neighborhood.h"

#include <iostream>
#include <vector>

using namespace std;

VND::VND(Environment& env, const Instance& init)
: AbstractSearch(env, 1), instance(init)
{
}

void VND::reset(const Instance& init)
{
    instance = init;
}

void VND::run()
{
    double deltaSatisfaction;
    unsigned i = 0;
    
    if (stepFunction == Config::SF_RANDOM) {
	cout << "Cannot use RANDOM step function for VND, using NEXT instead!" << endl;
	stepFunction = Config::SF_NEXT;
    }
    
    start();
    
    do {
	
	env.printStepResult(instance);
	
	// Choose best or next x' in N(x), apply if f(x') <= f(x)
	
	if ( neighborhoods[i]->performStep(instance, stepFunction, false) ) {
	    i = 1;
	} else {
	    i++;
	}
	
    } while (i < neighborhoods.size());
    
}

