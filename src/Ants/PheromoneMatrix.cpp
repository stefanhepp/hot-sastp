#include "PheromoneMatrix.h"

#include <cstring>

PheromoneMatrix::PheromoneMatrix (Environment &env)
    : problem(env.getProblem())
{
    defaultTau = env.getConfig().getInitialTau();
    persistFactor = env.getConfig().getPersistFactor();
    minTau = env.getConfig().getTauMin();
    maxTau = env.getConfig().getTauMax();
    
    matrix = new double***[problem.getNumSpot()];
    memset(matrix, 0, sizeof(double***)*problem.getNumSpot());
    
    reset(defaultTau); 
}

PheromoneMatrix::~PheromoneMatrix()
{
    destroy();
    
    delete matrix;
}

void PheromoneMatrix::reset(float initTau)
{
    defaultTau = initTau;

    destroy();
}

float PheromoneMatrix::getTau(const TourNode start, const TourNode end) const
{
    if (double*** mi = matrix[start.spot]) {
	if (double **sj = mi[start.method]) {
	    if (double* mj = sj[end.spot]) {
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
    double*** mi = matrix[start.spot];
    if (!mi) {
	unsigned numMethods = problem.getSpot(start.spot).getMethods().size();
	mi = new double**[numMethods];
	memset(mi, 0, sizeof(double**)*numMethods);
	matrix[start.spot] = mi;
    }
    
    // get spot array for given start method
    double** sj = mi[start.method];
    if (!sj) {
	sj = new double*[problem.getNumSpot()];
	memset(sj, 0, sizeof(double*)*problem.getNumSpot());
	mi[start.method] = sj;
    }
    
    // get method array for end spot
    double* mj = sj[end.spot];
    if (!mj) {
	unsigned numMethods = problem.getSpot(end.spot).getMethods().size();
	mj = new double[numMethods];
	
	for (int i = 0; i < numMethods; i++) {
	    mj[i] = defaultTau;
	}
	
	sj[end.spot] = mj;
    }
    
    mj[end.method] = tau;
}

void PheromoneMatrix::addTau(const TourNode start, const TourNode end, double deltaTau)
{
    setTau(start, end, getTau(start, end) + deltaTau);
}

void PheromoneMatrix::evaporate()
{
    defaultTau = persistFactor * defaultTau;
    
    unsigned numSpots = problem.getNumSpot();
    
    for (int i = 0; i < numSpots; i++) {
	double*** mi = matrix[i];
	if (!mi) continue;
	
	unsigned numMethods = problem.getSpot(i).getMethods().size();
	
	for (int j = 0; j < numMethods; j++) {
	    double** sj = mi[j]; 
	    if (!sj) continue;
	    
	    for (int k = 0; k < numSpots; k++) {
		double* mj = sj[k];
		if (!mj) continue;
		
		unsigned numEndMethods = problem.getSpot(k).getMethods().size();
		for (int l = 0; l < numEndMethods; l++) {
		    mj[l] = persistFactor * mj[l];
		}
	    }
	}
    }
}

void PheromoneMatrix::destroy()
{
    unsigned numSpots = problem.getNumSpot();
    
    for (int i = 0; i < numSpots; i++) {
	double*** mi = matrix[i];
	if (!mi) continue;
	
	unsigned numMethods = problem.getSpot(i).getMethods().size();
	
	for (int j = 0; j < numMethods; j++) {
	    double** sj = mi[j]; 
	    if (!sj) continue;
	    
	    for (int k = 0; k < numSpots; k++) {
		double* mj = sj[k];
		if (mj) delete mj;
	    }
	    
	    delete sj;
	}
	
	delete mi;
    }
        
    memset(matrix, 0, sizeof(double***)*problem.getNumSpot());
}

