#include "Environment.h"

#include <iostream>

using namespace std;

Environment::Environment()
: problem(0), solution(0)
{
}

Environment::~Environment()
{
    if (problem) delete problem;
    if (solution) delete solution;
}

void Environment::usage(char* programName)
{
    cout << "Usage: " << programName << " [<options>] <input.prob> <output.sol>\n";
    cout << "\n";
    cout << "Options can be:\n";
    cout << "-a integer ( algorithm, takes the following values: 0 - GREEDY 1-NEIGHBOUR1 2-NEIGHBOUR2"<<endl;
    cout << "more to be added here" <<endl;
    
}

int Environment::parseArguments(int argc, char* argv[])
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



