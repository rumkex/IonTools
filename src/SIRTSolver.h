#pragma once
#include <vector>
#include "Solver.h"

class SIRTSolver: public Solver
{
public:
	SIRTSolver(int nodeCount): Solver(nodeCount)
	{}

	virtual void solve(LinearConstraint* constraint)
	{
		const vector<double>& b = constraint->getRightSide();
		const SparseMatrix& A = constraint->getMatrix();
		double norm = 0.0;
		for (int i = 0; i < (int)A.size(); i++)
			norm += constraint->getNorm(i);
		//const SparseVector& columnCache = constraint->getColumnCache();
		for (int i = 0; i < (int)(int)b.size(); i++)
		{
			double C = (b[i] - A[i].dot(x)) / norm;
			for(auto it = A[i].cbegin(); it != A[i].cend(); it++)
			{
				dx[(*it).first] += (*it).second * C;
			}
		}
		
		vector<double> db = A.transform(dx);
		double lambda = 0.0;
		double dbnorm = 0.0;
		for (int i = 0; i < (int)b.size(); i++)
			dbnorm += db[i]*db[i];
		for (int i = 0; i < (int)b.size(); i++)
			lambda += db[i] * (b[i] - A[i].dot(x)) / dbnorm;
			
		for (int i = 0; i < (int)(int)x.size(); i++)
		{
			x[i] += lambda * weights[i] * dx[i];
			dx[i] = 0.0;
		}
	}

	virtual void solve(PositiveConstraint* constraint)
	{
		for (int i = 0; i < (int)x.size(); i++)
			if (x[i] < 0.0) x[i] = 0.0;
	}
};
