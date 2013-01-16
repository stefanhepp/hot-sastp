#ifndef PHEROMONEMATRIX_H
#define PHEROMONEMATRIX_H

#include "Framework/SASTProblem.h"
#include "Support/Instance.h"

class PheromoneMatrix
{
    SASTProblem &problem;
    
    // Value used for uninitialized data
    float defaultTau;
    
public:
    PheromoneMatrix(SASTProblem &problem, float initTau)
     : problem(problem), defaultTau(initTau) {}
    
    ~PheromoneMatrix() { destroy(); }
    
    void reset(float initTau);
    
    float getTau(TourNode start, TourNode end);
    
    void setTau(TourNode start, TourNode end);
    
private:
    // destroy all used memory
    void destroy();
};

#endif // PHEROMONEMATRIX_H
