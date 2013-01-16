#ifndef ANT_H
#define ANT_H

#include "Framework/SASTProblem.h"
#include "Support/Environment.h"
#include "Support/Instance.h"
#include "Support/SpotSearch.h"
#include "Ants/PheromoneMatrix.h"

// This class implements the behaviour of ant k. 
// An ant creates a tour based on some heuristics and a parametrized neighborhood.
class Ant
{
    Instance instance;
    
    SASTProblem &problem;
    
public:
    Ant(Environment &env, int k);
    
    Instance &getInstance() { return instance; }
    
    void setInstance(const Instance& inst) { instance = inst; }
    
    // construct a tour based on the pheromeone matrix and the neighborhood heuristics
    void findTour(PheromoneMatrix &pm);
    
    // Update pheromeones at the end of a full step for the next iteration
    void addPheromones(PheromoneMatrix &pm, double factor);
};

#endif // ANT_H
