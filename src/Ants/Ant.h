#ifndef ANT_H
#define ANT_H

#include "Framework/SASTProblem.h"
#include "Support/Environment.h"
#include "Support/Instance.h"
#include "Support/SpotSearch.h"
#include "Support/ProblemHelper.h"
#include "Ants/PheromoneMatrix.h"

// This class implements the behaviour of ant k. 
// An ant creates a tour based on some heuristics and a parametrized neighborhood.
class Ant
{
protected:
    Instance instance;
    
    SASTProblem &problem;
    
public:
    Ant(Environment &env, int k) ;

    virtual ~Ant() {}
    
    Instance &getInstance() { return instance; }
    
    virtual void setInstance(const Instance& inst) { instance = inst; }
    
    // construct a tour based on the pheromeone matrix and the neighborhood heuristics
    virtual void findTour(PheromoneMatrix &pm)=0;
    
    // Update pheromeones at the end of a full step for the next iteration

    virtual void addPheromones(PheromoneMatrix &pm, double factor)=0;

    virtual Ant* clone() = 0;
    
protected: 
    
    unsigned _antNumber;
    double _alpha, _beta;

};


class AntNearest: public Ant {
public: 
    AntNearest(Environment& env, int k):
        Ant(env,k), spotsearch(env.getSpotSearch()), helper(env.getProblem(), env.getSpotSearch())
        {
            _maxk = env.getConfig().getMaxKNearestSpots();
            _insertMode = env.getConfig().getNodeInsertMode();
        }
    
    virtual void findTour(PheromoneMatrix &pm);
    
    virtual void addPheromones(PheromoneMatrix& pm, double factor);
    
    virtual Ant* clone() { return new AntNearest(*this); }
    
private: 
    int insertSpot();
    
    TourNode selectBestTourNode(NearestSpotList nearest, unsigned insertAt);
    
    /**
     * This is a helping function in order for us to compute the p_ij^k
     * Get the distance per satisfaction to power of beta ( part of the computation of :visibility n:)
     * @param begin - the node from which you start (i) 
     * @param end - the node you pick 
     * @param m - the method choose for node end
     * 
     * Computes the ratio of distance per satisfaction and then to the power of beta
     */
    double getDistancePerSatisfaction(Spot begin, Spot end, const Method& m);
        
    SpotSearch& spotsearch;
    ProblemHelper helper;
    unsigned _maxk;
    
    Config::NodeInsertMode _insertMode;
    
};

class AntInsert: public Ant {
    std::vector<TourNode> insertionOrder;
    
public: 
    AntInsert(Environment& env, int k):
       Ant(env,k), spotsearch(env.getSpotSearch()), helper(env.getProblem(), env.getSpotSearch())
       {}
    
    virtual void setInstance(const Instance& inst);
    
    virtual void findTour(PheromoneMatrix &pm);
    
    virtual void addPheromones(PheromoneMatrix& pm, double factor);
    
    virtual Ant* clone() { return new AntInsert(*this); }
    
private: 
    
    int insertSpot() {};
    TourNode selectBestTourNode(NearestSpotList nearest, unsigned insertAt, Config::NodeInsertMode insertMethod);
    
    SpotSearch& spotsearch;
    ProblemHelper helper;
    unsigned _maxk;
    
};
#endif // ANT_H
