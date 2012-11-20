#ifndef LOOKUP_H
#define LOOKUP_H

#include "Framework/SASTProblem.h"

#include <vector>

class Lookup
{
public:
    Lookup(const SASTProblem& problem);
    ~Lookup();
    
    /**
     * Initialize the lookup data structures.
     * @param maxk the maximum k neighbors that we need to find.
     */
    void initialize(unsigned maxk);
    
    /**
     * Find nearest spot to a given spot.
     * @param spot the index of the spot, or -1 for the origin.
     * @return the index of the nearest spot.
     */
    unsigned findNearestSpot(int spot);
    
    /**
     * Find k nearest spots to a given spot.
     * @param spot the index of the spot, or -1 for the origin.
     * @return a vector containing the indices of the k nearest spots.
     */
    std::vector<unsigned> findNearestSpots(int spot, unsigned k);
    
private:
    const SASTProblem& problem;
    
    /**
     * A list of all k nearest spots per spot.
     * Note that the index to this vector is shifted by one. Entry 0 corresponds to the hotel.
     */
    std::vector<std::vector<unsigned>*> nearestSpots;
    
    void addNearestSpots(const Spot& spot, unsigned maxk);
};

#endif // LOOKUP_H
