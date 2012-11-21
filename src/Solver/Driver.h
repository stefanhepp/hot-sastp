#ifndef DRIVER_H
#define DRIVER_H

#include "Support/Environment.h"
#include "Solver/GreedyTour.h"

class Driver
{
public:
    Driver(Environment& env) : env(env) {}
    
    const Environment& getEnvironment() const { return env; }
    
    /**
     * Prepare datastructures for the solvers (init data structures, ..)
     */
    void prepare();
    
    void solve();
    
    void writeSolution();
    
private:
    Environment& env;
    
};

#endif // DRIVER_H
