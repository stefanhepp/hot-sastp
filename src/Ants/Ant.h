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
    Ant(Environment &env, int k) ;
    
    Instance &getInstance() { return instance; }
    
    void setInstance(const Instance& inst) { instance = inst; }
    
    // construct a tour based on the pheromeone matrix and the neighborhood heuristics
    virtual void findTour(PheromoneMatrix &pm)=0;
    
    // Update pheromeones at the end of a full step for the next iteration

    virtual void addPheromones(PheromoneMatrix &pm, double factor)=0;

protected: 
    
    unsigned _antNumber;
    double _alpha, _beta;

};


class AntNearest: public Ant {
public: 
    AntNearest(Environment& env, int k):Ant(env,k){}
    
    void findTour(PheromoneMatrix &pm);
    
    void addPheromones(PheromoneMatrix& pm, double factor);
private: 
    
};

class AntInsert: public Ant {
    std::vector<TourNode> insertionOrder;
    
public: 
    AntInsert(Environment& env, int k):Ant(env,k){}
    
    void findTour(PheromoneMatrix &pm);
    
    void addPheromones(PheromoneMatrix& pm, double factor);
private: 

    
};
#endif // ANT_H
