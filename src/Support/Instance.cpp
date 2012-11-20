#include "Instance.h"

Instance::Instance(const SASTProblem &problem)
: problem(problem), tourTime(0.0), totalSatisfaction(0.0), 
  remainingStamina(problem.getInitStamina())
{
}

void Instance::clear()
{
    tour.clear();
    totalSatisfaction = 0.0;
    tourTime = 0.0;
    remainingStamina = problem.getInitStamina();
}

double Instance::getTotalRequiredRestTime() const 
{
    return remainingStamina < 0 ? -remainingStamina / problem.getHabitus() : 0.0; 
}

void Instance::insertNode(unsigned int index, const TourNode& node)
{
    addTourValues( getInsertDeltaValues(index, node) );
    
    tour.insert( tour.begin() + index, node);
}

void Instance::updateNode(unsigned int index, const TourNode& node)
{
    addTourValues( getUpdateDeltaValues(index, node) );
    
    tour[index] = node;
}

void Instance::deleteNode(unsigned int index)
{
    addTourValues( getDeleteDeltaValues(index) );
    
    tour.erase(tour.begin() + index);
}

TourValues Instance::getInsertDeltaValues(unsigned int index, const TourNode& node)
{
    // insert node before 'index'
    TourValues values;
    
    const Spot& prevSpot = getSpot(index - 1);
    const Spot& nextSpot = getSpot(index);
    const Spot& newSpot  = getSpot(node);

    // Update tour time and satisfaction to include travel to new spot
    double deltaDist = problem.getDistance(prevSpot, newSpot) + problem.getDistance(newSpot, nextSpot);
    deltaDist -= problem.getDistance(prevSpot, nextSpot);
    
    values.addDeltaDistance(problem, deltaDist);
    
    // Add new method at new spot
    values += newSpot.getMethod(node.method);
    
    return values;
}

TourValues Instance::getUpdateDeltaValues(unsigned int index, const TourNode& node)
{
    TourNode oldNode = getNode(index);
    
    // Return if nothing changes
    if (oldNode == node) return TourValues();

    const Spot& oldSpot = getSpot(oldNode);
    const Spot& newSpot = getSpot(node);
    
    TourValues values;
    
    // Update tour time and satisfaction values if spot changed
    if (oldNode.spot != node.spot) {
	// new distance .. 
	const Spot& prevSpot = getSpot(index - 1);
	const Spot& nextSpot = getSpot(index + 1);
	double deltaDist = problem.getDistance(prevSpot, newSpot) + problem.getDistance(newSpot, nextSpot);
	deltaDist -= problem.getDistance(prevSpot, oldSpot) + problem.getDistance(oldSpot, nextSpot);
	
	values.addDeltaDistance(problem, deltaDist);
    }
    
    // remove the old method
    values -= oldSpot.getMethod(oldNode.method);
    // add the new method instead
    values += newSpot.getMethod(node.method);
	    
    return values;
}

TourValues Instance::getDeleteDeltaValues(unsigned int index)
{
    TourNode node = getNode(index);

    // delete node at 'index'
    TourValues values;
    
    const Spot& prevSpot = getSpot(index - 1);
    const Spot& nextSpot = getSpot(index + 1);
    const Spot& oldSpot  = getSpot(node);

    // Update tour time and satisfaction to remove travel to new spot
    double deltaDist = problem.getDistance(prevSpot,  nextSpot);
    deltaDist -= problem.getDistance(prevSpot, oldSpot) + problem.getDistance(oldSpot, nextSpot);
    
    values.addDeltaDistance(problem, deltaDist);
    
    // Remove method of old spot
    values -= oldSpot.getMethod(node.method);
    
    return values;
}

TourValues Instance::getStepValues(const TourNode& from, const TourNode& to)
{
    // Step from hotel to hotel
    if (from.spot == -1 && to.spot == -1) return TourValues();
    
    const Spot& fromSpot = getSpot(from);
    const Spot& toSpot = getSpot(to);
    
    double dist = problem.getDistance(fromSpot, toSpot);
    double tourTime = dist / problem.getVelocity();
    double satisfaction = -dist * problem.getAlpha();
    
    // Step from some spot to next spot
    TourValues values(tourTime, satisfaction, 0.0);
    
    // Perform some method at next spot
    if (to.spot != -1) {
	values += toSpot.getMethod(to.method);
    }
    
    return values;
}

bool Instance::isValid() const
{
    // Stamina constraint is already included in the total tour time
    return (getTotalTime() <= problem.getMaxTime());
}

SASTPSolution* Instance::createSolution() const
{
    SASTPSolution* sol = new SASTPSolution(problem);
    
    double currStamina = problem.getInitStamina();
    double remainingRest = getTotalRequiredRestTime();
    
    for (const auto& node : tour) {
	const Method& m = getMethod(node);
	
	// update stamina
	currStamina -= m.getStamina();
	
	// fill up stamina up to max stamina if required
	double maxRest = (problem.getMaxStamina() - currStamina) / problem.getHabitus();
	double rest = std::min(remainingRest, maxRest);
	
	remainingRest -= rest;
	
	sol->addStop( node.spot, node.method, rest );
    }
    
    sol->finishTour();
    
    return sol;
}

void Instance::addTourValues(TourValues values)
{
    tourTime += values.tourTime;
    totalSatisfaction += values.satisfaction;
    remainingStamina -= values.stamina;
}

