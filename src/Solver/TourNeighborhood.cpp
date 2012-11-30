#include "TourNeighborhood.h"

#include <vector>

using namespace std;

NearestNodesNeighborhood::NearestNodesNeighborhood(Environment& env, unsigned int maxk, bool insertConsecutive)
: Neighborhood(env), maxk(maxk), insertConsecutive(insertConsecutive)
{
}


NearestTourExchange::NearestTourExchange(Environment& env, unsigned int maxRemove, unsigned int maxk, bool insertConsecutive)
: NearestNodesNeighborhood(env, maxk, insertConsecutive), maxRemove(maxRemove)
{
}

std::string NearestTourExchange::getName() const
{
    // TODO add option values to name
    return "Nearest-Tour-exchange"; 
}

bool NearestTourExchange::performStep(Instance& instance, Config::StepFunction stepFunction, bool alwaysApply)
{

    
    return false;
}



TwoNodesTourExchange::TwoNodesTourExchange(Environment& env, unsigned int maxk, bool insertConsecutive)
: NearestNodesNeighborhood(env, maxk, insertConsecutive)
{
}

std::string TwoNodesTourExchange::getName() const
{
    // TODO add option values to name
    return "2-Nodes-exchange";
}

bool TwoNodesTourExchange::performStep(Instance& instance, Config::StepFunction stepFunction, bool alwaysApply)
{
    
    
    return false;
}
