#include "Instance.h"

#include <assert.h>
#include <iostream>
#include <set>
#include <algorithm>

using namespace std;


Instance::Instance(const SASTProblem &problem)
: problem(problem), tourTime(0.0), totalSatisfaction(0.0), 
  remainingStamina(problem.getInitStamina())
{
    usedSpots.resize(problem.getSpots().size(), false);
}

Instance::Instance(const Instance& instance)
: problem(instance.getProblem()), tourTime(instance.getTourTime()),
  totalSatisfaction(instance.getTotalSatisfaction()),
  remainingStamina(instance.getRemainingStamina()),
  tour(instance.tour), usedSpots(instance.usedSpots)
{
}

Instance& Instance::operator=(const Instance& instance)
{
    if (this == &instance) return *this;
    
    assert(&problem == &instance.getProblem());
    
    tourTime = instance.getTourTime();
    totalSatisfaction = instance.getTotalSatisfaction();
    remainingStamina = instance.getRemainingStamina();
    tour = instance.tour;
    usedSpots = instance.usedSpots;  
    return *this;
}

const Spot& Instance::getSpot ( TourNode node ) const 
{
    return (node.spot == -1 || node.spot == problem.getSpots().size()) ? problem.getStartAsSpot() : problem.getSpot(node.spot); 
}


void Instance::clear()
{
    tour.clear();
    totalSatisfaction = 0.0;
    tourTime = 0.0;
    remainingStamina = problem.getInitStamina();
    usedSpots.clear();
    usedSpots.resize(problem.getSpots().size(), false);
}

double Instance::getTotalRequiredRestTime() const 
{
    return remainingStamina < 0 ? -remainingStamina / problem.getHabitus() : 0.0; 
}

unsigned int Instance::getNodeIndex(unsigned int spot) const
{
    unsigned index = 0;
   
    for (const auto& node : tour) {
	if (node.spot == spot) return index;
	index++;
    }
    
    // not found
    return -1;
}

unsigned Instance::insertNode(unsigned int index, TourNode node)
{
    addTourValues( getInsertDeltaValues(index, node) );
    
    tour.insert( tour.begin() + index, node);
    
    usedSpots[node.spot] = true;
    
    return index;
}

void Instance::insertNodes(const TourNodeIndexList& nodes)
{
    // TODO could be implemented more efficiently: insert dummy elements first to move tail,
    //      then move elements between first and last insert position into correct position
    
    for (int i = nodes.size(); i >= 0; --i) {
	insertNode(nodes[i].first, nodes[i].second);
    }
    
}


void Instance::updateNode(unsigned int index, TourNode node)
{
    addTourValues( getUpdateDeltaValues(index, node) );
    
    usedSpots[ tour[index].spot ] = false;
    
    tour[index] = node;
    
    usedSpots[node.spot] = true;
}

void Instance::deleteNode(unsigned int index)
{
    addTourValues( getDeleteDeltaValues(index) );
    
    usedSpots[ tour[index].spot ] = false;
    
    tour.erase(tour.begin() + index);
}

void Instance::deleteNodes(TourNodeIndexList& nodes)
{
    // TODO this can be implemented by using just one sweep over the array:
    //      - start at the first node, copy elements up one position
    //      - increase copy distance when trying to copy a node that has to be deleted
    //      - afterwards, delete whole range to delete multiple nodes
    //      - implement a getDeltaDeleteNodes() cost function (this is the tricky part..)
    
    for (int i = 0; i < nodes.size(); ++i) {
	nodes[i].first -= i;
	deleteNode(nodes[i].first);
    }
    
}


void Instance::crossOverEdges(unsigned int firstEdge, unsigned int secondEdge)
{
    addTourValues( getCrossOverDeltaValues(firstEdge, secondEdge) );
    
    if (firstEdge > secondEdge) {
	unsigned tmp = firstEdge;
	firstEdge = secondEdge;
	secondEdge = tmp;
    }
    
    unsigned steps = (secondEdge - firstEdge) / 2;
    
    for ( int pos = 0; pos < steps; pos++) {
	unsigned p1 = firstEdge + pos;
	unsigned p2 = secondEdge - pos - 1;
	
	TourNode tmp = tour[p1];
	tour[p1] = tour[p2];
	tour[p2] = tmp;
    }
}


bool Instance::containsSpot ( unsigned int spotId ) const 
{
    return usedSpots[spotId];
}


TourValues Instance::getInsertDeltaValues(unsigned int index, TourNode node)
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

TourValues Instance::getUpdateDeltaValues(unsigned int index, TourNode node)
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

TourValues Instance::getCrossOverDeltaValues(unsigned int firstEdge, unsigned int secondEdge)
{
    const Spot& f1 = getSpot(firstEdge - 1);
    const Spot& f2 = getSpot(firstEdge);
    const Spot& s2 = getSpot(secondEdge - 1);
    const Spot& s1 = getSpot(secondEdge);
    
    double diff = problem.getDistance(f1, s2) + problem.getDistance(s1, f2) 
                - problem.getDistance(f1, f2) - problem.getDistance(s1, s2);

    double deltaTime = diff / problem.getVelocity();
    double deltaSatisfaction = -diff * problem.getAlpha();

    return TourValues(deltaTime, deltaSatisfaction, 0.0);
}

TourValues Instance::getStepValues(const TourNode& from, const TourNode& to) const
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

set<unsigned> Instance::getUnusedSpotIDs() const
{
    set<unsigned> unused;
    
    unsigned spots = problem.getSpots().size();
    
    for (unsigned i = 0; i < spots; i++) {
	if (!usedSpots[i]) {
	    unused.insert(i);
	}
    }
    
    return unused;
}

bool Instance::isValid() const
{
    // Stamina constraint is already included in the total tour time
    return (getTotalTime() <= problem.getMaxTime());
}

bool Instance::isValid(TourValues delta) const
{
    double time = tourTime + delta.tourTime;
    double stamina = remainingStamina - delta.stamina;
    if (stamina < 0) {
	time += -stamina / problem.getHabitus();
    }
    
    return time < problem.getMaxTime();
}

double Instance::getSatisfactionPerTotalTimeRatio(TourValues delta) const
{
    double time = tourTime + delta.tourTime;
    double stamina = remainingStamina - delta.stamina;
    if (stamina < 0) {
	time += -stamina / problem.getHabitus();
    }
    
    double ratio = delta.satisfaction / time;
    return ratio;
}

std::vector< unsigned int > Instance::getRatioSortedNodes() const
{
    unsigned tourLength = tour.size();
    
    vector<unsigned> sorted;
    sorted.reserve(tourLength);
    
    vector<double> ratios;
    ratios.reserve(tourLength);
    
    unsigned index = 0;
    TourNode lastNode = getHotelNode();
    
    for (auto& node : tour) {
	
	double ratio = getSatisfactionPerTotalTimeRatio(getStepValues(lastNode, node));
	
	auto it = upper_bound(ratios.begin(), ratios.end(), ratio);
	ratios.insert(it, ratio);
	
	sorted.insert(sorted.begin() + (it - ratios.begin()), index);
	
	lastNode = node;
	index++;
    }
    
    return sorted;
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
	
	currStamina += rest * problem.getHabitus();
	
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

