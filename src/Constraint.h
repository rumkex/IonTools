#pragma once

class Solver;

class Constraint
{
	friend class Solver;
public:
	Constraint(): parent(0)
	{}
	
	virtual ~Constraint()
	{}
	
	virtual void iterate() = 0;
	
	virtual double getDifference()
	{
		return 0.0;
	}

protected:
	Solver* parent;
private:
};
