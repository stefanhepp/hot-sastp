#include "Ant.h"

#include <math.h>

#include "Framework/SASTProblem.h"
#include "Solver/TourNeighborhood.h"

Ant::Ant(Environment &env, PheromoneMatrix& pm, int k)
: instance(env.getProblem()),_pm(pm), problem(env.getProblem())
{
    _antNumber = k; 
    _alpha = env.getConfig().getAntAlpha();
    _beta = env.getConfig().getAntBeta();
    // TODO insert neighborhood structures, if required
}

double Ant::getTauEta(TourNode lastNode, TourNode newNode, TourValues deltaValues)
{
    // get tau_ij
    double tau = _pm.getTau(lastNode, newNode);
    // calculate t_ij^alpha * n_ij^beta
    return pow(tau, _alpha) * getDistancePerSatisfaction(deltaValues);
}

double Ant::getDistancePerSatisfaction(TourValues deltaValues)
{
    double result;
    //compute time per satisfaction ( = 1 / fitness )
    result = 1 / instance.getSatisfactionPerTotalTimeRatio( deltaValues );
    //make it to power of Beta
    result = pow(result, _beta); 
    //return the value to be used in the computation of p_ij^k
    return result;
}

double Ant::getTourDeltaTau() const
{
    return instance.getTotalTime() / instance.getTotalSatisfaction();
}

void Ant::findTour()
{
    instance.clear();
    
    size_t numSpots = getInstance().getProblem().getSpots().size();
    
      
    while (instance.getTourLength() < numSpots ) {
        // add a new spot into the tour
        unsigned newSpot = insertSpot();
        
        // If inserting the new spot makes the tour too long, do not add it to the tour and stop
	
	// TODO allow the tour to be up to maxTime * some factor and hope that the localSearch will make it valid later
	//      (get 'some factor' from config)
        if (!instance.isValid()) {
            instance.deleteNode(newSpot);
            break;
        }
    }
}



int AntNearest::insertSpot()
{
    NearestSpotList nearest = spotsearch.findNearestSpots(instance, instance.getTourLength() - 1, _antNumber); 
    
    // find best spot and method, add it to the tour
    TourNode best = selectBestTourNode(nearest);
    
    return instance.addNode(best);
}

TourNode AntNearest::selectBestTourNode(NearestSpotList nearest)
{
    // list contains pairs of valid candidates, and their tauEta value
    std::vector<std::pair<TourNode,double>> candidates;   
    double sumP = 0;
    
    for(const auto& ps : nearest) {
        int tournode = ps.first;
        unsigned spotId = ps.second;
        
        const Spot& nearestspot = problem.getSpot(spotId);
        TourNode lastNode = instance.getTour().back();
	
        unsigned bestInsert;
        
       // helper.getInsertDeltaTourLength(instance, tournode, nearestspot, _insertMode, bestInsert);
       
        // check all methods of this spot
        unsigned methodId = 0;
        for (const auto& m : nearestspot.getMethods()) {
            
	    TourNode newNode(spotId, methodId);
	    TourValues insertValues = instance.getInsertDeltaValues(instance.getTourLength(), newNode);
	    
            double tauEta = getTauEta(lastNode, newNode, insertValues);
            
	    sumP += tauEta;
	    candidates.push_back(std::make_pair(newNode,tauEta));

	    // TODO What happens if the new node causes the tour to get invalid? Several approches:
	    //  - getTauEta shoud assign it a low value, so it is unlikely to get picked, but will still be picked if all other methods
	    //    make it invalid as well or are just bad choices. We assume the localSearch will make the tour valid afterwards.
	    //  - Skip that method, use instance.isValid(insertValues) to check if tour is still valid.
	                
            methodId++;
        }
    }
    
    // TODO r = rand(0..1); p = 0; 
    // for ( entry : tauEtaList )
    //     if p + entry.second/sumP > r then return entry.first else p += entry.second/sumP endif
    

    // we should only reach that point if our tauEtaList is empty (i.e. we found no usable methods), 
    // we return just some best guess here
    return TourNode(nearest.front().second, 0);
}

void AntNearest::addPheromones(double factor)
{
    double deltaTau = getTourDeltaTau();
    
    // TODO for i in 0..tourLength-1 do _pm.addTau(instance.getNode(i - 1), instance.getNode(i), deltaTau * factor);
    // Note: first edge has start-index -1 (the hotel)
    
}



void AntInsert::setInstance(const Instance& inst)
{
    // TODO we should update the insertionOrder: 
    // - remove all nodes in insertionOrder where the spot is no longer in the tour
    // - update tournodes in insertionOrder with new method-id if spot is still in the tour
    // - add nodes in the tour that are new to the insertionOrder at the end
    
    Ant::setInstance(inst);
}


int AntInsert::insertSpot()
{
    // TODO interpret construction graph not as tour but as decision tree which 
    // nodes to insert -> t_ij means: insert node j after node i has been inserted into tour
    // But where to insert? use shortest distance!
 
    return -1;
}

TourNode AntInsert::selectBestTourNode(NearestSpotList nearest, unsigned int& insertAt, Config::NodeInsertMode insertMethod)
{

    return TourNode(0,0);
}

void AntInsert::addPheromones(double factor)
{
    double deltaTau = getTourDeltaTau();
    
    
}

