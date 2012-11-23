#ifndef GREEDYTOUR_H
#define GREEDYTOUR_H

#include "Framework/SASTProblem.h"
#include "Framework/SASTPSolution.h"
#include "Support/Instance.h"
#include "Support/Environment.h"
#include "Support/SpotSearch.h"
#include "Support/ProblemHelper.h"
#include "Solver/AbstractSearch.h"

/**
 * Create an (initial) solution using a greedy algorithm based on inserting favorable 
 * nodes into an existing tour.
 */
class GreedyTour : public AbstractSearch {
public:
    GreedyTour(Environment& env);
    
    virtual Instance& getInstance() { return instance; };
    
    virtual void reset(const Instance& init);
    
    virtual void run();
    
protected:

    Instance instance;
    
    SpotSearch& spotsearch;
    ProblemHelper helper;
    
    // Maximum number of nearest nodes to search for best neighbor
    unsigned maxk; 
    
    /**
     * Add or insert a spot into the tour.
     * @return the index of the inserted spot in the tour.
     */
    virtual unsigned insertSpot()=0;
  
    TourNode selectBestTourNode(NearestSpotList nearest, unsigned& insertAt, Config::NodeInsertMode insertMode = Config::NIM_ALWAYS_AFTER);

};



class GreedyNearestNeighbor : public GreedyTour {
public:
    
    GreedyNearestNeighbor ( Environment& env );
    
protected:
    
    virtual unsigned insertSpot();
};



class GreedyInsertHeuristic : public GreedyTour {
    
    Config::NodeInsertMode nodeInsertMode;
    
public:
    
    GreedyInsertHeuristic ( Environment& env );
    
protected:
        
    virtual unsigned insertSpot();
};



#endif