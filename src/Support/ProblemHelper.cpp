#include "ProblemHelper.h"

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