#include "Environment.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include <sys/time.h>
#include <assert.h>
#include <unistd.h>
#include "optionparser.h"

#define MAXDOUBLE 100000.0
using namespace std;

Config::Config()
{
    // Set some default values
    _algorithm = AT_ANT_LS;
    _neighborhood = NT_EDGE_TWO_OPT;
    _maxKNearestSpots = 10;
    _nodeInsertMode = NIM_SHORTEST_PATH;
    _stepFunction = SF_NEXT;
    _vndMode = 0;
    _greedyInsertHeuristic = true;
    _verbose = false;
    _debug = false;
    _writeDot = false;
    _maxStepsWithNoChange = 25u;
    _printAllSteps = false;
    _printCSVOutput = false;
    _printSolution = false;
    _alphaRCL = 1;
    _maxRuntime = 1800;
    //the following options have to be decided upon -- this are just some initials 
    _stepsToFinish = 10000;
    _numberOfAnts = 20; 
    _numUpdateBestAnts = 1;
    _initialTau = 1;
    _alpha = 1; 
    _beta = 2;

    _antHeuristic = AH_NEAREST;
    _minTau = 0.0;
    _maxTau = MAXDOUBLE;
    
    _minAntK = 1;
    _maxAntK = _numberOfAnts*2;
    
    _allowedTimeFactor = 1.0;
    _persistFactor = 0.8;
    _improveAntSolution = false;
    _updateWithGlobalBest = 0;
}


struct Arg: public option::Arg {
    static void printError (const char* msg1, const option::Option& opt, const char* msg2) {
        fprintf (stderr, "%s", msg1);
        fwrite (opt.name, opt.namelen, 1, stderr);
        fprintf (stderr, "%s", msg2);
    }

    static option::ArgStatus Unknown (const option::Option& option, bool msg) {
        if (msg) printError ("Unknown option '", option, "'\n");
        return option::ARG_ILLEGAL;
    }

    static option::ArgStatus Required (const option::Option& option, bool msg) {
        if (option.arg != 0)
            return option::ARG_OK;

        if (msg) printError ("Option '", option, "' requires an argument\n");
        return option::ARG_ILLEGAL;
    }

    static option::ArgStatus NonEmpty (const option::Option& option, bool msg) {
        if (option.arg != 0 && option.arg[0] != 0)
            return option::ARG_OK;

        if (msg) printError ("Option '", option, "' requires a non-empty argument\n");
        return option::ARG_ILLEGAL;
    }

    static option::ArgStatus Numeric (const option::Option& option, bool msg) {
        char* endptr = 0;
        if (option.arg != 0 && strtol (option.arg, &endptr, 10)) {};
        if (endptr != option.arg && *endptr == 0)
            return option::ARG_OK;

        if (msg) printError ("Option '", option, "' requires a numeric argument\n");
        return option::ARG_ILLEGAL;
    }
    
    static option::ArgStatus Double (const option::Option& option, bool msg) {
        char* endptr = 0;
        if (option.arg != 0 && strtof (option.arg, &endptr)) {};
        if (endptr != option.arg && *endptr == 0)
            return option::ARG_OK;

        if (msg) printError ("Option '", option, "' requires a floating point argument\n");
        return option::ARG_ILLEGAL;	
    }

};

enum optionIndex {UNKNOWN, HELP, ALGORITHM, NEIGHBORHOOD, GREEDY_NN, KNEAREST, VERBOSE, DEBUG, DOT, PRINT_CSV, PRINT_ALL_STEPS, VNDMODE,
                  TIMEOUT, INSERTMODE, STEP, MAXSTEPS, ALPHA, ANTALPHA, ANTBETA, ANTTAU, ANTSTEPS, ANTNUMBEROFTHEM, 
                  ANTMAXTAU, ANTMINTAU,ANTHEURISTICTAG, ANTPERSITENCE, ANTUPDATEBEST, UPDATEGLOBAL, IMPROVEANTS, PRINTBESTANTS,
		  ANTMINK, ANTMAXK, ALLOWEDTIMEFACTOR  
};
const option::Descriptor usage[] = {
    {
        UNKNOWN, 0, "", "",        Arg::Unknown, "USAGE: sastpsolver [options] inputFile outputFile\n\n"
        "Options:"
    },
    { HELP, 0, "h", "help", Arg::None, "  \t--help  \tPrint usage and exit." },
    {
        ALGORITHM , 0, "a", "algorithm", Arg::Numeric, "  -a <arg>, \t--algorithm=<arg>"
        "  \tTakes an integer argument.\n \tOptions:\n \tAT_GREEDY = 0,\n \tAT_LOCALSEARCH = 1,"
        "\n \tAT_VND = 2, \n \tAT_GRASP_LS = 3, \n \tAT_GRASP_VND = 4,\n \tAT_GVNS = 5,"
	"\n \tAT_ANT = 6,\n \tAT_ANT_LS = 7,\n \tAT_ANT_VND = 8"
    },
    { NEIGHBORHOOD, 0, "n", "neighborhood", Arg::Numeric, "  -n <arg>, \t--neighborhood=<arg>"
        " \tSelect the neighborhood.\n \tOptions:\n \tNT_ONE_OPT = 0,\n \tNT_EDGE_TWO_OPT = 1,\n \tNT_METHOD_TWO_OPT = 2"
	",\n \tNT_NEAREST_TOUR_OPT = 3,\n \tNT_TWO_NODES_OPT = 4"
    },
    { VNDMODE, 0, "z", "vndmode", Arg::Numeric, "  -z <unsigned> \t--vndmode=<unsigned> \tUse the given configuration for the VND neighborhoods." },
    
    { GREEDY_NN, 0, "g", "greedy-nearest", Arg::None, "  -g, \t--greedy-nearest \tUse nearest neighbor instead of insert heuristic for greedy. " },
    {
        KNEAREST, 0, "k", "knear", Arg::Numeric, "  -k <arg>, \t--knear=<arg> \tMust have as an"
        " argument a number -maximal KNearest Spots."
    },
    { ALPHA , 0, "", "alpha", Arg::NonEmpty, "   \t--alpha=<arg> \tAlpha for the construction of Restricted Candidates List. Values in [0..1]"},
    
    { DOT, 0, "d", "dot", Arg::None, "  -d, \t--dot \tGenerate dot file from solution. " },
    { VERBOSE, 0, "v", "verbose", Arg::None, "  -v, \t--verbose \tBe verbosive."},
    { DEBUG, 0, "x", "debug", Arg::None, "  -x, \t--debug \tPrint out debug infos." },
    { PRINT_CSV, 0, "c", "csv", Arg::None, "  -c, \t--csv \tPrint result as CSV output." },

    { PRINT_ALL_STEPS, 0, "p", "allsteps", Arg::None, "  -p, \t--allsteps \tPrint result of all intermediate steps." },

    {
        INSERTMODE, 0, "i", "inMode", Arg::Numeric, "  -i <arg>, \t--inMode=<arg> \tNode insertion"
        "mode can take the following options:\n "
        "\tNIM_ALWAYS_BEFORE = 0, \n \tNIM_ALWAYS_AFTER = 1, \n \tNIM_SHORTEST_PATH = 2."
    },
    {
        STEP, 0, "s", "step", Arg::Numeric, "  -s <arg>, \t--step=<arg> \tStep function to be used. Values:\n"
        "\tSF_RANDOM = 0,\n\tSF_NEXT = 1,\n \tSF_BEST = 2."
    },
    {
	MAXSTEPS, 0, "m", "maxSteps", Arg::Numeric, "  -m <arg>, \t--maxSteps=<arg> \tMaximal number of steps with no improvement."
    },
    { TIMEOUT, 0, "t", "timeout", Arg::Numeric, "  -t <secs>, \t--timeout=<secs> \tTimeout for search in seconds." },
    
    { ANTALPHA, 0, "A", "antAlpha", Arg::Double, "  -A <double>, \t--antAlpha=<double> \tAlfa used for limiting the internal ratio." },
    
    { ANTBETA, 0, "B", "antBeta", Arg::Double, "  -B <double>, \t--antBeta=<double> \tBeta used for limiting the internal ratio." },
    
    { ANTTAU, 0, "T", "antTau", Arg::Double, "  -T <double>, \t--antTau=<double> \tInitial tau used in the ACO." },
    
    { ANTMINTAU, 0, "C", "minTau", Arg::Double, "  -C <double>, \t--minTau=<double> \tMin tau used in the ACO." },
    
    { ANTMAXTAU, 0, "D", "maxTau", Arg::Double, "  -D <double>, \t--maxTau=<double> \tMax tau used in the ACO. Use -1 to set it to current global best satisfaction." },
    
    { ANTMINK, 0, "J", "minAntK", Arg::Numeric, "  -J <unsigned> \t--minAntK=<unsigned> \tMinimum k value for ants." },
    
    { ANTMAXK, 0, "K", "maxAntK", Arg::Numeric, "  -K <unsigned> \t--maxAntK=<unsigned> \tMaximum k value for ants." },
    
    { ANTPERSITENCE, 0, "P", "persist", Arg::Double,   "  -P <double>, \t--persist=<double> \tPersistence factor used in the ACO." },
    
    { ALLOWEDTIMEFACTOR, 0, "F", "allowedTime", Arg::Double,   "  -F <double>, \t--allowedTime=<double> \tFactor of time to allow to exceed by the ants." },

    { ANTNUMBEROFTHEM, 0, "N", "ants", Arg::Numeric, "  -N <integer>, \t--ants=<integer> \tNumber of ants in the population." },
   
    { ANTSTEPS, 0, "S", "Steps", Arg::Numeric, "  -S <integer>, \t--Steps=<integer> \tHow many times we send the ants for solutions." },
   
    { ANTUPDATEBEST, 0, "W", "updBest", Arg::Numeric, "  -W <unsigned> \t--updBest=<unsigned> \tNumber of best ants used to influence the pheromone matrix." },
    
    { ANTHEURISTICTAG, 0, "H", "heuTag", Arg::Numeric, "  -H <unsigned> \t--heuTag=<unsigned> \tNeareast Neighbor = 0 , Insert Method =1" },
    
    { UPDATEGLOBAL, 0, "U", "updGlobal", Arg::Numeric, "  -U <unsigned> \t--updGlobal=<unsigned> \tUpdate with global best solution every n'th step." },
    
    { IMPROVEANTS, 0, "I", "improveAnts", Arg::None, "  -I, \t--improveAnts \tUse improved solutions to update pheromones." },
    
    { PRINTBESTANTS, 0, "X", "printAnts", Arg::None, "  -X, \t--printAnts \tPrint satisfaction from all ants in every step." },
      
    {
        UNKNOWN, 0, "", "", Arg::None,
        "\nDefault values for the options are: \n"
	" algorithm : AT_GREEDY; \n"
	" maxKNearestSpots : 10;\n"
	" nodeInsertMode : NIM_SHORTEST_PATH;\n"
	" stepFunction : SF_BEST; \n"
	" verbose : false; \n"
	" writeDot : false; \n"
	" maxStepsWithNoChange : 10; \n"
        "\nExample configurations will be added soon! \n"
        "to reproduce best solution found: \n"
        "-a 2 -s 1 -c \n"
    },
    { 0, 0, 0, 0, 0, 0 }
};

void Config::printHelp()
{
    int columns = getenv ("COLUMNS") ? atoi (getenv ("COLUMNS")) : 100;
    option::printUsage (fwrite, stdout, usage, columns);
}

int Config::parseArguments (int argc, char* argv[])
{

    argc -= (argc > 0);
    argv += (argc > 0); // skip program name argv[0] if present
    option::Stats stats (usage, argc, argv);
    option::Option *options = new option::Option[stats.options_max];
    option::Option *buffer = new option::Option[stats.buffer_max];

    option::Parser parse (usage, argc, argv, options, buffer);

    if (parse.error())
        exit (1);

    if (options[HELP] || argc == 0) {
        printHelp();
        exit (1);
    }
    
    bool hasSetMaxAntK = false;

    for (int i = 0; i < parse.optionsCount(); ++i) {
        option::Option& opt = buffer[i];
        switch (opt.index()) {
            case HELP:
                printHelp();
                break;
            case ALGORITHM:

                if (opt.arg)
                    switch (atoi (opt.arg)) {
                        case AlgorithmTag::AT_GRASP_LS:
                            _algorithm = AT_GRASP_LS;
                            break;
                        case AT_GRASP_VND:
                            _algorithm = AT_GRASP_VND;
                            break;
                        case AT_GREEDY:
                            _algorithm = AT_GREEDY;
                            break;
                        case AT_GVNS:
                            _algorithm = AT_GVNS;
                            break;
                        case AT_LOCALSEARCH:
                            _algorithm = AT_LOCALSEARCH;
                            break;
                        case AT_VND:
                            _algorithm = AT_VND;
                            break;
                        case AlgorithmTag::AT_ANT: 
                            _algorithm= AT_ANT;
                            break;
                        case AlgorithmTag::AT_ANT_LS:
                            _algorithm = AT_ANT_LS;
                            break;
                        case AlgorithmTag::AT_ANT_VND:
                            _algorithm = AT_ANT_VND;
                            break;
                        default:
                            printHelp();
                            exit (2);
                    }
                else {
                    printHelp();
                    exit (2);
                }
                break;
	    case NEIGHBORHOOD:
		assert(opt.arg);
		switch (atoi(opt.arg)) {
		    case NeighborhoodTag::NT_ONE_OPT:
			_neighborhood = NT_ONE_OPT;
			break;
		    case NeighborhoodTag::NT_EDGE_TWO_OPT:
			_neighborhood = NT_EDGE_TWO_OPT;
			break;
		    case NeighborhoodTag::NT_METHOD_TWO_OPT:
			_neighborhood = NT_METHOD_TWO_OPT;
			break;
		    case NeighborhoodTag::NT_NEAREST_TOUR_OPT:
			_neighborhood = NT_NEAREST_TOUR_OPT;
			break;
		    case NeighborhoodTag::NT_TWO_NODE_OPT:
			_neighborhood = NT_TWO_NODE_OPT;
			break;
		    case NeighborhoodTag::NT_GREEDY_TOUR_OPT:
			_neighborhood = NT_GREEDY_TOUR_OPT;
			break;
		    default:
			printHelp();
			exit(2);
		}
		break;	    
	    case GREEDY_NN:
		assert(!opt.arg);
		_greedyInsertHeuristic = false;
		break;
            case DOT:
                assert (!opt.arg);
                _writeDot = true;
                break;
	    case PRINT_CSV:
		assert( !opt.arg);
		_printCSVOutput = true;
		break;
	    case PRINT_ALL_STEPS:
		assert( !opt.arg);
		_printAllSteps = true;
		break;
            case KNEAREST:
                assert (atoi (opt.arg));
                _maxKNearestSpots = atoi (opt.arg);
                break;
            case VNDMODE:
                assert (atoi (opt.arg));
                _vndMode = atoi (opt.arg);
                break;
            case VERBOSE:
                assert (!opt.arg);
                _verbose = true;
                break;
	    case DEBUG:
		assert(!opt.arg);
		_debug = true;
		break;
            case INSERTMODE:
                if (opt.arg) {
                    switch (atoi (opt.arg)) {
                        case NIM_ALWAYS_AFTER:
                            _nodeInsertMode = NIM_ALWAYS_AFTER;
                            break;
                        case NIM_ALWAYS_BEFORE:
                            _nodeInsertMode = NIM_ALWAYS_BEFORE;
                            break;
                        case NIM_SHORTEST_PATH:
                            _nodeInsertMode = NIM_SHORTEST_PATH;
                            break;
                        default:
                            printHelp();
                            exit (2);
                    }
                } else {
                    printHelp();
                    exit (2);
                }

                break;
            case STEP:
                assert (opt.arg);
                switch (atoi (opt.arg)) {
                    case SF_BEST:
                        _stepFunction = SF_BEST;
                        break;
                    case SF_NEXT:
                        _stepFunction = SF_NEXT;
                        break;
                    case SF_RANDOM:
                        _stepFunction = SF_RANDOM;
                        break;
                    default:
                        printHelp();
                        exit (2);
                }
                break;
	    case MAXSTEPS:
		assert(opt.arg);
		_maxStepsWithNoChange = (unsigned)atoi(opt.arg);
		break;
	    case TIMEOUT:
		assert(opt.arg);
		_maxRuntime = (unsigned)atoi(opt.arg);
		break;
            case ALPHA: 
                assert(opt.arg);
                _alphaRCL = atof(opt.arg);
                assert(_alphaRCL >= 0 && _alphaRCL <= 1 );
                break;
            case ANTALPHA:
                assert(opt.arg);
                _alpha = atof(opt.arg);
                assert(_alpha >= 0 );
                //still have to check how big/small can alpah get 
                break;
            case ANTBETA:
                assert(opt.arg);
                _beta = atof(opt.arg);
                assert(_beta >= 0 );
                break;
            case ANTTAU:
                assert(opt.arg);
                _initialTau = atof(opt.arg);
                assert(_initialTau);
                break;
            case ANTNUMBEROFTHEM: 
                assert(opt.arg);
                _numberOfAnts = (unsigned)atoi(opt.arg);
                assert( _numberOfAnts >= 1 );
		if (!hasSetMaxAntK) _maxAntK = _numberOfAnts * 2;
                break;          
            case ANTSTEPS:
                assert(opt.arg);
                _stepsToFinish = (unsigned)atoi(opt.arg);
                assert( _stepsToFinish >= 1 );
                break;
            case ANTMAXTAU: 
                assert(opt.arg);
                _maxTau = (double) atof(opt.arg);
                break;
            case ANTMINTAU:
                assert(opt.arg);
                _minTau = (double) atof(opt.arg);
                assert(_minTau >= 0.0 );
                break;
            case ALLOWEDTIMEFACTOR:
                assert(opt.arg);
                _allowedTimeFactor = (double) atof(opt.arg);
                assert(_allowedTimeFactor >= 1.0 );
                break;
            case ANTMINK:
                assert(opt.arg);
                _minAntK = (unsigned) atoi(opt.arg);
                assert(_minAntK > 0);
                break;
            case ANTMAXK:
                assert(opt.arg);
                _maxAntK = (unsigned) atoi(opt.arg);
		hasSetMaxAntK = true;
                assert(_maxAntK > 0 );
                break;
            case ANTPERSITENCE: 
                assert(opt.arg);
                _persistFactor = (double) atof(opt.arg);
                assert( _persistFactor <= 1 && _persistFactor > 0 );
                break;
            case ANTUPDATEBEST:
                assert(opt.arg);
                _numUpdateBestAnts = (unsigned) atoi(opt.arg);
                assert(_numUpdateBestAnts >= 1);
                break;                
            case ANTHEURISTICTAG:
                assert(opt.arg);
                switch(atoi(opt.arg))
                {
                    case AntHeuristicTag::AH_INSERT: 
                        _antHeuristic = AH_INSERT;
                        break; 
                    case AntHeuristicTag::AH_NEAREST:
                        _antHeuristic = AH_NEAREST;
                        break;
                    default: 
                        break;
                }
                
                break;
	    case IMPROVEANTS:
		assert(!opt.arg);
		_improveAntSolution = true;
		break;
	    case PRINTBESTANTS:
		assert(!opt.arg);
		_printBestAnts = true;
		break;
	    case UPDATEGLOBAL:
		assert(opt.arg);
		_updateWithGlobalBest = (unsigned) atoi(opt.arg);
		assert(_updateWithGlobalBest >= 0);
		break;
            case UNKNOWN:
                // not possible because Arg::Unknown returns ARG_ILLEGAL
                // which aborts the parse with an error
                break;
        }
    }

    if (parse.nonOptionsCount() < 1 || parse.nonOptionsCount() > 2) {
        printHelp();
        exit (1);
    }
    
    _inputFile = string (parse.nonOption (0));
    if (parse.nonOptionsCount() > 1) {
	_outputFile = string (parse.nonOption (1));
    } else {
	_printSolution = true;
    }

    delete[] options;
    delete[] buffer;
    
    //the +1 is needed because the first step is to skip the program name - first line of this function
    return argc + 1;
}



Environment::Environment(Config& config)
 : config(config), problem(0), spotsearch(0), empty(0), printSteps(false)
{
    problemName = config.getInputFilename();
    size_t p = problemName.find_last_of('/');
    if (p != string::npos) {
	problemName = problemName.substr(p + 1);
    }
    p = problemName.find('.');
    if (p != string::npos) {
	problemName = problemName.substr(0, p);
    }
}

Environment::~Environment()
{
    deleteProblem();
}

void Environment::startTimer()
{
    gettimeofday(&startTime, NULL);
}

float Environment::getCurrentTime()
{
    timeval currTime;
    
    gettimeofday(&currTime, NULL);
    
    float currSecs = (currTime.tv_sec - startTime.tv_sec) + float(currTime.tv_usec - startTime.tv_usec)/1000000.0;
    
    return currSecs;
}

bool Environment::setPrintSteps(bool enable)
{
    bool tmp = printSteps;
    printSteps = enable;
    return tmp;
}

void Environment::printStepResult(const Instance& instance)
{
    if (!config.doPrintAllSteps() || !printSteps) return;
    
    if (config.doPrintCSVOutput()) {
	cout << problemName << "," << instance.getTotalSatisfaction() << "," << getCurrentTime() << endl;
    } else {
	cout << "Finished step (time: " << instance.getTotalTime() << 
	                        ", satisfaction: " << instance.getTotalSatisfaction() << 
	                        ", stamina: " << instance.getRemainingStamina() << 
	                        ", length: " << instance.getTour().size() << 
	                        ", runtime: " << getCurrentTime() << ")" << endl;
    }
    
}

void Environment::printSolution(const SASTPSolution& solution)
{
    if (config.doPrintCSVOutput()) {
	cout << problemName << "," << solution.getSatisfaction() << "," << getCurrentTime() << endl;
    } else {
	cout << "Found solution to " << problemName << " (time: " << solution.getTourTime() << 
	                                                  ", satisfaction: " << solution.getSatisfaction() << 
	                                                  ", stamina: " << solution.getStamina() << 
	                                                  ", length: " << solution.getTour().size() << 
	                                                  ", runtime: " << getCurrentTime() << ")" << endl;
    }
}

void Environment::loadProblemFile (const string& filename)
{
    deleteProblem();

    problem = new SASTProblem (filename);
    empty = new Instance(*problem);
}

void Environment::initSpotSearch (unsigned int maxk)
{
    if (spotsearch) delete spotsearch;

    spotsearch = new SpotSearch (*problem);

    spotsearch->initialize (maxk);
}

void Environment::deleteProblem()
{
    if (empty) delete empty;
    if (problem) delete problem;
    if (spotsearch) delete spotsearch;
}

