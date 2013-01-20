#ifndef _SASTPROBLEM_H_
#define _SASTPROBLEM_H_

#include <string>
#include <vector>
#include <ostream>

class Method
{
	size_t id;
	std::string name;
	double satisfaction;
	double time;
	double stamina;

public:
	Method(size_t id, const std::string& name,double satisfaction,double time,double stamina):
		id(id), name(name),satisfaction(satisfaction),time(time),stamina(stamina){}

	size_t getID() const { return id; }
	const std::string& getName()const{return name;}
	double getSatisfaction()const{return satisfaction;}
	double getTime()const{return time;}
	double getStamina()const{return stamina;}
};

inline std::ostream& operator<<(std::ostream& os,const Method& m)
{
	return os << "Method "<<m.getName()<<": "<<m.getSatisfaction()<<" satisfaction after "<<m.getTime()<<" time costing "<<m.getStamina()<<" stamina";
}

class Spot
{
	size_t id;
	std::string name;
	double x,y;
	std::vector<Method*> methods;

public:
	Spot(size_t id, const std::string& name,double x,double y):id(id),name(name),x(x),y(y){}
	
	Spot(const Spot& obj):id(obj.id), name(obj.name),x(obj.x),y(obj.y) 
	{
	  // If we make Method mutable, we need to copy here as well
	  methods.insert(methods.begin(), obj.methods.begin(), obj.methods.end());
	}

	void addMethod(Method* m){methods.push_back(m);}
	const std::vector<Method*>& getMethods()const{return methods;}
	
	const Method& getMethod(unsigned index) const {return *methods[index];}
	Method& getMethod(unsigned index) {return *methods[index];}

	void removeMethod(unsigned index) { methods.erase(methods.begin() + index); }
	
	size_t getID() const { return id; }
	const std::string& getName()const{return name;}
	double getX()const{return x;}
	double getY()const{return y;}
};

inline std::ostream& operator<<(std::ostream& os,const Spot& s)
{
	os << "Spot "<<s.getName()<<" offers "<<s.getMethods().size()<<" methods at location ("<<s.getX()<<","<<s.getY()<<"):\n";

	for(const auto* m:s.getMethods())os << *m<<"\n";

	return os;
}

class SASTProblem
{
public:
	SASTProblem(const std::string& filename);

	SASTProblem(const SASTProblem& obj);
	
	void store(const std::string& filename);

	void printData()const;

	int getNumSpot()const;
	int getNumMeth()const;
	double getMaxTime()const;
	double getInitStamina()const;
	double getMaxStamina()const;
	double getAlpha()const;
	double getHabitus()const;
	double getVelocity()const;

	double getStartX()const{return startX;}
	double getStartY()const{return startY;}

	const std::vector<Spot*>& getSpots()const{return spots;}
	
	const Spot& getSpot(unsigned index) const {return *spots[index];}
	Spot& getSpot(unsigned index) {return *spots[index];}
	
	const Spot& getStartAsSpot() const {return start;}

	double getDistance(const Spot& a,const Spot& b)const;

	double getTravelSatisfactionCost(const Spot& a,const Spot& b)const;
	double getTravelTime(const Spot& a,const Spot& b)const;

	static double getDistance(double x1, double y1, double x2, double y2);

private:
	double maxTime;
	double initStamina;
	double maxStamina;

	double alpha; 		//satisfaction cost for traveling one unit of distance
	double habitus;		//regenerated stamina for one unit of resting time
	double velocity;	//travel speed between spots

	double startX,startY;
	Spot   start;

	std::vector<Spot*> spots;

	SASTProblem():maxTime(0),initStamina(0),maxStamina(0),alpha(0),habitus(0),velocity(0),startX(0),startY(0),start(-1, "Origin",0,0){}

	friend class SASTProblemGenerator;
};

#endif // _SASTPROBLEM_H_
