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
    
    if (env.getConfig().isDebug()) {
	cout << "VND: Starting with " << neighborhoods[0]->getName() << endl;
    }
    
    start();
    
    do {
	
	// Choose best or next x' in N(x), apply if f(x') <= f(x)
	
	if ( neighborhoods[i]->performStep(instance, stepFunctions[i], false) ) {
	    env.printStepResult(instance);
	
	    if ( i != 0 && env.getConfig().isDebug() ) {
		cout << "VND: Switching back to " << neighborhoods[0]->getName() << endl;
	    }
	    i = 0;
	} else {
	    env.printStepResult(instance);	

	    i++;
	    if ( i < neighborhoods.size() && env.getConfig().isDebug() ) {
		cout << "VND: Switching to " << neighborhoods[i]->getName() << endl;
	    }
	}
	
    } while (i < neighborhoods.size());
    
}

