#include "Driver.h"

#include "Support/SpotSearch.h"

void Driver::prepare()
{
    // Initialize the SpotSearch
    env.initSpotSearch( env.getConfig().getMaxKNearestSpots() );
    
}

void Driver::solve()
{

}

void Driver::writeSolution()
{

}

