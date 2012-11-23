#include "Driver.h"

#include "Support/SpotSearch.h"
#include "Support/DotPrinter.h"
#include "Solver/GreedyTour.h"
#include "Solver/LocalSearch.h"
#include "Solver/Neighborhood.h"
#include "Solver/VND.h"
#include "Solver/GVNS.h"
#include "Solver/AbstractSearch.h"
#include "Solver/Grasp.h"

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
  switch(env.getConfig().getAlgorithm()){
    case Config::AT_GREEDY: {
	// TODO use env.getConfig() to instanciate appropriate greedy solver
	GreedyInsertHeuristic greedy(env);
	
	greedy.run();
	
	solution = greedy.getInstance().createSolution();
	break;
    }

    case Config::AT_LOCALSEARCH: {
        // TODO use env.getConfig() to instanciate appropriate greedy solver
        GreedyInsertHeuristic greedy(env);
        
        greedy.run();
        
        // TODO use config to select neighborhood
        Neighborhood* nb = new OneOPT(env.getProblem());
        
        LocalSearch local(env, *nb, greedy.getInstance());
        
        solution = local.getInstance().createSolution();
        break;
    }
    
    case Config::AT_VND: {
        // TODO use env.getConfig() to instanciate appropriate greedy solver
        GreedyInsertHeuristic greedy(env);
        
        greedy.run();
        
        VND vnd(env, greedy.getInstance());
        Neighborhood* one = new OneOPT(env.getProblem());
        vnd.addNeighborhood(*one);
        Neighborhood* two = new TwoOPT(env.getProblem());
        vnd.addNeighborhood(*two);
        
        solution = vnd.getInstance().createSolution();
        break;
    }
        
    case Config::AT_GRASP_LS: {
        // TODO use env.getConfig() to instanciate appropriate greedy solver
        GreedyInsertHeuristic greedy(env);
        
        greedy.run();
        
        Neighborhood* nb = new OneOPT(env.getProblem());
        AbstractSearch* search = new LocalSearch(env, *nb, greedy.getInstance());
                
        Grasp grasp(env, *search, search->getInstance());

        solution = grasp.getInstance().createSolution();
        break;
    }
   
    case Config::AT_GRASP_VND: {
        // TODO use env.getConfig() to instanciate appropriate greedy solver
        GreedyInsertHeuristic greedy(env);
        
        greedy.run();
        
        Neighborhood* nb = new OneOPT(env.getProblem());
        
        AbstractSearch* search = new VND(env, greedy.getInstance());
        
        Grasp grasp(env, *search, search->getInstance());

        solution = grasp.getInstance().createSolution();
        break;
    }
    case Config::AT_GVNS: {
        // TODO use env.getConfig() to instanciate appropriate greedy solver
        GreedyInsertHeuristic greedy(env);
        
        greedy.run();
        
        
        solution = greedy.getInstance().createSolution();
        break;
    }
  }
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

