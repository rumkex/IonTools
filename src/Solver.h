#pragma once
#include "Constraint.h"
#include "LinearConstraint.h"
#include "PositiveConstraint.h"
#include <vector>

using namespace std;

class Solver
{	
private:
	vector<Constraint*> constraints;
protected:
    vector<double> x, dx, weights;
	
public:
	Solver(int nodeCount): x(nodeCount), dx(nodeCount), weights(nodeCount, 1.0) {}
	virtual ~Solver() 
	{
		for (auto c: constraints)
			delete c;
	}
	
	virtual void addConstraint(Constraint* constraint)
	{
		constraint->parent = this;
		constraints.push_back(constraint);
	}
	
	virtual Constraint* getConstraint(int id)
	{
		return constraints[id];
	}

	virtual void iterate()
	{
		for(int i = 0; i < constraints.size(); i++)
			constraints[i]->iterate();
	}

	virtual void solve(LinearConstraint* constraint) = 0;
	virtual void solve(PositiveConstraint* constraint) = 0;
	

	void setWeights(const vector<double>& weights)
	{
		this->weights = weights;
	}

	void setResult(const vector<double>& result)
	{
		x = result;
	}
	
	vector<double>& getResult()
	{
		return x;
	}
	
};
