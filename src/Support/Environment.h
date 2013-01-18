#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "Framework/SASTProblem.h"
#include "Framework/SASTPSolution.h"
#include "Support/SpotSearch.h"
#include "Support/Instance.h"

#include <sys/time.h>

class Config
{
public:
    Config();

    void printHelp();
    
    enum AlgorithmTag{
	AT_GREEDY = 0,
	AT_LOCALSEARCH = 1,
	AT_VND = 2,
	AT_GRASP_LS = 3,
        AT_GRASP_VND = 4,
	AT_GVNS = 5,
        AT_ANT = 6, //dummy local search
	AT_ANT_LS = 7, //local search 
	AT_ANT_VND = 8 //VND search
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

    enum NeighborhoodTag{
      NT_ONE_OPT = 0,
      NT_EDGE_TWO_OPT = 1,
      NT_METHOD_TWO_OPT = 2,
      NT_NEAREST_TOUR_OPT = 3,
      NT_TWO_NODE_OPT = 4,
      NT_GREEDY_TOUR_OPT = 5
    };
    
    enum AntHeuristicTag {
      AH_NEAREST = 0,
      AH_INSERT = 1
    };
    
    /**
     * @return the index of the first unhandled argument.
     */
    int parseArguments(int argc, char* argv[]);
    
    AlgorithmTag getAlgorithm() const {return _algorithm;}
  
    NeighborhoodTag getNeighborhood() const { return _neighborhood; }
  
    NodeInsertMode getNodeInsertMode() const { return _nodeInsertMode; }
  
    StepFunction getStepFunction() const { return _stepFunction; }
  
    bool useGreedyInsertHeuristic() const { return _greedyInsertHeuristic; }
  
    /**
     * @return  initial tau set by the user
     */
    double getInitialTau() const { return _initialTau; }
  
    /**
     * @return the control parameter alpha ( used in for computing the ratio
     * between the pehromones and local information )
     */
    double getAntAlpha() const { return _alpha; }
  
    /**
     * @return the control parameter beta ( used in for computing the ratio
     * between the pehromones and local information )
     */
    double getAntBeta() const { return _beta; }
  
    /**
     * @return for how many steps we have to send the ants for solutions 
     */
    unsigned getNumberOfSteps() const { return _stepsToFinish; }
    
    /**
     * @return the number of ants in the population 
     */
    unsigned getNumberOfAnts() const { return _numberOfAnts; }
    
    /**
     * @return the number of best ants that update the pheromone matrix
     */
    unsigned getNumUpdateBestAnts() const {return _numUpdateBestAnts; } 
     
    /**
     * @return the persistence factor (1-rho), (0..1], default: 0.95
     */
    double getPersistFactor() const { return _persistFactor; }
    
    /**
     * @return get minimum tau value (default: 0.0)
     */
    double getTauMin() const { return _minTau; }
     
    /**
     * @return get maximum tau value (default: INFINITY)
     */ 
    double getTauMax() const { return _minTau; }
    
    unsigned getMinAntK() const { return _minAntK; }
    
    unsigned getMaxAntK() const { return _maxAntK; }
    
    /**
     * @return Heuristic to be used by ants
     */
    AntHeuristicTag getAntHeuristics() const { return _antHeuristic; }
    
    /**
     * @return true if we should use the improved tour to update the pheromones (default: false).
     */
    bool doImproveAntSolution() const { return _improveAntSolution; }
    
    double getAllowedTimeFactor() const { return _allowedTimeFactor; }
    
    /**
     * @return 0 to update only with iteration best ants, or k > 0 to update every kth round with the global best ant.
     * (default: 0)
     */
    unsigned getUpdateWithGlobalBest() const { return _updateWithGlobalBest; }
    
    bool doPrintBestAnts() const { return _printBestAnts; }
    
    /**
     * @return the maximum number of nearest spots that should be searched for.
     */
    unsigned getMaxKNearestSpots() const { return _maxKNearestSpots; }
    unsigned getMaxStepsNoChange() const { return _maxStepsWithNoChange; }
  
    std::string getInputFilename() const { return _inputFile;}
    std::string getOutputFilename() const {return _outputFile;}

    bool isVerbose() const { return _verbose; }
    
    bool isDebug() const { return _debug; }
    
    bool doWriteDotFile() const { return _writeDot; }
    
    bool doPrintCSVOutput() const { return _printCSVOutput; }
    
    bool doPrintAllSteps() const { return _printAllSteps; }
    
    bool doPrintSolution() const { return _printSolution; }
    
    /**
     * @return maximum allowed runtime in seconds
     */
    unsigned getMaxRuntime() const { return _maxRuntime; }
    double getAlpha() const { return _alphaRCL; }
    
private: 
    std::string _inputFile;
    std::string _outputFile;
    
    AlgorithmTag     _algorithm;       
    NeighborhoodTag  _neighborhood;
    NodeInsertMode   _nodeInsertMode;
    StepFunction     _stepFunction;
    bool             _greedyInsertHeuristic;
    
    unsigned _maxKNearestSpots;
    unsigned _maxStepsWithNoChange;
       
    unsigned _maxRuntime;
    double   _alphaRCL;
    
     /**
     * The following parameters are used in order to control the ant 
     * colony optimization 
     */
    
    double _initialTau;
    double _maxTau; 
    double _minTau;
    
    unsigned _minAntK;
    unsigned _maxAntK;
    
    /**
     * Parameters _alpha and _beta control the ratio between pheromones and 
     * local information ( _beta = 0 => fast convergence to a (random) path 
     * _alpha = 0 : classical greedy approach (nearest neighbor heuristic) 
     */
    double _alpha;
    double _beta; 
    
    /**
     * _stepsToFinish -- how many times do we send the ants to create paths 
     * this is involved in the termination criteria
     */
    unsigned _stepsToFinish; 
    
    /**
     * _numberOfAnts -- size of the population
     */
    unsigned _numberOfAnts;
    unsigned _numUpdateBestAnts;
    
    double   _persistFactor;
    AntHeuristicTag _antHeuristic;
    
    double _allowedTimeFactor;
    
    bool     _improveAntSolution;
    unsigned _updateWithGlobalBest;
    
    bool _verbose;
    bool _debug;
    bool _writeDot;
    bool _printSolution;
    bool _printCSVOutput;
    bool _printAllSteps;
    bool _printBestAnts;
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
    Instance    *empty;
    
    timeval startTime;
    
    std::string problemName;
    
    bool printSteps;
    
public:
    Environment(Config& config);
    ~Environment();
    
    const Config& getConfig() const { return config; }
    Config& getConfig() { return config; }
    
    const SASTProblem& getProblem() const { return *problem; }
    SASTProblem& getProblem() { return *problem; }
    
    const SpotSearch& getSpotSearch() const { return *spotsearch; }
    SpotSearch& getSpotSearch() { return *spotsearch; }
    
    const Instance& getEmptyInstance() const { return *empty; }
    
    void startTimer();
    
    /**
     * @return the current run time in seconds
     */
    float getCurrentTime();
    
    void printStepResult(const Instance& instance);
    
    void printSolution(const SASTPSolution& solution);
    
    bool setPrintSteps(bool enable);
    
    void loadProblemFile(const std::string& filename);
    
    void initSpotSearch(unsigned maxk);
    
private:
    
    void deleteProblem();
    
};

#endif // ENVIRONMENT_H
