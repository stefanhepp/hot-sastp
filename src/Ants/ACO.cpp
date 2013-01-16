#include "ACO.h"

#include "Ants/Ant.h"

ACO::ACO(Environment& env, AbstractSearch &localSearch)
// TODO get stepsToStop from config, set initial tau value to something meaningful
: AbstractSearch(env, 100), instance(env), PM(env.getProblem(), 0), 
  localSearch(localSearch)
{
}

void ACO::reset(const Instance& init)
{
    // Does not do much though, not used to initialize pheromones or anything.
    instance = init;
    PM.reset(0);
}

void ACO::run()
{
    start();

    double deltaSatisfaction = 0;
    
    do {
	
	// perform step with all ants
	for (AntList::iterator it = ants.begin(), ie = ants.end(); it != ie; ++it) {
	    it->findTour(PM);
	}
	
	// perform deamon actions and sort instances by satisfaction
	
	
	// update pheromone matrix
	updatePheromones();
	
	
    } while (!shouldStop(deltaSatisfaction));

}

void ACO::initAnts()
{

}

void ACO::updatePheromones()
{

}
