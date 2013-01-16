#include "PheromoneMatrix.h"

void PheromoneMatrix::reset(float initTau)
{
    defaultTau = initTau;

    destroy();
    
    // TODO reset data structures
}

float PheromoneMatrix::getTau(const TourNode start, const TourNode end) const
{

    return defaultTau;
}

void PheromoneMatrix::setTau(const TourNode start, const TourNode end)
{

}

void PheromoneMatrix::destroy()
{

}
