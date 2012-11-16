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
    
}

int Environment::parseArguments(int argc, char* argv[])
{
    if (argc < 3) {
	usage(argv[0]);
	return 1;
    }
    
    return 0;
}

