#include "ProblemHelper.h"

#include <iostream>
#include <algorithm>

using namespace std;

double ProblemHelper::getInsertDeltaTourLength(const Instance& tour, unsigned int index, const Spot& newspot, 
					        Config::NodeInsertMode insertMode, unsigned int& bestInsert)
{    
    double deltaTour;
    
    const Spot& spot = tour.getSpot(index);
    double dist = problem.getDistance(spot, newspot);

    switch (insertMode) {
	case Config::NIM_ALWAYS_AFTER: {
	    // Insert after the node with the currently nearest spot
	    bestInsert = index + 1;
	    const Spot& next = tour.getSpot(index + 1);
	    deltaTour = dist + problem.getDistance(newspot, next)
			- problem.getDistance(spot, next);
	    break;
	}
	case Config::NIM_ALWAYS_BEFORE: {
	    // Insert before the node with the currenty nearest spot (except for the origin)
	    bestInsert = index == -1 ? 0 : index; 
	    const Spot& prev = tour.getSpot(index == -1 ? 0 : index - 1);
	    deltaTour = dist + problem.getDistance(newspot, prev) 
			- problem.getDistance(spot, prev);
	    break;
	}
	case Config::NIM_SHORTEST_PATH: {
	    
	    // if we are at the origin, always insert after the node
	    if (index == -1) {
		bestInsert = index + 1;
		const Spot& next = tour.getSpot(index + 1);
		deltaTour = dist + problem.getDistance(newspot, next)
			    - problem.getDistance(spot, next);
		break;
	    }
	    
	    // Select to insert either before or after the node, depending on the distance differences
	    const Spot& prev = tour.getSpot(index - 1);
	    const Spot& next = tour.getSpot(index + 1);
	    
	    double deltaBefore = dist + problem.getDistance(newspot, prev) 
				    - problem.getDistance(spot, prev);
	    double deltaAfter  = dist + problem.getDistance(newspot, next)
				    - problem.getDistance(spot, next);
	    
	    bestInsert = deltaBefore < deltaAfter ? index : index + 1;
	    deltaTour = std::min(deltaBefore, deltaAfter);
	    break;
	}
    }

    return deltaTour;
}

unsigned int ProblemHelper::findInsertPosition(const Instance& tour, unsigned int index, const Spot& newspot, Config::NodeInsertMode insertMode)
{
    unsigned bestInsert;
    
    const Spot& spot = tour.getSpot(index);
    double dist = problem.getDistance(spot, newspot);

    switch (insertMode) {
	case Config::NIM_ALWAYS_AFTER: {
	    // Insert after the node with the currently nearest spot
	    bestInsert = index + 1;
	    break;
	}
	case Config::NIM_ALWAYS_BEFORE: {
	    // Insert before the node with the currenty nearest spot (except for the origin)
	    bestInsert = index == -1 ? 0 : index; 
	    break;
	}
	case Config::NIM_SHORTEST_PATH: {

	    // if we are at the origin, always insert after the node
	    if (index == -1) {
		bestInsert = index + 1;
		break;
	    }
	    
	    // Select to insert either before or after the node, depending on the distance differences
	    const Spot& prev = tour.getSpot(index - 1);
	    const Spot& next = tour.getSpot(index + 1);
	    
	    double deltaBefore = dist + problem.getDistance(newspot, prev) 
				    - problem.getDistance(spot, prev);
	    double deltaAfter  = dist + problem.getDistance(newspot, next)
				    - problem.getDistance(spot, next);
	    
	    bestInsert = deltaBefore < deltaAfter ? index : index + 1;
	    break;
	}
    }

    return bestInsert;
}

double ProblemHelper::calcInsertSatisfactionTimeRatio(double currRemainingStamina, const Method& m,
                                                      double deltaTour, double& deltaTime)
{
    double deltaSatisfaction = m.getSatisfaction() - deltaTour * problem.getAlpha();
    deltaTime = m.getTime() + deltaTour / problem.getVelocity();
    
    // honor loss of time by reduced stamina, if remaining stamina gets below zero
    double newStamina = currRemainingStamina > 0.0 ? 
				std::min(currRemainingStamina - m.getStamina(), 0.0) : 
				-m.getStamina();
    if (newStamina < 0.0) {
	deltaTime += -newStamina * problem.getHabitus();
    }
    
    return deltaSatisfaction / deltaTime;
}

unsigned int ProblemHelper::removeDominatedMethods()
{
  unsigned count = 0;
  
  for (auto& spot : problem.getSpots()) {
    
    for (int i = 0; i < spot->getMethods().size(); i++) {
      const Method &method = spot->getMethod(i);
      
      double gain = method.getSatisfaction();
      double cost = method.getTime() + method.getStamina() * problem.getHabitus();
      
      bool isdom = false;
      for (int j = 0; j < spot->getMethods().size(); j++) {
	if (j == i) continue;
	
	const Method &m = spot->getMethod(j);
	
	double g = m.getSatisfaction();
	double c = m.getTime() + m.getStamina() / problem.getHabitus();
	if (g >= gain && c <= cost) isdom++;
      }
      
      if (isdom) {
	spot->removeMethod(i);
	i--;
	count++;
      }
    }
  }
  
  return count;
}

void ProblemHelper::sortMethods()
{
  for (auto& spot : problem.getSpots()) {
    
    std::sort(spot->getMethods().begin(), spot->getMethods().end(), [this](const Method* a, const Method* b) {
      double ra = a->getSatisfaction() / (a->getTime() + a->getStamina() / problem.getHabitus());
      double rb = b->getSatisfaction() / (b->getTime() + b->getStamina() / problem.getHabitus());
      
      return ra > rb;
    });
  }
}

