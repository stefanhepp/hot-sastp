#include "SASTPSolution.h"

#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iostream>
#include <set>

#include "SASTProblem.h"

using namespace std;

SASTPSolution::SASTPSolution(const SASTProblem& p):
	prob(p),stamina(p.getInitStamina()),satisfaction(0),tourtime(0),finished(false)
{
}

void SASTPSolution::addStop(int spot, int method, double restTime)
{
	if(finished)throw logic_error("already finished");
	if(restTime<0)throw logic_error("negative rest time");

	const Spot&   s=prob.getSpot(spot);
	const Method& m=s.getMethod(method);
	const Spot&   lastSpot=tour.empty()?prob.getStartAsSpot():prob.getSpot(tour.back().spot);

	Stop stop(spot,method,restTime);

	//traveltime to spot
	tourtime+=prob.getTravelTime(lastSpot,s);

	//time of method
	tourtime+=m.getTime();

	//time of rest
	tourtime+=restTime;

	//satisfaction cost of travel
	satisfaction-=prob.getTravelSatisfactionCost(lastSpot,s);

	//satisfaction of method
	satisfaction+=m.getSatisfaction();

	//stamina cost of method
	stamina-=m.getStamina();

//	if(stamina<0)throw logic_error("stamina is negative");

	//regen stamina from rest
	stamina+=prob.getHabitus()*restTime;
	stamina=min(stamina,prob.getMaxStamina());

	//actually add stop
	tour.push_back(stop);
}

void SASTPSolution::finishTour()
{
	if(finished)throw logic_error("already finished");
	finished=true;

	//add return trip to time and satisfaction, if the tour actually leaves the starting location
	if(tour.empty())return;

	const Spot& lastSpot=prob.getSpot(tour.back().spot);
	const Spot  finishSpot("Origin",prob.getStartX(),prob.getStartY());

	tourtime+=prob.getTravelTime(lastSpot,finishSpot);
	satisfaction-=prob.getTravelSatisfactionCost(lastSpot,finishSpot);
}

bool SASTPSolution::isValid(bool verbose) const
{
	bool result=true;

	if(!finished)
	{
		if(verbose)cout << "Solution not valid because it is not finished\n";
		result=false;
	}
	if(tourtime>prob.getMaxTime())
	{
		if(verbose)cout << "Solution not valid because it exceeds maxtime ("<< prob.getMaxTime()<<")\n";
		result=false;
	}
	if(stamina<0)
	{
		if(verbose)cout << "Solution not valid because end stamina of "<<stamina<<" < 0\n";
		result=false;
	}

	set<unsigned> spots;
	
	//check stamina development
	double staminatrace=prob.getInitStamina();
	int i=0;
	for(const auto& stop:tour)
	{
		if (!spots.insert(stop.spot).second) {
			cout << "Spot " << stop.spot << " appears multiple times in tour" << endl;
			result = false;
		}
		    
		++i;
		staminatrace-=prob.getSpot(stop.spot).getMethod(stop.method).getStamina();
		if(staminatrace<0)
		{
			if(verbose)cout << "Solution not valid because after stop "<<i<<" stamina is at "<<staminatrace<<"\n";
			result=false;
		}
		staminatrace = min(staminatrace + prob.getHabitus()*stop.restingTime, prob.getMaxStamina());
	}

	if(staminatrace!=stamina) {
	    cerr << "Expected stamina: " << stamina << ", calculated stamina: " << staminatrace << "; inconsistent!" << endl;
	    throw logic_error("stamina inconsistent");
	}

	if(result && verbose)cout << "Solution is valid\n";

	return result;
}

void SASTPSolution::load(const std::string& filename)
{
	if(finished)throw logic_error("already finished");
	if(!tour.empty())throw logic_error("tour already started");

	ifstream is(filename);

	if(!is)throw logic_error("reading stream to "+filename+" not openable");

	for(string line;getline(is,line);)
	{
		if(line.empty())continue;
		if(*line.begin()=='#')continue;

		stringstream s;
		s<<line;

		Stop stop(0,0,0);
		s >> stop.spot>>stop.method>>stop.restingTime;

		addStop(stop.spot,stop.method,stop.restingTime);
	}

	finishTour();
}

void SASTPSolution::store(const std::string& filename)const
{
	if(!finished)throw logic_error("can only store finished solutions");

	ofstream of(filename);

	if(!of)throw logic_error("writing stream to "+filename+" not openable");

	of<<"#Solution to SASTP\n";
	of<<"#Spot Method Resting Time\n";

	for(const auto& stop:tour)
	{
		of<<stop.spot<<" "<<stop.method<<" "<<stop.restingTime<<"\n";
	}
}

void SASTPSolution::print(std::ostream& os, bool pretty)const
{
    if (pretty) {
	os << "Solution has "<<tour.size()<<" stops, taking "<<tourtime<<" time and achieving "<<satisfaction<<" satisfaction, with "<<stamina<<" stamina remaining\n";
	os << "Stops:\n";

	for(const auto& s:tour)
	{
		os << "Spot "<<s.spot<<" with method "<<s.method<<" and then resting "<<s.restingTime<<"\n";
	}
    } else {
    	os<<"#Solution to SASTP\n";
	os<<"#Spot Method Resting Time\n";

	for(const auto& stop:tour)
	{
		os<<stop.spot<<" "<<stop.method<<" "<<stop.restingTime<<"\n";
	}
    }
}
