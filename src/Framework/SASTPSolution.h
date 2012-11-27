#ifndef SASTPSOLUTION_H_
#define SASTPSOLUTION_H_

#include <vector>
#include <string>
#include <ostream>

class SASTProblem;

struct Stop
{
	int spot;
	int method;
	double restingTime;

	Stop(int spot, int method,double restingTime):spot(spot),method(method),restingTime(restingTime){}
};

class SASTPSolution
{
	const SASTProblem& prob;
	std::vector<Stop> tour;
	double stamina;
	double satisfaction;
	double tourtime;
	bool finished;

public:
	SASTPSolution(const SASTProblem& p);

	void addStop(int spot,int method,double restTime);
	void finishTour();

	void load(const std::string& filename);
	void store(const std::string& filename)const;
	void print(std::ostream& os, bool pretty = true)const;

	double getStamina()const{return stamina;}
	double getSatisfaction()const{return satisfaction;}
	double getTourTime()const{return tourtime;}

	const std::vector<Stop>& getTour()const{return tour;}

	bool isValid(bool verbose=false)const;
};

inline std::ostream& operator<<(std::ostream& os, const SASTPSolution& s)
{
	s.print(os);
	return os;
}

#endif /* SASTPSOLUTION_H_ */
