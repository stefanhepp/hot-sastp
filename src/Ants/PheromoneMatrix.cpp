#include "PheromoneMatrix.h"


void PheromoneMatrix::reset(float initTau)
{
    defaultTau = initTau;

    destroy();
    
    // TODO initialize data structures
    
}

float PheromoneMatrix::getTau(const TourNode start, const TourNode end) const
{

    return defaultTau;
}

void PheromoneMatrix::setTau(const TourNode start, const TourNode end, double tau)
{
    // clamp first
    if (tau < minTau) tau = minTau;
    if (tau > maxTau) tau = maxTau;
    
    // now set the value, construct data structures lazily
    
    
}

void PheromoneMatrix::addTau(const TourNode start, const TourNode end, double deltaTau)
{
    setTau(start, end, getTau(start, end) + deltaTau);
}

void PheromoneMatrix::evaporate()
{
    defaultTau = persistFactor * defaultTau;
    
    
}

void PheromoneMatrix::destroy()
{

}
