#include "GreedyTour.h"

#include "Support/Instance.h"
#include "Framework/SASTProblem.h"

GreedyTour::GreedyTour(const SASTProblem& problem)
: instance(problem)
{
    // instance is an empty tour to begin with
}

void GreedyTour::run()
{
    size_t numSpots = getInstance().getProblem().getSpots().size();
    
    while (instance.getTourLength() < numSpots) {
	
	// add a new spot into the tour
	unsigned newSpot = insertSpot();
	
	// If inserting the new spot makes the tour too long, do not add it to the tour and stop
	if (!instance.isValid()) {
	    instance.deleteNode(newSpot);
	    break;
	}
    }
    
}

unsigned GreedyNearestNeighbor::insertSpot() {
    
}

