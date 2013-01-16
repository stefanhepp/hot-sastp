#include "Ant.h"

#include "Framework/SASTProblem.h"
#include "Solver/TourNeighborhood.h"

Ant::Ant(Environment &env, int k)
: instance(env), problem(env.getProblem())
{
    // TODO insert neighborhood structures, if required
}

void Ant::findTour(PheromoneMatrix &pm)
{
    instance.clear();
    
    // TODO use neigborhood and heuristics as well as pheromones to construct a valid tour
    
    // Add nodes at end of tour until tour is complete
    // Better way to do it: interpret construction graph not as tour but as decision tree which 
    // nodes to insert -> t_ij means: insert node j after node i has been inserted into tour
    // TODO but where to insert? use shortest distance?
    
}

