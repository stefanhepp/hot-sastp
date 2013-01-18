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
 
    bool debug;
    
    // Spot_i->Method_i->Spot_j->Method_j->Tau_ij
    double ****matrix;
    
public:
   PheromoneMatrix(Environment& env);
    
    ~PheromoneMatrix();
    
    void reset(float initTau);
    
    void setMinTau(double tau) { minTau = tau; }
    
    void setMaxTau(double tau) { maxTau = tau < minTau ? minTau : tau; }
    
    float getTau(const TourNode start, const TourNode end) const;
    
    void setTau(const TourNode start, const TourNode end, double tau);
    
    void addTau(const TourNode start, const TourNode end, double deltaTau);
    
    void evaporate();
    
private:
    // destroy all used memory
    void destroy();
};

#endif // PHEROMONEMATRIX_H
