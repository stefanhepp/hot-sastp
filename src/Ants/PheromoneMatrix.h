#ifndef PHEROMONEMATRIX_H
#define PHEROMONEMATRIX_H

#include "Framework/SASTProblem.h"
#include "Support/Instance.h"
#include "Support/Environment.h"

class PheromoneMatrix
{
    SASTProblem &problem;
    
    // Value used for uninitialized data
    double defaultTau;
    
    double persistFactor;
    double minTau;
    double maxTau;
    
public:
   PheromoneMatrix(Environment& env);
    
    ~PheromoneMatrix();
    
    void reset(float initTau);
    
    float getTau(const TourNode start, const TourNode end) const;
    
    void setTau(const TourNode start, const TourNode end, double tau);
    
    void addTau(const TourNode start, const TourNode end, double deltaTau);
    
    void evaporate();
    
private:
    // destroy all used memory
    void destroy();
};

#endif // PHEROMONEMATRIX_H
