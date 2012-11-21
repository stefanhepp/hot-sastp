#include "Environment.h"

#include <iostream>

using namespace std;

void Config::usage(char* programName)
{
    cout << "Usage: " << programName << " [<options>] <input.prob> <output.sol>\n";
    cout << "\n";
    cout << "Options can be:\n";
    cout << "-a integer ( algorithm, takes the following values: 0 - GREEDY 1-NEIGHBOUR1 2-NEIGHBOUR2"<<endl;
    cout << "more to be added here" <<endl;
    
}

int Config::parseArguments(int argc, char* argv[])
{
    if (argc < 3) {
	usage(argv[0]);
	return 1;
    }
    
    _inputFile = string(argv[argc-1]);
    _outputFile = string(argv[argc]);
    
    for (unsigned i = 1; i < argc-1 ; i++) 
    {
      if(argv[i]== "-a")
      {
	switch(atoi(argv[i+1])){
	  case GREEDY: 
	     _algorithm = GREEDY;
	     break;
	  case NEIGHBOUR1: 
	    _algorithm = NEIGHBOUR1;
	    break;
	  case NEIGHBOUR2: 
	    _algorithm = NEIGHBOUR2;
	    break;
	  default: 
	    cout<<"The algorithm value is not a supported one"<<endl;
	    return 1;
	}
	}else{ 
	  cout<< "Option not supported yet"<<endl;
	  return 1; 
	}
      }
    return 0;
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
}

void Environment::deleteProblem()
{
    if (problem) delete problem;
    if (spotsearch) delete spotsearch;
}
