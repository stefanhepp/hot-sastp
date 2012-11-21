#include "Driver.h"

#include "Support/SpotSearch.h"

Driver::~Driver()
{
    if (solution) delete solution;
}


void Driver::prepare()
{
    // Initialize the SpotSearch
    env.initSpotSearch( env.getConfig().getMaxKNearestSpots() );
    
}

void Driver::solve()
{
    // TODO use env.getConfig() to instanciate appropriate greedy solver
    GreedyInsertHeuristic greedy(env);
    
    
    greedy.run();
    
    // TODO use config to run requested heuristic improvement on initial solution, if enabled
    
    
    
    solution = greedy.getInstance().createSolution();
}

void Driver::writeSolution()
{
    solution->store(env.getConfig().getOutputFilename());
}

