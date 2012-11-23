#ifndef DRIVER_H
#define DRIVER_H

#include "Framework/SASTPSolution.h"
#include "Support/Environment.h"

class Driver
{
public:
    Driver(Environment& env) : env(env), solution(0) {}
    ~Driver();
    
    const Environment& getEnvironment() const { return env; }
    
    SASTPSolution& getSolution() { return *solution; }
    
    /**
     * Prepare datastructures for the solvers (init data structures, ..)
     */
    void prepare();
    
    void solve();
    
    void checkSolution();
    
    void writeSolution();
    
private:
    Environment& env;
    
    SASTPSolution *solution;
    
};

#endif // DRIVER_H
