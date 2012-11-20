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
    // TODO Update tour stats
    
    
    tour.insert( tour.begin() + index, node);
}

void Instance::updateNode(unsigned int index, const TourNode& node)
{

    tour[index] = node;
}

void Instance::deleteNode(unsigned int index)
{
    
    
    tour.erase(tour.begin() + index);
}

bool Instance::isValid() const
{
    // Stamina constraint is already included in the total tour time
    return (getTotalTime() <= problem.getMaxTime());
}


