#include "Environment.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include <assert.h>
#include "optionparser.h"

using namespace std;

Config::Config()
{
    // Set some default values
    _algorithm = AT_GREEDY_NN;
    _maxKNearestSpots = 5;
    _nodeInsertMode = NIM_SHORTEST_PATH;
    _stepFunction = SF_BEST;
    _verbose = false;
    _writeDot = false;
    _maxStepsWithNoChange = 10u;
    _printAllSteps = false;
    _printCSVOutput = false;
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
};

enum optionIndex {UNKNOWN, HELP, ALGORITHM, KNEAREST, VERBOSE, DOT, PRINT_CSV, PRINT_ALL_STEPS, INSERTMODE, STEP, MAXSTEPS};
const option::Descriptor usage[] = {
    {
        UNKNOWN, 0, "", "",        Arg::Unknown, "USAGE: sastpsolver [options] inputFile outputFile\n\n"
        "Options:"
    },
    { HELP, 0, "h", "help", Arg::None, "  \t--help  \tPrint usage and exit." },
    {
        ALGORITHM , 0, "a", "algorithm", Arg::Numeric, "  -a <arg>, \t--algorithm=<arg>"
        "  \tTakes an integer argument.\n \tOptions:\n \tAT_GREEDY_IN = 0 ,\n \tAT_GREEDY_NN =1 ,\n \tAT_LOCALSEARCH = 2"
        ",\n \tAT_VND = 3, \n \tAT_GRASP = 4, \n \tAT_GVNS = 5. \n"
    },
    {
        KNEAREST, 0, "k", "knear", Arg::Numeric, "  -k <arg>, \t--knear=<arg> \tMust have as an"
        " argument a number -maximal KNearest Spots."
    },
    { DOT, 0, "d", "dot", Arg::None, "  -d, \t--dot \tGenerate dot file from solution. " },
    { VERBOSE, 0, "v", "verbose", Arg::None, "  -v, \t--verbose \tBe verbosive."},
    { PRINT_CSV, 0, "c", "csv", Arg::None, "  -c, \t--csv \tPrint result as CSV output." },
    { PRINT_ALL_STEPS, 0, "s", "allsteps", Arg::None, "  -s, \t--allsteps \tPrint result of all intermediate steps." },
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
	MAXSTEPS, 0, "m", "maxSteps", Arg::Numeric, " -m <arg>, \t--maxSteps=<arg> \tMaximal number of steps with no improvement.\n"
    },
    {
        UNKNOWN, 0, "", "", Arg::None,
        "\nExamples:\n"
        "Default values for the options are: \n"
	" algorithm : AT_GREEDY_IN; \n"
	" maxKNearestSpots : 5;\n"
	" nodeInsertMode : NIM_SHORTEST_PATH;\n"
	" stepFunction : SF_BEST; \n"
	" verbose : false; \n"
	" writeDot : false; \n"
	" maxStepsWithNoChange : 10; \n"
        "\nExample configurations be added soon! \n"
    },
    { 0, 0, 0, 0, 0, 0 }
};

void Config::printHelp()
{
    int columns = getenv ("COLUMNS") ? atoi (getenv ("COLUMNS")) : 80;
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
                        case AT_GREEDY_NN:
                            _algorithm = AT_GREEDY_NN;
                            break;
                        case AT_GREEDY_IN:
                            _algorithm = AT_GREEDY_IN;
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
                        default:
                            printHelp();
                            exit (2);
                    }
                else {
                    printHelp();
                    exit (2);
                }
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
            case VERBOSE:
                assert (!opt.arg);
                _verbose = true;
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
            case UNKNOWN:
                // not possible because Arg::Unknown returns ARG_ILLEGAL
                // which aborts the parse with an error
                break;
        }
    }

    if (parse.nonOptionsCount() != 2) {
        printHelp();
        exit (1);
    }

    _inputFile = string (parse.nonOption (0));
    _outputFile = string (parse.nonOption (1));

    delete[] options;
    delete[] buffer;
    
    //the +1 is needed because the first step is to skip the program name - first line of this function
    return argc + 1;
}




Environment::~Environment()
{
    deleteProblem();
}

void Environment::loadProblemFile (const string& filename)
{
    deleteProblem();

    problem = new SASTProblem (filename);
}

void Environment::initSpotSearch (unsigned int maxk)
{
    if (spotsearch) delete spotsearch;

    spotsearch = new SpotSearch (*problem);

    spotsearch->initialize (maxk);
}

void Environment::deleteProblem()
{
    if (problem) delete problem;
    if (spotsearch) delete spotsearch;
}

