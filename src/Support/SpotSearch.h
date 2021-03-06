#ifndef LOOKUP_H
#define LOOKUP_H

#include "Framework/SASTProblem.h"
#include "Instance.h"

#include <vector>

/**
 * First is the index of the node in the tour to which the spot is nearest (-1 for origin).
 * Second is the index of the nearest spot.
 */
typedef std::pair<int,unsigned>    NearestSpot;
typedef std::vector< NearestSpot > NearestSpotList;

/**
 * Pair of nodes that are near to each other.
 */
typedef std::pair<unsigned,unsigned> NearestNodes;
typedef std::vector< NearestNodes > NearestNodesList;

class SpotSearch
{
public:
    SpotSearch(const SASTProblem& problem);
    ~SpotSearch();
    
    /**
     * Initialize the lookup data structures.
     * @param maxk the maximum k neighbors that we need to find.
     */
    void initialize(unsigned maxk);
    
    /**
     * Find nearest spot to a given spot.
     * @param tournode the index of the spot in the tour, or -1 for the origin.
     * @return the index of the nearest spot.
     */
    NearestSpot findNearestSpot(const Instance& tour, int tournode, bool skipUsed = true);
    
    /**
     * Find nearest spot to a tour.
     * @return a pair <tournode,nearest> where tournode is the index of the tour nodes to which the spot is nearest, and the
     *  index of the nearest spot.
     */
    NearestSpot findNearestTourSpot(const Instance& tour, bool skipUsed = true);
    
    /**
     * Find k nearest spots to a given spot.
     * @param tournode the index of the spot in the tour, or -1 for the origin.
     * @return a vector containing the indices of the k nearest spots.
     */
    NearestSpotList findNearestSpots(const Instance& tour, int tournode, unsigned k, bool skipUsed = true);
    
    /**
     * Find k nearest spots to a tour.
     * @return a list of pairs <tournode,nearest> where tournode is the index of the tour nodes to which the spot is nearest, and the
     *  index of the nearest spot.
     */
    NearestSpotList findNearestTourSpots(const Instance& tour, unsigned k, bool uniqueSpots = false, bool skipUsed = true);
    
    /**
     * Find k nearest non-adjacent tour nodes.
     */
    NearestNodesList findNearestTourNodes(const Instance& tour, unsigned k);
    
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
