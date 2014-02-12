#pragma once

#include <cmath>
#include <vector>
#include "SparseVector.h"
#include "SparseMatrix.h"
#include "Constraint.h"

class LinearConstraint: public Constraint
{
public:
    LinearConstraint(const SparseMatrix& A, const vector<double>& b): A(A), b(b)
    {
        generateCache();		
    }

    virtual void iterate();
	
	virtual double getDifference();

    const SparseMatrix& getMatrix()
    {
        return A;
    }

    const vector<double>& getRightSide()
    {
        return b;
    }

    const vector<double>& getRowCache()
    {
        return rowCache;
    }

    const SparseVector& getColumnCache()
    {
        return columnCache;
    }

    double getNorm(int index)
    {
        return normCache[index];
    }

protected:
    SparseMatrix A;
    vector<double> b;

    LinearConstraint() {}

    void generateCache()
    {
        columnCache.clear();
        rowCache.clear();
        rowCache.resize(b.size());
        normCache.clear();
        normCache.resize(b.size());
        for (int i = 0; i < (int)b.size(); i++)
        {
            rowCache[i] += A[i].sum();
            normCache[i] = A[i].lengthSquared();
            columnCache += A[i];
        }
		constraintNorm = 0.0;
		for (int i = 0; i < (int)b.size(); i++)
			constraintNorm += std::abs(b[i]);
    }

private:
    vector<double> rowCache, normCache;
    SparseVector columnCache;
	double constraintNorm;
};
