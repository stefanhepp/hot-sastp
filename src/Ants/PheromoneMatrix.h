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
     : problem(problem), defaultTau(initTau) 
    {
	reset(initTau); 
    }
    
    ~PheromoneMatrix() { destroy(); }
    
    void reset(float initTau);
    
    float getTau(const TourNode start, const TourNode end) const;
    
    void setTau(const TourNode start, const TourNode end);
    
private:
    // destroy all used memory
    void destroy();
};

#endif // PHEROMONEMATRIX_H
