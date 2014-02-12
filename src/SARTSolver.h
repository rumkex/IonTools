#pragma once
#include <vector>
#include "Solver.h"

class SARTSolver: public Solver
{
public:
	SARTSolver(int nodeCount): Solver(nodeCount), dx(nodeCount)
	{}

	virtual void solve(LinearConstraint* constraint) {
		const vector<double>& b = constraint->getRightSide();
		const SparseMatrix& A = constraint->getMatrix();
		const vector<double>& rowCache = constraint->getRowCache();
		const SparseVector& columnCache = constraint->getColumnCache();
		for (int i = 0; i < (int)(int)b.size(); i++) {
			double C = (b[i] - A[i].dot(x)) / rowCache[i];
			for(auto it = A[i].cbegin(); it != A[i].cend(); it++) {
				dx[(*it).first] += (*it).second * C;
			}
		}
		for (int i = 0; i < (int)(int)x.size(); i++) {
			x[i] += weights[i] * dx[i] / columnCache[i];
			dx[i] = 0.0;
		}
	}

	virtual void solve(PositiveConstraint* constraint) {
		for (int i = 0; i < (int)x.size(); i++)
			if (x[i] < 0.0) x[i] = 0.0;
	}

protected:
	vector<double> dx;
};
