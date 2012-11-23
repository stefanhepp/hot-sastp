#include "SpotSearch.h"

#include "Instance.h"

#include <assert.h>
#include <vector>
#include <algorithm>
#include <set>

SpotSearch::SpotSearch(const SASTProblem& problem)
: problem(problem)
{
}

SpotSearch::~SpotSearch()
{
    for (auto* vec: nearestSpots) {
	delete vec;
    }
}


void SpotSearch::initialize(unsigned int maxk)
{
    // TODO since we may want to skip unused spots, we cannot fix a maxk for the lookup lists
    //      So we just add all spots to the lookup table, making the size quadratic.
    //      To improve, we should have two separate lookup methods: either lists if we do not 
    //      ignore used spots, and a kd-tree or something to look for unused spots.
    
    unsigned listSize = problem.getSpots().size();
    
    addNearestSpots(problem.getStartAsSpot(), listSize);
    
    for (const auto* spot : problem.getSpots()) {
	addNearestSpots(*spot, listSize);
    }
}

void SpotSearch::addNearestSpots(const Spot& spot, unsigned int maxk)
{
    std::vector<unsigned>* nearest = new std::vector<unsigned>();
    nearest->reserve(maxk);
    
    // Quick'n'dirty hack to create initial lookup map
    std::vector<double> dist;
    dist.reserve(maxk);
    double maxDist = 0.0;
    unsigned spotIndex = nearestSpots.size() - 1;
    
    unsigned index = 0;
    for (const auto* s : problem.getSpots()) {
	if (index == spotIndex) continue;
	
	double d = problem.getDistance(spot, *s);
	
	if (d >= maxDist) {
	    // large distance, but we might still have room for it?
	    if (nearest->size() < maxk) {
		nearest->push_back(index);
		dist.push_back(d);
		maxDist = d;
	    }
	    
	} else {
	    // found a smaller one than the k-largest spot, insert into list
	    if (nearest->size() == maxk) {
		nearest->pop_back();
		dist.pop_back();
	    }
	    
	    // find the index where to insert the new spot
	    auto distIt = std::upper_bound(dist.begin(), dist.end(), d);
	    
	    dist.insert(distIt, d);
	    nearest->insert(nearest->begin() + (distIt - dist.begin()), index);

	    maxDist = dist.back();
	}
	
	index++;
    }
    
    nearestSpots.push_back(nearest);
}


NearestSpot SpotSearch::findNearestSpot(const Instance& instance, int spot, bool skipUsed)
{
    std::vector<unsigned>* nearest = nearestSpots[spot+1];
    
    for (unsigned nearestspot : *nearest) {
	if (skipUsed && instance.containsSpot(nearestspot)) continue;
	
	return std::make_pair(spot,nearestspot);
    }
    // Should never be reached
    assert(false && "Trying to find nearest spot, but all spots are used");
    return std::make_pair(spot,-1);
}

NearestSpot SpotSearch::findNearestSpot(const Instance& tour, bool skipUsed)
{
    // initialize with nearest spot to hotel
    int tournode = -1;
    unsigned nearest = findNearestSpot(tour, tournode, skipUsed).second;
    
    double dist = problem.getDistance(problem.getStartAsSpot(), problem.getSpot(nearest));
    
    unsigned index = 0;
    for(const auto& node : tour.getTour()) {
	// get nearest spot for every node in the tour, compare with current nearest spot
	unsigned spotIdx = findNearestSpot(tour, node.spot, skipUsed).second;
	double spotDist = problem.getDistance(problem.getSpot(node.spot), problem.getSpot(spotIdx));
	
	if (spotDist <= dist) {
	    tournode = index;
	    nearest = spotIdx;
	    dist = spotDist;
	}
	
	index++;
    }
    
    return std::make_pair(tournode, nearest);
}


NearestSpotList SpotSearch::findNearestSpots(const Instance& tour, int spot, unsigned int k, bool skipUsed)
{
    std::vector<unsigned>* nearest = nearestSpots[spot+1];

    // return a vector containing the first k entries
    assert(k <= nearest->size());
    
    NearestSpotList pairs;

    if (skipUsed) {
	pairs.reserve(k);
	for (unsigned nearestspot : *nearest) {
	    if (tour.containsSpot(nearestspot)) continue;
	    
	    pairs.push_back( std::make_pair(spot, nearestspot) );
	    
	    if (pairs.size() >= k) break;
	}
    } else {
	pairs.resize(k);
	std::transform(nearest->begin(), nearest->begin() + k, pairs.begin(), 
			[spot](unsigned ns){ return std::make_pair(spot, ns); } );
    }
    
    return pairs;
}

NearestSpotList SpotSearch::findNearestSpots(const Instance& tour, unsigned int k, bool skipUsed)
{
    NearestSpotList nearest;
    nearest.reserve(k);
    
    std::vector<double> dist;
    dist.reserve(k);
    
    // initialize with nearest spots to hotel
    nearest = findNearestSpots(tour, -1, k, skipUsed);
    for (const auto& spot : nearest) {
	dist.push_back(problem.getDistance(problem.getStartAsSpot(), problem.getSpot(spot.second)));
    }
    
    // keep track of the maximum distance of the k neighbors found so far
    double maxDist = dist.back();
    
    int tourIndex = 0;
    for(const auto& node : tour.getTour()) {
	// get k nearest spots for every node in the tour, compare with current nearest spots
	std::vector<unsigned>* spots = nearestSpots[node.spot+1];

	for (unsigned spotIndex : *spots) {
	    if (skipUsed && tour.containsSpot(spotIndex)) continue;
	    
	    double spotDist = problem.getDistance(problem.getSpot(node.spot), problem.getSpot(spotIndex));
	    
	    // check if the list is already at size k. we can assume that if !skipUsed
	    bool listFull = !skipUsed || dist.size() >= k;
	    
	    // All other spots will actually be further out, so we can stop looking
	    // This will implicitly stop after at most k iterations, dist(spots[k]) will always be > maxDist!
	    // (otherwise we would have added all k nearer spots to our list already)
	    if (listFull && spotDist >= maxDist) break;

	    // need to insert new spot into list.
	    if (listFull) {
		nearest.pop_back();
		dist.pop_back();
	    }
	    
    	    auto distIt = std::upper_bound(dist.begin(), dist.end(), spotDist);
	    
	    dist.insert(distIt, spotDist);
	    nearest.insert(nearest.begin() + (distIt - dist.begin()), std::make_pair(tourIndex, spotIndex));
	    
	    maxDist = dist.back();
	}
	
	tourIndex++;
    }
    
    return nearest;
}

NearestNodesList SpotSearch::findNearestTourNodes(const Instance& tour, unsigned int k)
{
    NearestNodesList nearest;
    nearest.reserve(k);
    
    std::vector<double> dist;
    dist.reserve(k);
    
    double maxDist = 0.0;
    
    int tourIndex = 0;
    for (const auto& node : tour.getTour()) {
	std::vector<unsigned>* spots = nearestSpots[node.spot+1];
	
	for (unsigned spotIndex : *spots) {
	    // To avoid adding duplicate entries, only add pairs where first.spot < second.spot.
	    if (node.spot > spotIndex) continue;
	    
	    // skip all spots that are not on the tour
	    if (!tour.containsSpot(spotIndex)) continue;
	    
	    double spotDist = problem.getDistance(problem.getSpot(node.spot), problem.getSpot(spotIndex));
	    
	    if (dist.size() >= k && spotDist >= maxDist) break;
	    
	    // We need to get the index of the node in the tour for the nearest spot
	    unsigned nearestnode = tour.getNodeIndex(spotIndex);
	    
	    if (dist.size() >= k) {
		nearest.pop_back();
		dist.pop_back();
	    }

    	    auto distIt = std::upper_bound(dist.begin(), dist.end(), spotDist);
	    
	    dist.insert(distIt, spotDist);
	    nearest.insert(nearest.begin() + (distIt - dist.begin()), std::make_pair(tourIndex, nearestnode));
	    
	    maxDist = dist.back();	    
	}
	
	tourIndex++;
    }
    
    return nearest;
}
