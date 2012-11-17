#ifndef INSTANCE_H
#define INSTANCE_H

#include "Framework/SASTProblem.h"

#include <vector>

struct TourNode {
    unsigned spot;
    unsigned method;
    
    TourNode(unsigned spot, unsigned method) : spot(spot), method(method) {};
};

class Instance
{
    typedef std::vector<TourNode> TourList;
    
    const SASTProblem &problem;
    
    double totalTime;
    double totalSatisfaction;
    double remainingStamina;
    
    TourList tour;
    
public:
    Instance(const SASTProblem& problem);
    
    double getTotalTime() const { return totalTime; };
    double getTotalSatisfaction() const { return totalSatisfaction; };
    double getRemainingStamina() const { return remainingStamina; };
    
    /**
     * This gets the total rest time based only on the amount of stamina. This
     * ignores any rest times assigned to nodes in the tour.
     */
    double getTotalRequiredRestTime() const { return remainingStamina < 0 ? -remainingStamina / problem.getHabitus() : 0.0; };
    
    const TourList getTour() const;
    const TourNode& getTourNode(unsigned index) const { return tour[index]; };
    
    void setTourNode(unsigned index, TourNode& node);
    
    // TODO define functions to add nodes to tour, to swap nodes, .. that also update the total values
    
    // TODO define functions (on demand) to check if a move is legal for this tour (i.e., if the resulting totals would still be valid)
    
};

#endif // INSTANCE_H
