#ifndef PROBLEMHELPER_H
#define PROBLEMHELPER_H

#include "Framework/SASTProblem.h"
#include "Support/SpotSearch.h"
#include "Support/Environment.h"
#include "Support/Instance.h"

/**
 * Collection of a bunch of useful functions related to calculating improvements and finding spots or methods.
 */
class ProblemHelper
{
    SASTProblem& problem;
    SpotSearch& spotsearch;
    
public:
    ProblemHelper(SASTProblem& problem, SpotSearch& spotsearch) : problem(problem), spotsearch(spotsearch) {}
    
    /**
     * Get the difference of the tour length when inserting a spot into the tour at a given point.
     * @param tour the current tour
     * @param index the node after/before which to insert the new spot
     * @param newspot the spot to insert
     * @param insertMode insert before/after the node or select the best way to insert depending on tour length
     * @param bestInsert will be set to the index where the new spot should be inserted (handles origin properly)
     * @return the tour length difference
     */
    double getInsertDeltaTourLength(const Instance& tour, unsigned index, const Spot& newspot, 
				     Config::NodeInsertMode insertMode, unsigned &bestInsert);
    
    /**
     * Calculate the ratio of delta satisfaction to delta time for inserting a new method into the tour 
     * with additional travel distance deltaTime.
     * @param currRemainingStamina the current remaining stamina at the end of the tour (excluding rests)
     * @param m the new method to insert
     * @param deltaTour the tour length increase
     * @param deltaTime will be set to the time increase of the tour for visiting the tour, traveling, and additional 
     *                  resting time (based on the current remaining stamina of the tour instance.
     * @return delta satisfaction / delta total time.
     */
    double calcInsertSatisfactionTimeRatio(double currRemainingStamina, const Method& m, double deltaTour, double& deltaTime);
};

#endif // PROBLEMHELPER_H
