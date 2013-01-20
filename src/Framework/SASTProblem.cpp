#include "SASTProblem.h"

#include <cassert>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>

using namespace std;

SASTProblem::SASTProblem(const std::string& filename):
		maxTime(0),initStamina(0),maxStamina(0),alpha(0),habitus(0),velocity(0),startX(0),startY(0),start(-1, "Origin",0,0)
{
	ifstream ifs(filename);

	if(ifs)
	{
		string token;

		ifs>>token>>maxTime;
		assert(token=="maxtime");

		ifs>>token>>initStamina;
		assert(token=="initst");

		ifs>>token>>maxStamina;
		assert(token=="maxst");

		ifs>>token>>alpha;
		assert(token=="alpha");

		ifs>>token>>habitus;
		assert(token=="habitus");

		ifs>>token>>velocity;
		assert(token=="speed");

		ifs>>token>>startX>>startY;
		assert(token=="start");
		
		start = Spot(-1, "Origin",startX, startY);

		int num_spots=0, num_methods=0;

		ifs>>token>>num_spots;
		assert(token=="numspot");

		ifs>>token>>num_methods;
		assert(token=="nummeth");

		unsigned method_id = 0;
		
		for(int i=0;i<num_spots+num_methods;++i) //sum is number of lines that have to be read
		{
			string line;
			do{getline(ifs, line);}while(line.length()==0);

			stringstream s(line, std::stringstream::in);

			s>>token;
			if(token=="spot")
			{
				size_t id;
				string name;
				double x,y;
				s>>id>>name>>x>>y;
				assert(id == spots.size());
				if(!spots.empty()) assert(!spots.back()->getMethods().empty());
				
				spots.push_back(new Spot(id,name,x,y));
				
				method_id = 0;
			}
			if(token=="method")
			{
				string name;
				double satisfaction,time,stamina;

				s>>name>>satisfaction>>time>>stamina;
				assert(!spots.empty());

				spots.back()->addMethod(new Method(method_id, name,satisfaction,time,stamina));
				
				method_id++;
			}
		}
	}
	else
	{
		throw logic_error("failed to open input file "+filename);
	}
	if(initStamina>maxStamina)
	{
		throw logic_error("Error in input file: initial stamina > maximum stamina\n");
	}
}

SASTProblem::SASTProblem(const SASTProblem& obj)
: maxTime(obj.maxTime),initStamina(obj.initStamina),maxStamina(obj.maxStamina),alpha(obj.alpha),habitus(obj.habitus),
  velocity(obj.velocity),startX(obj.startX),startY(obj.startY),start(obj.start)
{
  for (const Spot* spot : obj.spots) {
    spots.push_back(new Spot(*spot));
  }
}

void SASTProblem::store(const std::string& filename)
{
	ofstream os(filename);

	if(!os)throw logic_error("no output stream to "+filename);

	os << "maxtime "<<maxTime<<"\n";
	os << "initst "<<initStamina<<"\n";
	os << "maxst "<<maxStamina<<"\n";
	os << "alpha "<<alpha<<"\n";
	os << "habitus "<<habitus<<"\n";
	os << "speed "<<velocity<<"\n";
	os << "start "<<startX<<" "<<startY<<"\n";

	os << "numspot "<<spots.size()<<"\n";

	int numMeth=0;
	for(const auto* s:spots)numMeth+=s->getMethods().size();

	os << "nummeth "<<numMeth<<"\n";

	for(int i=0;i<spots.size();++i)
	{
		const Spot& spot=*spots.at(i);
		os << "spot "<<i<<" "<<spot.getName()<<" "<<spot.getX()<<" "<<spot.getY()<<"\n";

		for(const auto* m:spot.getMethods())
		{
			os << "method "<<m->getName()<<" "<<m->getSatisfaction()<<" "<<m->getTime()<<" "<<m->getStamina()<<"\n";
		}
	}
}



void SASTProblem::printData()const
{
	cout << "Problem has " << getNumSpot() << " spots and " << getNumMeth() << " methods\n";
	cout << "MaxTime = " << maxTime << "\n";
	cout << "MaxStamina = " << maxStamina << "\n";
	cout << "InitStamina = " << initStamina << "\n";
	cout << "alpha = " << alpha << "\n";
	cout << "habitus = " << habitus << "\n";
	cout << "speed = " << velocity << "\n";
	cout << "Start is at ("<<startX<<","<<startY<<")\n";

	for(const auto& s:spots)cout << s<<"\n";
}

int SASTProblem::getNumSpot()const
{
	return spots.size();
}

int SASTProblem::getNumMeth()const
{
	int ret=0;

	for(const auto* e:spots)ret+=e->getMethods().size();

	return ret;
}

double SASTProblem::getMaxTime()const
{
	return maxTime;
}

double SASTProblem::getInitStamina()const
{
	return initStamina;
}

double SASTProblem::getMaxStamina()const
{
	return maxStamina;
}

double SASTProblem::getAlpha()const
{
	return alpha;
}

double SASTProblem::getHabitus()const
{
	return habitus;
}

double SASTProblem::getVelocity()const
{
	return velocity;
}

double SASTProblem::getDistance(double x1, double y1, double x2, double y2)
{
	const double dx=x1-x2;
	const double dy=y1-y2;

	return sqrt(dx*dx+dy*dy);
}

double SASTProblem::getDistance(const Spot& sA,const Spot& sB)const
{
	return getDistance(sA.getX(),sA.getY(),sB.getX(),sB.getY());
}

double SASTProblem::getTravelSatisfactionCost(const Spot& a,const Spot& b)const
{
	return alpha*getDistance(a,b);
}

double SASTProblem::getTravelTime(const Spot& a,const Spot& b)const
{
	return getDistance(a,b)/velocity;
}

