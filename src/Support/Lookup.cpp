#include "Lookup.h"

#include <assert.h>
#include <vector>
#include <algorithm>

Lookup::Lookup(const SASTProblem& problem)
: problem(problem)
{
}

Lookup::~Lookup()
{
    for (auto* vec: nearestSpots) {
	delete vec;
    }
}


void Lookup::initialize(unsigned int maxk)
{
    addNearestSpots(problem.getStartAsSpot(), maxk);
    
    for (const auto* spot : problem.getSpots()) {
	addNearestSpots(*spot, maxk);
    }
}

void Lookup::addNearestSpots(const Spot& spot, unsigned int maxk)
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


unsigned int Lookup::findNearestSpot(int spot)
{
    std::vector<unsigned>* nearest = nearestSpots[spot+1];
    
    return nearest->front();
}

std::vector< unsigned int > Lookup::findNearestSpots(int spot, unsigned int k)
{
    std::vector<unsigned>* nearest = nearestSpots[spot+1];

    // return a vector containing the first k entries
    assert(k <= nearest->size());
    
    return std::vector<unsigned>(nearest->begin(), nearest->begin() + k);
}

