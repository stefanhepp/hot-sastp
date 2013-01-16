#ifndef ACO_H
#define ACO_H

#include "Support/Instance.h"
#include "Support/Environment.h"
#include "Ants/PheromoneMatrix.h"
#include "Ants/Ant.h"
#include "Solver/AbstractSearch.h"

#include <vector>



class ACO : public AbstractSearch
{
    typedef std::vector<Ant*> AntList;
    typedef std::vector<double> DoubleList;
    
    // This is (a copy of) the best instance of the last iteration
    Instance instance;

    PheromoneMatrix PM;
    
    // The local search to use to improve the results
    AbstractSearch &localSearch;
    
    // If true, improve results only for evaluation, not for pheromone update
    bool improveForFitnessOnly;
    
    AntList ants;
    
public:
  
    ACO(Environment& env, AbstractSearch &localSearch);
    
    virtual Instance& getInstance();
    
    virtual void reset(const Instance& init);
    
    virtual void run();
    
private:
    void initAnts(int numAnts);
    
    void updatePheromones(DoubleList &satisfaction);
    
    
};

#endif // ACO_H
