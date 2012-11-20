#ifndef INSTANCE_H
#define INSTANCE_H

#include "Framework/SASTProblem.h"
#include "Framework/SASTPSolution.h"

#include <vector>

struct TourNode {
    unsigned spot;
    unsigned method;
    
    TourNode(unsigned spot, unsigned method) : spot(spot), method(method) {};
};

struct TourValue {
    double tourTime;
    double totalTime;
    double satisfaction;
    double stamina;
    
    TourValue(double tourTime, double totalTime, double satisfaction, double stamina) : 
       tourTime(tourTime), totalTime(totalTime), satisfaction(satisfaction), stamina(stamina) {};
};

typedef std::vector<TourNode> TourList;

class Instance
{
    const SASTProblem &problem;
    
    // TODO maybe change this to totalTour, calculate tourTime from totalTour instead
    double tourTime;
    double totalSatisfaction;
    double remainingStamina;
    
    TourList tour;
    
public:
    Instance(const SASTProblem& problem);
    
    const SASTProblem& getProblem() const { return problem; };
    
    /**
     * Get the time required for the tour excluding resting times.
     */
    double getTourTime() const { return tourTime; };
    double getTotalSatisfaction() const { return totalSatisfaction; };
    double getRemainingStamina() const { return remainingStamina; };

    /**
     * Get the total tour time, including any resting times for filling up stamina.
     */
    double getTotalTime() const { return tourTime + getTotalRequiredRestTime(); };
    
    /**
     * This gets the total rest time based only on the amount of remaining stamina.
     * This function ignores any rest times assigned to nodes in the tour.
     */
    double getTotalRequiredRestTime() const;
    
    const TourList  getTour() const { return tour; };
    const TourNode& getNode(unsigned index) const { return tour[index]; };
    
    /**
     * Get the number of spots in the tour (excluding the hotel).
     */
    size_t getTourLength() const { return tour.size(); };
    
    unsigned getSpotIndex(unsigned index) const { return tour[index].spot; };
    unsigned getMethodIndex(unsigned index) const { return tour[index].method; };
    
    const Spot&   getSpot(const TourNode& node) const { return problem.getSpot(node.spot); };
    const Method& getMethod(const TourNode& node) const { return problem.getSpot(node.spot).getMethod(node.method); };    
    
    const Spot&   getSpot(unsigned index) const { return getSpot(getNode(index)); };
    const Method& getMethod(unsigned index) const { return getMethod(getNode(index)); };
    
    void clear(); 
    
    void updateNode(unsigned index, const TourNode& node);
    void updateNode(unsigned index, unsigned spot, unsigned method) { updateNode(index, TourNode(spot, method)); };
    
    void insertNode(unsigned index, const TourNode& node);
    void insertNode(unsigned index, unsigned spot, unsigned method) { insertNode(index, TourNode(spot, method)); };
    
    void addNode(const TourNode &node) { insertNode(tour.size(), node); };
    void addNode(unsigned spot, unsigned method) { addNode(TourNode(spot, method)); };
    
    void deleteNode(unsigned index);
    
    /**
     * Check if the current instance satisfies all constraints
     */
    bool isValid() const;

    TourValue getUpdateDeltaValues(unsigned index, const TourNode& node);
    TourValue getInsertDeltaValues(unsigned index, const TourNode& node);
    TourValue getDeleteDeltaValues(unsigned index);

    SASTPSolution* createSolution() const;
    
};

#endif // INSTANCE_H
