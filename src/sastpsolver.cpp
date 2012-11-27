
#include "Framework/SASTProblem.h"
#include "Framework/SASTPSolution.h"
#include "Support/Environment.h"
#include "Solver/Driver.h"

#include <assert.h>
#include <stdlib.h>
#include <time.h>

#include <iostream>
using namespace std;
int main(int argc, char* argv[]) {
    
    Config config;
    
    int nextArg = config.parseArguments(argc, argv);
    
    // For now, config should handle all arguments
    assert(nextArg == argc);
   
    srand(time(NULL));
    Environment env(config);
    Driver driver(env);
    
    // Use the argument-provided filename to load the input file
    env.loadProblemFile(config.getInputFilename());
    
    // initialize data structures
    driver.prepare();
    
    // run the solver(s)
    driver.solve();

    // write solution to file
    driver.writeSolution();
    
    driver.checkSolution();
    
    return 0;
}
