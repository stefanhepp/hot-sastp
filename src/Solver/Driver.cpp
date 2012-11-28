#include "Driver.h"


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
    if(env.getConfig().getAlgorithm() == Config::AT_GREEDY_IN)
    {
        gt = new GreedyInsertHeuristic(env);
        return gt;
        
    }
    else
    { 
        gt = new GreedyNearestNeighbor(env);
        return gt;
    }
}

LocalSearch* Driver::getLocalSearch(Environment& env, const Instance& init) 
{
    // TODO make the neighborhood for local search configurable
    
    Neighborhood* nb = new OneOPT(env);
    
    LocalSearch* ls;
    ls = new LocalSearch(env, *nb, init);
    
    return ls;
}

VND* Driver::getVND(Environment& env, const Instance& init)
{
    VND* vnd = new VND(env, init);
    
    Neighborhood* one = new OneOPT(env);
    vnd->addNeighborhood(*one);
    
    Neighborhood* two = new TwoOPT(env);
    vnd->addNeighborhood(*two);

    return vnd;
}


void Driver::solve()
{
  env.startTimer();
  env.setPrintSteps(false);
    
  switch(env.getConfig().getAlgorithm()){
    case Config::AT_GREEDY_IN:
    case Config::AT_GREEDY_NN:
    {
	GreedyTour* greedy = getGreedyTour(env);
	
	env.setPrintSteps(true);
	greedy->run();
	
	solution = greedy->getInstance().createSolution();
	break;
    }

    case Config::AT_LOCALSEARCH: {
        GreedyTour* greedy = getGreedyTour(env);
        
        greedy->run();
        
        LocalSearch* local = getLocalSearch(env, greedy->getInstance());
        
	env.setPrintSteps(true);
	local->run();
	
        solution = local->getInstance().createSolution();
        break;
    }
    
    case Config::AT_VND: {
        GreedyTour* greedy = getGreedyTour(env);
        
        greedy->run();
        
	VND* vnd = getVND(env, greedy->getInstance());
	
	env.setPrintSteps(true);
	vnd->run();
	
        solution = vnd->getInstance().createSolution();
        break;
    }
        
    case Config::AT_GRASP_LS: {
	
	LocalSearch *ls = getLocalSearch(env, env.getEmptyInstance());
	
        Grasp grasp(env, *ls, env.getEmptyInstance());
	
	env.setPrintSteps(true);
        grasp.run();
        
        solution = grasp.getInstance().createSolution();
        break;
    }
   
    case Config::AT_GRASP_VND: {
        
	VND* vnd = getVND(env, env.getEmptyInstance());
	
        Grasp grasp(env, *vnd, env.getEmptyInstance());

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

