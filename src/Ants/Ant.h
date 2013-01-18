#ifndef ANT_H
#define ANT_H

#include "Framework/SASTProblem.h"
#include "Support/Environment.h"
#include "Support/Instance.h"
#include "Support/SpotSearch.h"
#include "Support/ProblemHelper.h"
#include "Ants/PheromoneMatrix.h"

#include <vector>

class AntNeighborhood {
protected:
    unsigned _antNumber;
    unsigned _maxK;
    
public:
    AntNeighborhood(Environment &env, unsigned k);
            
    unsigned getMaxNearestK() { return _maxK; }
    
    
};




// This class implements the behaviour of ant k. 
// An ant creates a tour based on some heuristics and a parametrized neighborhood.
class Ant
{
protected:
    Instance instance;
    
    SASTProblem &problem;
    
    ProblemHelper helper;
    
    SpotSearch& spotsearch;    
    
    AntNeighborhood &neighborhood;
    
public:
    Ant(Environment &env, PheromoneMatrix &pm, AntNeighborhood &nb, int k);

    virtual ~Ant() {}
    
    Instance &getInstance() { return instance; }
    
    virtual void setInstance(const Instance& inst) { instance = inst; }
    
    // construct a tour based on the pheromeone matrix and the neighborhood heuristics
    virtual void findTour();
    
    // Update pheromeones at the end of a full step for the next iteration

    virtual void addPheromones(double factor)=0;

    virtual Ant* clone() = 0;
    
protected: 
    /**
     * Insert a new node into the tour and return its index.
     */
    virtual int insertSpot() = 0;

    virtual TourNode getLastNode() = 0;
    
    virtual TourValues getInsertValues(int tournode, TourNode newNode, unsigned &insertAt)=0;
    
    /**
     * @param insertAt - out parameter, returns index where to insert the selected node
     */
    TourNode selectBestTourNode(NearestSpotList nearest, unsigned &insertAt, Config::NodeInsertMode insertMethod);

    /**
     * This computes the tau_ij^alpha * eta_ij^beta.
     *
     * @param insertIndex - the index where to insert the new node (will be inserted before that index)
     * @param newNode - the spot and method to insert
     * 
     * Computes the ratio of distance per satisfaction and then to the power of beta
     */
    double getTauEta(TourNode lastNode, TourNode newNode, TourValues insertValues);
    
    /**
     * This is a helping function in order for us to compute the p_ij^k
     * Get the distance per satisfaction to power of beta ( part of the computation of :visibility n:)
     * 
     * @param insertValues - the delta values for inserting the new node
     * 
     * Computes the ratio of distance per satisfaction and then to the power of beta
     */
    double getEta(TourValues insertValues);
    
    /**
     * Calculate 1/length(T^k)
     */
    double getTourDeltaTau() const;

    
    PheromoneMatrix& _pm;
    unsigned _antNumber;
    double _alpha, _beta;

};


class AntNearest: public Ant {
public: 
    AntNearest(Environment& env, PheromoneMatrix &pm, AntNeighborhood &nb, int k)
     : Ant(env,pm,nb,k)
    { }
    
    virtual void addPheromones(double factor);
    
    virtual Ant* clone() { return new AntNearest(*this); }
    
protected:
    virtual int insertSpot();
    
    virtual TourNode getLastNode() { return instance.getLastNode(); }
    
    virtual TourValues getInsertValues(int tournode, TourNode newNode, unsigned &insertAt);
    
private:        
    
};

class AntInsert: public Ant {
    std::vector<TourNode> insertionOrder;
    
public: 
    AntInsert(Environment& env, PheromoneMatrix &pm, AntNeighborhood &nb, int k):
       Ant(env,pm,nb,k), 
       _insertMode(env.getConfig().getNodeInsertMode())
       {}
       
    virtual void findTour();
    
    virtual void setInstance(const Instance& inst);
    
    virtual void addPheromones(double factor);
    
    virtual Ant* clone() { return new AntInsert(*this); }
    
protected:
    virtual int insertSpot();
    
    virtual TourNode getLastNode() { return insertionOrder.empty() ? instance.getHotelNode() : insertionOrder.back(); }
    
    virtual TourValues getInsertValues(int tournode, TourNode newNode, unsigned &insertAt);
private:     
    
    Config::NodeInsertMode _insertMode;
};
#endif // ANT_H
