#include "PheromoneMatrix.h"

#include <cstring>
#include <iostream>

using namespace std;

PheromoneMatrix::PheromoneMatrix (Environment &env)
    : problem(env.getProblem()), debug(env.getConfig().isDebug())
{
    defaultTau = env.getConfig().getInitialTau();
    persistFactor = env.getConfig().getPersistFactor();
    minTau = env.getConfig().getTauMin();
    maxTau = env.getConfig().getTauMax();
    if (maxTau < minTau) maxTau = minTau;
    
    matrix = new double***[problem.getNumSpot()+1];
    memset(matrix, 0, sizeof(double***)*(problem.getNumSpot()+1));
    
    reset(defaultTau); 
}

PheromoneMatrix::~PheromoneMatrix()
{
    destroy();
    
    delete[] matrix;
}

void PheromoneMatrix::reset(float initTau)
{
    defaultTau = initTau;

    destroy();
}

float PheromoneMatrix::getTau(const TourNode start, const TourNode end) const
{
    if (double*** mi = matrix[start.spot+1]) {
	if (double **sj = mi[start.method]) {
	    if (double* mj = sj[end.spot+1]) {
		return mj[end.method];
	    }
	}
    }
    return defaultTau;
}

void PheromoneMatrix::setTau(const TourNode start, const TourNode end, double tau)
{
    // clamp first
    if (tau < minTau) tau = minTau;
    if (tau > maxTau) tau = maxTau;
    
    // now set the value, construct data structures lazily
    double*** mi = matrix[start.spot+1];
    if (!mi) {
	unsigned numMethods = start.spot != -1 ? problem.getSpot(start.spot).getMethods().size() : 1;
	mi = new double**[numMethods];
	memset(mi, 0, sizeof(double**)*numMethods);
	matrix[start.spot+1] = mi;
    }
    
    // get spot array for given start method
    double** sj = mi[start.method];
    if (!sj) {
	sj = new double*[problem.getNumSpot()+1];
	memset(sj, 0, sizeof(double*)*(problem.getNumSpot()+1));
	mi[start.method] = sj;
    }
    
    // get method array for end spot
    double* mj = sj[end.spot+1];
    if (!mj) {
	unsigned numMethods = end.spot != -1 ? problem.getSpot(end.spot).getMethods().size() : 1;
	mj = new double[numMethods];
	
	for (int i = 0; i < numMethods; i++) {
	    mj[i] = defaultTau;
	}
	
	sj[end.spot+1] = mj;
    }
    
    mj[end.method] = tau;
}

void PheromoneMatrix::addTau(const TourNode start, const TourNode end, double deltaTau)
{
    if (debug) {
	std::cerr << "Add Tau (" << (int)start.spot << "," << start.method << ") -> (" << end.spot << "," << end.method << ") += " << deltaTau << "\n";
    }
    
    setTau(start, end, getTau(start, end) + deltaTau);
}

void PheromoneMatrix::evaporate()
{
    defaultTau = persistFactor * defaultTau;
    
    unsigned numSpots = problem.getNumSpot()+1;
    
    for (int i = 0; i < numSpots; i++) {
	double*** mi = matrix[i];
	if (!mi) continue;
	
	unsigned numMethods = i > 0 ? problem.getSpot(i-1).getMethods().size() : 1;
	
	for (int j = 0; j < numMethods; j++) {
	    double** sj = mi[j]; 
	    if (!sj) continue;
	    
	    for (int k = 0; k < numSpots; k++) {
		double* mj = sj[k];
		if (!mj) continue;
		
		unsigned numEndMethods = k > 0 ? problem.getSpot(k-1).getMethods().size() : 1;
		for (int l = 0; l < numEndMethods; l++) {
		    double tau = persistFactor * mj[l];
		    if (tau < minTau) tau = minTau;
		    if (tau > maxTau) tau = maxTau;
		    mj[l] = tau;
		}
	    }
	}
    }
}

void PheromoneMatrix::destroy()
{
    unsigned numSpots = problem.getNumSpot()+1;
    
    for (int i = 0; i < numSpots; i++) {
	double*** mi = matrix[i];
	if (!mi) continue;
	
	unsigned numMethods = i > 0 ? problem.getSpot(i-1).getMethods().size() : 1;
	
	for (int j = 0; j < numMethods; j++) {
	    double** sj = mi[j]; 
	    if (!sj) continue;
	    
	    for (int k = 0; k < numSpots; k++) {
		double* mj = sj[k];
		if (mj) delete[] mj;
	    }
	    
	    delete[] sj;
	}
	
	delete[] mi;
    }
        
    memset(matrix, 0, sizeof(double***)*numSpots);
}

void PheromoneMatrix::print(ostream& out)
{
    unsigned numSpots = problem.getNumSpot()+1;
    
    unsigned count = numSpots;
    
    out << "Pheromone matrix: default tau = " << defaultTau << endl;
    
    for (int i = 0; i < numSpots; i++) {
	double*** mi = matrix[i];
	if (!mi) continue;
	
	unsigned numMethods = i > 0 ? problem.getSpot(i-1).getMethods().size() : 1;
	
	count += numMethods;
	
	for (int j = 0; j < numMethods; j++) {
	    double** sj = mi[j]; 
	    if (!sj) continue;
	
	    count += numSpots;
	    
	    out << " - (Spot,Method) = (" << (i-1) << "," << j << ") -> ";
	    
	    for (int k = 0; k < numSpots; k++) {
		double* mj = sj[k];
		if (!mj) continue;
		
		unsigned numEndMethods = k > 0 ? problem.getSpot(k-1).getMethods().size() : 1;
		
		count += 2 * numEndMethods;
		
		for (int l = 0; l < numEndMethods; l++) {
		    double tau = mj[l];
		    
		    out << "(" << (k-1) << "," << l << ") = " << tau << ", ";
		}
		
	    }
	    
	    out << endl;
	}
    }    
    
    count *= 4;
    
    out << "  Size: " << count << " bytes\n";
}

