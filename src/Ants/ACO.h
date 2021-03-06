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
    typedef struct {
      unsigned antId;
      double antSatisfaction;
      double optSatisfaction;
    } AntResults;
  
    typedef std::vector<Ant*> AntList;
    typedef std::vector<AntResults> SatisfactionList;
    
    // This is (a copy of) the best instance of the last iteration, after localSearch
    Instance instance;

    // Copy of our globally best ant, for updates
    Ant* bestAnt;

    PheromoneMatrix PM;
    
    // The local search to use to improve the results
    AbstractSearch &localSearch;
    AbstractSearch *optionalSearch;

    unsigned optionalSearchSteps;
    
    unsigned maxSteps;
    
    // If true, improve results only for evaluation, not for pheromone update
    bool improveAntSolution;
    
    unsigned updateWithGlobalBest;
    
    AntList ants;
public:
  
    ACO(Environment& env, AbstractSearch &localSearch, AbstractSearch* optionalSearch = 0, unsigned optionalSearchSteps = 0);

    virtual Instance& getInstance() { return instance; }

    virtual void reset(const Instance& init);
    
    virtual void run();
    
    virtual AbstractSearch* clone() const { return new ACO(*this); }

    
private:
    void initAnts(int numAnts);
    
    void setBestAnt(Ant* ant);
    
    void updatePheromones(SatisfactionList &satisfaction, unsigned round);
    
};

#endif // ACO_H
