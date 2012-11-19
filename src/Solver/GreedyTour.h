#ifndef GREEDYTOUR_H
#define GREEDYTOUR_H

#include "Support/Instance.h"
#include "Framework/SASTProblem.h"
#include "Framework/SASTPSolution.h"

/**
 * Create an (initial) solution using a greedy algorithm based on inserting favorable 
 * nodes into an existing tour.
 */
class GreedyTour {

    Instance instance;

public:
    GreedyTour(const SASTProblem& problem);
    
    Instance& getInstance() { return instance; };
    
    void run();
    
protected:

    /**
     * Add or insert a spot into the tour.
     * @return the index of the inserted spot in the tour.
     */
    virtual unsigned insertSpot() =0;
    
};

class GreedyNearestNeighbor : public GreedyTour {
public:
    GreedyNearestNeighbor(const SASTProblem& problem) : GreedyTour(problem) {};
    
protected:
    
    virtual unsigned insertSpot();
    
};

#endif