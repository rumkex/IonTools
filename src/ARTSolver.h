#pragma once
#include "Solver.h"

class ARTSolver: public Solver
{
public:
	ARTSolver(int nodeCount): Solver(nodeCount)
	{}

	virtual void solve(LinearConstraint* constraint)
	{
		const vector<double>& b = constraint->getRightSide();
		const SparseMatrix& A = constraint->getMatrix();
		for (int i = 0; i < (int)b.size(); i++)
		{
			double C = (b[i] - A[i].dot(x)) / constraint->getNorm(i);
			for (auto it = A[i].cbegin(); it != A[i].cend(); it++)
				x[(*it).first] += weights[(*it).first] * C * (*it).second;
		}
	}

	virtual void solve(PositiveConstraint* constraint)
	{
		for (int i = 0; i < (int)x.size(); i++)
			if (x[i] < 0.0) x[i] = 0.0;
	}
};
