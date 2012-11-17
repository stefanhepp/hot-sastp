#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "Framework/SASTProblem.h"
#include "Framework/SASTPSolution.h"

class Environment
{
    SASTProblem *problem;
    SASTPSolution *solution;

public:
    Environment();
    ~Environment();
    
    void usage(char* programName);

    /**
     * @return 0 on success, error code otherwise.
     */
    int parseArguments(int argc, char* argv[]);
    
    SASTProblem& getProblem() { return *problem; };
    SASTPSolution& getSolution() { return *solution; };
    
    
};

#endif // ENVIRONMENT_H
