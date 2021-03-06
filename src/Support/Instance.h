#ifndef INSTANCE_H
#define INSTANCE_H

#include "Framework/SASTProblem.h"
#include "Framework/SASTPSolution.h"

#include <vector>
#include <set>

struct TourNode {
    unsigned spot;
    unsigned method;
    
    TourNode() : spot(-1), method(0) {}
    
    TourNode(unsigned spot, unsigned method) : spot(spot), method(method) {}

    bool operator==(const TourNode& n) {
	return spot == n.spot && method == n.method;
    }
    bool operator!=(const TourNode& n) {
	return spot != n.spot || method != n.method;
    }
};

struct TourValues {
    
    /**
     * The time for the tour, excluding rests
     */
    double tourTime;
    
    /**
     * The satisfaction gained.
     */
    double satisfaction;
    
    /**
     * The amount of stamina spent, excluding rests (a positive value means more stamina is used)
     */
    double stamina;
    
    TourValues() : tourTime(0.0), satisfaction(0.0), stamina(0.0) {}
    
    TourValues(double tourTime, double satisfaction, double stamina) : 
       tourTime(tourTime), satisfaction(satisfaction), stamina(stamina) {}
       
    void addDeltaDistance(const SASTProblem& problem, double deltaDist) {
	tourTime += deltaDist / problem.getVelocity();
	satisfaction -= deltaDist * problem.getAlpha();
    }
    
    const TourValues operator+(const TourValues& v) {
	return TourValues(tourTime + v.tourTime, satisfaction + v.satisfaction, stamina + v.stamina);
    }
    
    const TourValues operator-(const TourValues& v) {
	return TourValues(tourTime - v.tourTime, satisfaction - v.satisfaction, stamina - v.stamina);
    }
    
    const TourValues operator+(const Method& m) {
	return TourValues(tourTime + m.getTime(), satisfaction + m.getSatisfaction(), stamina + m.getStamina());
    }
    
    const TourValues operator-(const Method& m) {
	return TourValues(tourTime - m.getTime(), satisfaction - m.getSatisfaction(), stamina - m.getStamina());
    }
    
    TourValues& operator+=(const TourValues& v) {
	tourTime += v.tourTime; 
	satisfaction += v.satisfaction;
	stamina += v.stamina;
	return *this;
    }
    
    TourValues& operator-=(const TourValues& v) {
	tourTime -= v.tourTime; 
	satisfaction -= v.satisfaction;
	stamina -= v.stamina;
	return *this;
    }
    
    TourValues &operator+=(const Method& m) {
	tourTime += m.getTime();
	satisfaction += m.getSatisfaction();
	stamina += m.getStamina();
	return *this;
    }
        
    TourValues &operator-=(const Method& m) {
	tourTime -= m.getTime();
	satisfaction -= m.getSatisfaction();
	stamina -= m.getStamina();
	return *this;
    }
    
};



typedef std::vector<TourNode> TourList;

/**
 * List of pairs of (tour index,tour node)
 */
typedef std::vector< std::pair<unsigned, TourNode> > TourNodeIndexList;



class Instance
{
    const SASTProblem &problem;
    
    double tourTime;
    double totalSatisfaction;
    double remainingStamina;
    
    TourList tour;
    
    std::vector<bool> usedSpots;
    std::set<unsigned> unusedSpotIDs;
    
public:
    Instance(const SASTProblem& problem);

    Instance(const Instance& instance);
    
    Instance& operator=(const Instance& instance);
    
    const SASTProblem& getProblem() const { return problem; }
    
    /**
     * Get the time required for the tour excluding resting times.
     */
    double getTourTime() const { return tourTime; };
    double getTotalSatisfaction() const { return totalSatisfaction; };
    double getRemainingStamina() const { return remainingStamina; };

    TourValues getTourValues() const { return TourValues(tourTime, totalSatisfaction, -remainingStamina); }
    
    /**
     * Get the total tour time, including any resting times for filling up stamina.
     */
    double getTotalTime() const { return tourTime + getTotalRequiredRestTime(); }
    
    /**
     * This gets the total rest time based only on the amount of remaining stamina.
     * This function ignores any rest times assigned to nodes in the tour.
     */
    double getTotalRequiredRestTime() const;
    
    const TourList  getTour() const { return tour; }
    
    bool empty() const { return tour.empty(); }
    
    TourNode getNode(int index) const { return (index == -1 || index == tour.size()) ? getHotelNode() : tour.at(index); }
    
    TourNode getHotelNode() const { return TourNode(-1, 0); }

    TourNode getLastNode() const { return tour.empty() ? getHotelNode() : tour.back(); }
    
    /**
     * Get the number of spots in the tour (excluding the hotel).
     */
    size_t getTourLength() const { return tour.size(); };
    
    unsigned getSpotIndex(unsigned index) const { return index == -1 ? -1 : tour.at(index).spot; }
    unsigned getMethodIndex(unsigned index) const { return index == -1 ? 0 : tour.at(index).method; }
    
    /**
     * Get the index of the node in this tour refering to spot.
     */
    unsigned getNodeIndex(unsigned spot) const;
    
    const Spot&   getSpot(TourNode node) const;
    const Method& getMethod(TourNode node) const { return problem.getSpot(node.spot).getMethod(node.method); }
    
    const Spot&   getSpot(int index) const { return getSpot(getNode(index)); }
    const Method& getMethod(int index) const { return getMethod(getNode(index)); }
    
    void clear(); 
    
    void updateNode(unsigned index, TourNode node);
    void updateNode(unsigned index, unsigned spot, unsigned method) { updateNode(index, TourNode(spot, method)); }
    
    /**
     * @return the index of the new node in the tour.
     */    
    unsigned insertNode(unsigned index, TourNode node);
    
    /**
     * @return the index of the new node in the tour.
     */
    unsigned insertNode(unsigned index, unsigned spot, unsigned method) { return insertNode(index, TourNode(spot, method)); }
    
    void insertNodes(const TourNodeIndexList& nodes);
    
    /**
     * @return the index of the new node in the tour.
     */
    unsigned addNode(TourNode node) { return insertNode(tour.size(), node); }
    
    /**
     * @return the index of the new node in the tour.
     */
    unsigned addNode(unsigned spot, unsigned method) { return addNode(TourNode(spot, method)); }
    
    void deleteNode(unsigned index);
    
    /**
     * Delete the given nodes from the tour (first value in pair is the index to delete).
     * List must be sorted by tour index. The index value of the nodes will be updated
     * to point to the next index in the tour (so that insertNodes works correctly on the result).
     */
    void deleteNodes(TourNodeIndexList& nodes);
    
    /**
     * Perform a crossover of edges (firstEdge-1,firstEdge) and (secondEdge-1,secondEdge)
     */
    void crossOverEdges(unsigned firstEdge, unsigned secondEdge);
    
    bool containsSpot(unsigned spotId) const;
    bool containsSpot(TourNode node) const { return containsSpot(node.spot); }
    
    /**
     * @return a set of currently unused spot indices.
     */
    std::set<unsigned> getUnusedSpotIDs() const;
    
    /**
     * Check if the current instance satisfies all constraints
     */
    bool isValid() const;

    bool isValid(const TourValues& delta) const;
    
    double getSatisfactionPerTotalTimeRatio(const TourValues& values) const;
    
    /**
     * @param fromIndex index of the previous node
     * @param toSpot spot to go to and to select method from
     * @param bestMethod will be set to the best method id of the spot
     * @return the satisfaction by time ratio to visit this spot using the best method
     */
    double getBestMethodRatio(unsigned fromIndex, const Spot& toSpot, unsigned& bestMethod) const;
    
    /**
     * Get the indices of the tour nodes, sorted by satisfaction / time ratio
     */
    std::vector<unsigned> getRatioSortedNodes() const;
    
    TourValues getUpdateDeltaValues(unsigned index, TourNode node);
    TourValues getInsertDeltaValues(unsigned index, TourNode node);
    TourValues getDeleteDeltaValues(unsigned index);
    
    TourValues getCrossOverDeltaValues(unsigned firstEdge, unsigned secondEdge);
    
    TourValues getTravelDeltaValues(const Spot& from, const Spot& to) const;
    
    /**
     * Get the deltas for all values for a step from a node to another node, and then doing the action at the 'to' node.
     */
    TourValues getStepValues(const TourNode& from, const TourNode& to) const;
    TourValues getStepValues(int fromNode, int toNode) const { return getStepValues(getNode(fromNode), getNode(toNode)); }
    TourValues getStepValues(unsigned fromSpot, unsigned fromMethod, unsigned toSpot, unsigned toMethod) const
                  { return getStepValues(TourNode(fromSpot, fromMethod), TourNode(toSpot, toMethod)); }
    TourValues getStepValues(unsigned toNode) const { return getStepValues(getNode(toNode-1), getNode(toNode)); }
    
    SASTPSolution* createSolution(const SASTProblem& origProblem) const;
  
private:
    
    void addTourValues(TourValues values);
    
};

#endif // INSTANCE_H
