#include "Environment.h"

#include <iostream>

using namespace std;

Config::Config()
{
    // Set some default values
    _algorithm = AT_GREEDY;
    _maxKNearestSpots = 5;
    _nodeInsertMode = NIM_SHORTEST_PATH;
    _stepFunction = SF_BEST;
    _verbose = false;
    _writeDot = false;
}


void Config::usage(char* programName)
{
    cout << "Usage: " << programName << " [<options>] <input.prob> <output.sol>\n";
    cout << "\n";
    cout << "Options can be:\n";
    cout << "-a integer ( algorithm, takes the following values: 0 - GREEDY 1-NEIGHBOUR1 2-NEIGHBOUR2"<<endl;
    cout << "-p           write out .dot file" << endl;
    
}

int Config::parseArguments ( int argc, char* argv[] )
{
    if ( argc < 3 ) {
        usage ( argv[0] );
        exit ( 1 );
    }

    _inputFile = string ( argv[argc-2] );
    _outputFile = string ( argv[argc-1] );

    for ( unsigned i = 1; i < argc-2 ; i++ ) {
        if ( string ( argv[i] ) == "-a" ) {
            switch ( atoi ( argv[i+1] ) ) {
            case AT_GREEDY:
                _algorithm = AT_GREEDY;
                break;
            case AT_LOCALSEARCH:
                _algorithm = AT_LOCALSEARCH;
                break;
            case AT_VND:
                _algorithm = AT_VND;
                break;
            default:
                cout<<"The algorithm value is not a supported one"<<endl;
                exit ( 2 );
            }
	    i++;
	} else if (string(argv[i]) == "-p" ) {
	    _writeDot = true;
        } else {
            cout<< "Option not supported yet"<<endl;
            exit ( 3 );
        }
    }
    
    return argc;
}



Environment::~Environment()
{
    deleteProblem();
}

void Environment::loadProblemFile ( const string& filename )
{
    deleteProblem();
    
    problem = new SASTProblem(filename);
}

void Environment::initSpotSearch ( unsigned int maxk )
{
    if (spotsearch) delete spotsearch;
    
    spotsearch = new SpotSearch(*problem);
    
    spotsearch->initialize(maxk);
}

void Environment::deleteProblem()
{
    if (problem) delete problem;
    if (spotsearch) delete spotsearch;
}
