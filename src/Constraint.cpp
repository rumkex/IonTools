#include "Constraint.h"
#include "LinearConstraint.h"
#include "Solver.h"

#include <cmath>
#include <iostream>

using namespace std;

double LinearConstraint::getDifference()
{	
	double delta = 0.0;
	auto diff = A.transform(parent->getResult());
	for (int i = 0; i < (int)diff.size(); i++)
	{
		delta += std::abs(b[i] - diff[i]) / constraintNorm;
	}
	return delta;
}

void LinearConstraint::iterate()
{
    parent->solve(this);
}

void PositiveConstraint::iterate()
{
    parent->solve(this);
}
