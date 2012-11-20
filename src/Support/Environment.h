#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "Framework/SASTProblem.h"
#include "Framework/SASTPSolution.h"

using namespace std;

class Environment
{
    SASTProblem *problem;
    SASTPSolution *solution;
   
    
public:
    Environment();
    ~Environment();
    
    void usage(char* programName);
    
    enum AlgorithmTag{
      GREEDY = 0,
      NEIGHBOUR1 = 1,
      NEIGHBOUR2 = 2
      };
    
    /**
     * @return 0 on success, error code otherwise.
     */
    int parseArguments(int argc, char* argv[]);
    
    SASTProblem& getProblem() { return *problem; };
    SASTPSolution& getSolution() { return *solution; };
    AlgorithmTag getAlgorithm() {return _algorithm;};
  
    string getInputFilename() {return _inputFile;};
    string getOutputFilename() {return _outputFile;};
private: 
    string _inputFile;
    string _outputFile;
    AlgorithmTag _algorithm;       
};

#endif // ENVIRONMENT_H
