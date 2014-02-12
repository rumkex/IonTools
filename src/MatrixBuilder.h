#pragma once
#include "RegularGrid.h"
#include "SparseMatrix.h"
#include "SparseVector.h"
#include "LinearConstraint.h"
#include "Ray.h"
#include "integrate.h"

#include <vector>
#include <iostream>
#include <memory>

using namespace std;

template<int dim>
class MatrixBuilder
{
public:
	enum BuilderMode
	{
		Differential,
		Absolute
	};	
private:
	BuilderMode mode;

    std::vector<SparseVector> rows;
    std::vector<double> values;
    shared_ptr<RegularGrid<dim>> interpolator;

public:

    MatrixBuilder(const shared_ptr<RegularGrid<dim>> &interp, BuilderMode mode = BuilderMode::Differential): mode(mode), interpolator(interp)
    {}

    void addRay(const Ray<dim> &r)
    {
        std::function<SparseVector(const Vector<dim>&)> func = [this](const Vector<dim> &v) { return interpolator->getPointWeights(v); };
        SparseVector row;
        integrate(row, func, r.start, r.end, interpolator->getDelta(0));
        addRow(row, r.value);
    }

    void addRow(const SparseVector& row, const double value)
    {
        rows.push_back(row);
        values.push_back(value);
    }

    Constraint* getConstraint()
    {
		if (mode == BuilderMode::Differential)
		{
			for (int i = 0; i < (int)rows.size() - 1; i++)
			{
				rows[i] -= rows[i+1];
				values[i] -= values[i+1];
			}
			rows.pop_back();
			values.pop_back();
		}
        return new LinearConstraint(SparseMatrix(rows), values);
    }

};
