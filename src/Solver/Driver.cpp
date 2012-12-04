#include "Driver.h"

#include "Support/DotPrinter.h"

#include "Solver/GreedyTour.h"
#include "Solver/VND.h"
#include "Solver/Grasp.h"
#include "Solver/GVNS.h"
#include "Solver/Neighborhood.h"
#include "Solver/TourNeighborhood.h"

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

GreedyTour* Driver::getGreedyTour(Environment& env){
    GreedyTour* gt;
    if(env.getConfig().useGreedyInsertHeuristic())
    {
	if (env.getConfig().isVerbose()) cout << "Creating insert heuristic greedy search .." << endl;
	
        gt = new GreedyInsertHeuristic(env);
        return gt;
    }
    else
    { 
	if (env.getConfig().isVerbose()) cout << "Creating nearest neighbor greedy search .." << endl;
	
        gt = new GreedyNearestNeighbor(env);
        return gt;
    }
}

LocalSearch* Driver::getLocalSearch(Environment& env, const Instance& init) 
{
    // TODO we should clean up created neighborhoods
    
    Neighborhood* nb;
    NodeInserter* ni;
    
    switch (env.getConfig().getNeighborhood()) {
	case Config::NT_ONE_OPT:
	    if (env.getConfig().isVerbose()) cout << "Creating local search with Spot-1-opt neighborhood .." << endl;
	    
	    nb = new SpotOneOPT(env);
	    break;
	case Config::NT_EDGE_TWO_OPT:
	    if (env.getConfig().isVerbose()) cout << "Creating local search with Edge-2-opt neighborhood .." << endl;
	    
	    nb = new EdgeTwoOPT(env);
	    break;
	case Config::NT_METHOD_TWO_OPT:
	    if (env.getConfig().isVerbose()) cout << "Creating local search with Method-2-opt neighborhood .." << endl;
	    
	    nb = new MethodTwoOPT(env);
	    break;
	case Config::NT_NEAREST_TOUR_OPT:
	    if (env.getConfig().isVerbose()) cout << "Creating local search with Nearest-Tour-exchange with consecutive inserter .." << endl;
	    
	    ni = new ConsecutiveNodeInserter(env, env.getConfig().getMaxKNearestSpots(), false);
	    
	    nb = new NearestTourExchange(env, 2, 4, *ni);
	    break;
	case Config::NT_TWO_NODE_OPT:
	    if (env.getConfig().isVerbose()) cout << "Creating local search with 2-Nodes-Exchange and random insert .." << endl;
	    
	    ni = new RandomNodeInserter(env, env.getConfig().getMaxKNearestSpots(), true);
	    
	    nb = new TwoNodesTourExchange(env, *ni);
	    break;
	case Config::NT_GREEDY_TOUR_OPT:
	    if (env.getConfig().isVerbose()) cout << "Creating local search with Greedy-tour-exchange and greedy inserter .." << endl;
	    
	    AbstractSearch* as = new GreedyInsertHeuristic(env);
	    
	    ni = new SearchNodeInserter(env, *as);
	    
	    nb = new NearestTourExchange(env, 2, 4, *ni);
	    break;	    
    }
    
    LocalSearch* ls;
    ls = new LocalSearch(env, *nb, init);
    
    return ls;
}

VND* Driver::getVND(Environment& env, const Instance& init)
{
    Neighborhood* nb;
    NodeInserter* ni;
    
    VND* vnd = new VND(env, init);
    
    //nb = new SpotOneOPT(env);
    //vnd->addNeighborhood(*nb);
    
    nb = new EdgeTwoOPT(env);
    vnd->addNeighborhood(*nb);
    
    nb = new MethodTwoOPT(env);
    vnd->addNeighborhood(*nb);

    //ni = new SearchNodeInserter(env, *as);
    //nb = new TwoNodesTourExchange(env, *ni);
    //vnd->addNeighborhood(*nb);
    
    //ni = new RandomNodeInserter(env, env.getConfig().getMaxKNearestSpots(), false);
    //nb = new TwoNodesTourExchange(env, *ni);
    //vnd->addNeighborhood(*nb);
    
    ni = new ConsecutiveNodeInserter(env, env.getConfig().getMaxKNearestSpots(), false);
    nb = new NearestTourExchange(env, 2, 4, *ni);
    vnd->addNeighborhood(*nb);
    
    return vnd;
}


void Driver::solve()
{
  env.startTimer();
  env.setPrintSteps(false);
  
  bool verbose = env.getConfig().isVerbose();
    
  switch(env.getConfig().getAlgorithm()){
    case Config::AT_GREEDY:
    {
	GreedyTour* greedy = getGreedyTour(env);
	
	if (verbose) cout << "Running greedy search .." << endl;
	
	env.setPrintSteps(true);
	greedy->run();
	
	solution = greedy->getInstance().createSolution();
	break;
    }

    case Config::AT_LOCALSEARCH: {
        GreedyTour* greedy = getGreedyTour(env);

	if (verbose) cout << "Running greedy search for initial solution .." << endl;

	greedy->run();
        
        LocalSearch* local = getLocalSearch(env, greedy->getInstance());
        
	if (verbose) cout << "Running local search .." << endl;
	
	env.setPrintSteps(true);
	local->run();
	
        solution = local->getInstance().createSolution();
        break;
    }
    
    case Config::AT_VND: {
        GreedyTour* greedy = getGreedyTour(env);
        
	if (verbose) cout << "Running greedy search for initial solution .." << endl;

        greedy->run();
        
	VND* vnd = getVND(env, greedy->getInstance());
	
	if (verbose) cout << "Running VND search .." << endl;

	env.setPrintSteps(true);
	vnd->run();
	
        solution = vnd->getInstance().createSolution();
        break;
    }
        
    case Config::AT_GRASP_LS: {
	
	LocalSearch *ls = getLocalSearch(env, env.getEmptyInstance());
	
        Grasp grasp(env, *ls, env.getEmptyInstance());
	
	if (verbose) cout << "Running GRASP search with local search .." << endl;

	env.setPrintSteps(true);
        grasp.run();
        
        solution = grasp.getInstance().createSolution();
        break;
    }
   
    case Config::AT_GRASP_VND: {
        
	VND* vnd = getVND(env, env.getEmptyInstance());
	
        Grasp grasp(env, *vnd, env.getEmptyInstance());

	if (verbose) cout << "Running GRASP search with VND .." << endl;

	env.setPrintSteps(true);
	grasp.run();
	
        solution = grasp.getInstance().createSolution();
        break;
    }
    case Config::AT_GVNS: {

	VND* vnd = getVND(env, env.getEmptyInstance());
	
        GVNS gvns(env, *vnd, env.getEmptyInstance());
	
	// TODO add GVNS neighborhoods (different from VND neighborhoods!)
	// gvns.addNeighborhood( new ..  );
	
	if (verbose) cout << "Running GVNS search with VND .." << endl;

	env.setPrintSteps(true);
	gvns.run();
        
        solution = gvns.getInstance().createSolution();
        break;
    }
  }  
  
  if (solution) {
      env.printSolution(*solution);
  }
}

void Driver::checkSolution() {
    if (!solution) {
	cout << "No solution found!" << endl;
	return;
    }
    
    if (!solution->isValid(env.getConfig().isVerbose())) {
	cout << "Solution is not valid. Go back to debugging!" << endl;
    }
}

void Driver::writeSolution()
{
    if (env.getConfig().doPrintSolution()) {
	solution->print(cout, false);
    }
    
    string solFilename = env.getConfig().getOutputFilename();
    if (!solFilename.empty()) {
	
	solution->store(solFilename);
	
	if (env.getConfig().doWriteDotFile()) {
	    string dotFilename = solFilename;
	    
	    if (dotFilename.substr(dotFilename.length()-4) == ".sol") {
		dotFilename = dotFilename.substr(0, dotFilename.length()-4);
	    }
	    dotFilename = dotFilename + ".dot";
	
	    if (!env.getConfig().doPrintCSVOutput()) {
		cout << "Exporting tour to " << dotFilename << endl;
	    }
	    
	    DotPrinter printer(env.getProblem(), *solution);
	    printer.writeDotFile(dotFilename, env.getProblem().getSpots().size() > 10);
	}
    }
}

