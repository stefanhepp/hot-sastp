#include "GVNS.h"

#include "Solver/VND.h"

GVNS::GVNS(Environment& env, VND& vnd, const Instance& init)
: AbstractSearch(env, env.getConfig().getMaxStepsNoChange()), vnd(vnd), instance(init)
{
}

void GVNS::reset(const Instance& init)
{
    instance = init;
}

void GVNS::run()
{
    unsigned k;
    double deltaSatisfaction;
    
    start();
    
    do { 
	k = 0;
	
	bool printStep = env.setPrintSteps(false);
	double prevSatisfaction = instance.getTotalSatisfaction();
	
	do {
	    
	    // Shaking: generate x' from N_k(x)
	    Instance shaken = instance;
	    neighborhoods[k]->performStep(shaken, Config::SF_RANDOM, true);
	    
	    // x' = VND(x')
	    vnd.reset(shaken);
	    vnd.run();
	    
	    // check for improvement, otherwise increase GVNS neighborhood
	    if (vnd.getInstance().getTotalSatisfaction() > instance.getTotalSatisfaction()) {
		instance = vnd.getInstance();
		k = 0;
	    } else {
		k = k + 1;
	    }
	    
	} while (k < neighborhoods.size());
	
	deltaSatisfaction = instance.getTotalSatisfaction() - prevSatisfaction;
	
	env.setPrintSteps(printStep);
	env.printStepResult(instance);
	
    } while (!shouldStop(deltaSatisfaction));
}

