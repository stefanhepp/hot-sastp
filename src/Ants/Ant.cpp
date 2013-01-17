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


void AntNearest::findTour()
{
    instance.clear();
    
    size_t numSpots = getInstance().getProblem().getSpots().size();
    
      
    while (instance.getTourLength() < numSpots ) {
        // add a new spot into the tour
        unsigned newSpot = insertSpot();
        
        // If inserting the new spot makes the tour too long, do not add it to the tour and stop
        if (!instance.isValid()) {
            instance.deleteNode(newSpot);
            break;
        }
    }
    
    
    // TODO use neigborhood and heuristics as well as pheromones to construct a valid tour
    
    // Add nodes at end of tour until tour is complete
    // Better way to do it: interpret construction graph not as tour but as decision tree which 
    // nodes to insert -> t_ij means: insert node j after node i has been inserted into tour
    // TODO but where to insert? use shortest distance?
    
}

int AntNearest::insertSpot()
{
    NearestSpotList nearest = spotsearch.findNearestSpots(instance, instance.getTourLength() - 1, _maxk); 
    
    // find best spot and method, add it to the tour
    unsigned tmp;
    TourNode best = selectBestTourNode(nearest, tmp);
    
    return instance.addNode(best);
}

TourNode AntNearest::selectBestTourNode(NearestSpotList nearest, unsigned insertAt )
{
     TourNode best(-1, 0);
    // start off with something really bad so that we are sure to select the first node
    double bestRatio = -1.0f;
        
    for(const auto& ps : nearest) {
        int tournode = ps.first;
        unsigned spotId = ps.second;
        
        const Spot& nearestspot = problem.getSpot(spotId);
        //how can we get the last spot inserted in the tour
        const Spot& lastSpot = problem.getSpot(instance.getTour().back().spot);
        
        unsigned bestInsert;
        
       // helper.getInsertDeltaTourLength(instance, tournode, nearestspot, _insertMode, bestInsert);
       
        // check all methods of this spot
        unsigned methodId = 0;
        for (const auto& m : nearestspot.getMethods()) {
            
            double deltaTour = getDistancePerSatisfaction(lastSpot ,nearestspot, *m);
            _pm.getTau(instance.getTour().back(), (TourNode(spotId, methodId)));
                
            double deltaTime;
            double ratio = helper.calcInsertSatisfactionTimeRatio(instance.getRemainingStamina(), *m, deltaTour, deltaTime);
            
            // check if we have a new best value, but check for time constraints ..
            if (instance.getTotalTime() + deltaTime <= problem.getMaxTime() && 
                ratio > bestRatio) 
            {
                // found a new best candidate
                best.spot = spotId;
                best.method = methodId;
                bestRatio = ratio;
                insertAt = bestInsert;
            }
            
            methodId++;
        }
        
    }
    
    // Too bad, we did not find a node that still fits, just return the first nearest one..
    if (best.spot == -1) {
        insertAt = nearest.front().first + 1;
        best.spot = nearest.front().second;
        best.method = 0;
    }
    
    return best;
}
void AntNearest::addPheromones(double factor)

{

}

void AntInsert::setInstance(const Instance& inst)
{
    // TODO we should update the insertionOrder: 
    // - remove all nodes in insertionOrder where the spot is no longer in the tour
    // - update tournodes in insertionOrder with new method-id if spot is still in the tour
    // - add nodes in the tour that are new to the insertionOrder at the end
    
    Ant::setInstance(inst);
}

double AntNearest::getDistancePerSatisfaction(Spot begin, Spot end, const Method& m )
{
    double result;
    //compute distance per satisfaction 
    result = problem.getDistance(begin, end) / m.getSatisfaction();
    //make it to power of Beta
    result = pow(result, _beta); 
    //return the value to be used in the computation of p_ij^k
    return result;
}

void AntInsert::findTour()
{

}

void AntInsert::addPheromones(double factor)
{

}
