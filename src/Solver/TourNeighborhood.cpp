#include "TourNeighborhood.h"

NearestTourNodes::NearestTourNodes(Environment& env, unsigned int maxRemove, bool removeConsecutive, 
				   unsigned int maxk, bool insertConsecutive)
: Neighborhood(env), maxRemove(maxRemove), removeConsecutive(removeConsecutive), maxk(maxk), 
  insertConsecutive(insertConsecutive)
{
}

std::string NearestTourNodes::getName() const
{
    // TODO generate name based on parameters
    return "NearestTourNodes";
}

bool NearestTourNodes::performStep(Instance& instance, Config::StepFunction stepFunction, bool alwaysApply)
{
    
}
