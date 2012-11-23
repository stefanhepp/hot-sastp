#include "Driver.h"

#include "Support/SpotSearch.h"
#include "Support/DotPrinter.h"

#include <iostream>

using namespace std;

Driver::~Driver()
{
    if (solution) delete solution;
}


void Driver::prepare()
{
    // Initialize the SpotSearch
    env.initSpotSearch( env.getConfig().getMaxKNearestSpots() );
    
}

void Driver::solve()
{
    // TODO use env.getConfig() to instanciate appropriate greedy solver
    GreedyInsertHeuristic greedy(env);
    
    
    greedy.run();
    
    // TODO use config to run requested heuristic improvement on initial solution, if enabled
    
    
    
    solution = greedy.getInstance().createSolution();
}

void Driver::checkSolution() {
    if (!solution) {
	cout << "No solution found!" << endl;
	return;
    }
    
    if (solution->isValid(env.getConfig().isVerbose())) {
	cout << "Found a valid solution ";
    } else {
	cout << "Solution is not valid. Go back to debugging! ";
    }
    cout << "(time: " << solution->getTourTime() << ", satisfaction: " << solution->getSatisfaction() << ")" << endl;
}

void Driver::writeSolution()
{
    string solFilename = env.getConfig().getOutputFilename();
    
    solution->store(solFilename);
    
    if (env.getConfig().doWriteDotFile()) {
	string dotFilename = solFilename;
	
	if (dotFilename.substr(dotFilename.length()-4) == ".sol") {
	    dotFilename = dotFilename.substr(0, dotFilename.length()-4);
	}
	dotFilename = dotFilename + ".dot";
	
	cout << "Exporting tour to " << dotFilename << endl;
	
	DotPrinter printer(env.getProblem(), *solution);
	printer.writeDotFile(dotFilename, env.getProblem().getSpots().size() > 10);
    }
}

