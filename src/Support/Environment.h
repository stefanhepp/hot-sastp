#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "Framework/SASTProblem.h"
#include "Framework/SASTPSolution.h"
#include "Support/SpotSearch.h"

class Config
{
public:
    Config();

    void printHelp();
    
    enum AlgorithmTag{
	AT_GREEDY_NN = 0,
        AT_GREEDY_IN = 1,
	AT_LOCALSEARCH = 2,
	AT_VND = 3,
	AT_GRASP_LS = 4,
        AT_GRASP_VND = 5,
	AT_GVNS = 6
    };
    
    enum NodeInsertMode {
	NIM_ALWAYS_BEFORE = 0,
	NIM_ALWAYS_AFTER = 1,
	NIM_SHORTEST_PATH = 2 
    };
      
    enum StepFunction {
	SF_RANDOM = 0,
	SF_NEXT = 1,
	SF_BEST = 2
    };
    
    enum StopCriterionTag{
	SC_DEFAULT = 0,
	SC_OTHER = 1
    };
    
    /**
     * @return the index of the first unhandled argument.
     */
    int parseArguments(int argc, char* argv[]);
    AlgorithmTag getAlgorithm() const {return _algorithm;}
  
    NodeInsertMode getNodeInsertMode() const { return _nodeInsertMode; }
  
    StepFunction getStepFunction() const { return _stepFunction; }
  
    /**
     * @return the maximum number of nearest spots that should be searched for.
     */
    unsigned getMaxKNearestSpots() const { return _maxKNearestSpots; }
    unsigned getMaxStepsNoChange() const { return _maxStepsWithNoChange; }
  
    std::string getInputFilename() const { return _inputFile;}
    std::string getOutputFilename() const {return _outputFile;}

    bool isVerbose() const { return _verbose; }
    
    bool doWriteDotFile() const { return _writeDot; }
    
    bool doUseShortOutput() const { return _shortOutput; }
    
private: 
    std::string _inputFile;
    std::string _outputFile;
    
    AlgorithmTag _algorithm;       
    NodeInsertMode _nodeInsertMode;
    StepFunction _stepFunction;
    
    unsigned _maxKNearestSpots;
    unsigned _maxStepsWithNoChange;
    
    bool _verbose;
    bool _writeDot;
    bool _shortOutput;
};

/**
 * The environment class contains a reference to a configuration and a problem instance,
 * as well as to common data structures.
 * The environment takes ownership of the problem and the data structures, i.e.,
 * they will be deleted when the environment is deleted.
 * 
 * TODO maybe use refcounter internally for problem and spotsearch
 */
class Environment
{    
    Config& config;
    
    SASTProblem *problem;
    SpotSearch  *spotsearch;
    
public:
    Environment(Config& config) : config(config), problem(0), spotsearch(0) {}
    ~Environment();
    
    const Config& getConfig() const { return config; }
    Config& getConfig() { return config; }
    
    const SASTProblem& getProblem() const { return *problem; }
    SASTProblem& getProblem() { return *problem; }
    
    const SpotSearch& getSpotSearch() const { return *spotsearch; }
    SpotSearch& getSpotSearch() { return *spotsearch; }
    
    void loadProblemFile(const std::string& filename);
    
    void initSpotSearch(unsigned maxk);
    
private:
    
    void deleteProblem();
    
};

#endif // ENVIRONMENT_H
