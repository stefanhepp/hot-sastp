#include "SpotSearch.h"

#include "Instance.h"

#include <assert.h>
#include <vector>
#include <algorithm>

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
    addNearestSpots(problem.getStartAsSpot(), maxk);
    
    for (const auto* spot : problem.getSpots()) {
	addNearestSpots(*spot, maxk);
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
	}
	
	index++;
    }
    
    nearestSpots.push_back(nearest);
}


NearestSpot SpotSearch::findNearestSpot(int spot)
{
    std::vector<unsigned>* nearest = nearestSpots[spot+1];
    
    return std::make_pair(spot,nearest->front());
}

NearestSpot SpotSearch::findNearestSpot(const Instance& tour)
{
    // initialize with nearest spot to hotel
    int tournode = -1;
    unsigned nearest = findNearestSpot(tournode).second;
    
    double dist = problem.getDistance(problem.getStartAsSpot(), problem.getSpot(nearest));
    
    unsigned index = 0;
    for(const auto& node : tour.getTour()) {
	// get nearest spot for every node in the tour, compare with current nearest spot
	unsigned spotIdx = findNearestSpot(node.spot).second;
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


NearestSpotList SpotSearch::findNearestSpots(int spot, unsigned int k)
{
    std::vector<unsigned>* nearest = nearestSpots[spot+1];

    // return a vector containing the first k entries
    assert(k <= nearest->size());
    
    NearestSpotList pairs;
    pairs.resize(k);
    
    std::transform(nearest->begin(), nearest->begin() + k, pairs.begin(), 
		   [spot](unsigned ns){ return std::make_pair(spot, ns); } );
    
    return pairs;
}

NearestSpotList SpotSearch::findNearestSpots(const Instance& tour, unsigned int k)
{
    NearestSpotList nearest;
    nearest.reserve(k);
    
    std::vector<double> dist;
    dist.reserve(k);
    
    // initialize with nearest spots to hotel
    nearest = findNearestSpots(-1, k);
    for (const auto& spot : nearest) {
	dist.push_back(problem.getDistance(problem.getStartAsSpot(), problem.getSpot(spot.second)));
    }
    
    double maxDist = dist.back();
    
    int tourIndex = 0;
    for(const auto& node : tour.getTour()) {
	// get k nearest spots for every node in the tour, compare with current nearest spots
	std::vector<unsigned>* spots = nearestSpots[node.spot+1];

	for (unsigned spotIndex : *spots) {
	    double spotDist = problem.getDistance(problem.getSpot(node.spot), problem.getSpot(spotIndex));
	    
	    // All other spots will actually be further out, so we can stop looking
	    // This will implicitly stop after at most k iterations, dist(spots[k]) will always be > maxDist!
	    if (spotDist >= maxDist) break;

	    // need to insert new spot into list. We can assume that it has already size k
	    nearest.pop_back();
	    dist.pop_back();
	    
    	    auto distIt = std::upper_bound(dist.begin(), dist.end(), spotDist);
	    
	    dist.insert(distIt, spotDist);
	    nearest.insert(nearest.begin() + (distIt - dist.begin()), std::make_pair(tourIndex, spotIndex));
	    
	    maxDist = spotDist;
	}
	
	tourIndex++;
    }
    
    return nearest;
}
