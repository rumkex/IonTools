#pragma once
#include <cmath>
#include "RegularGrid.h"
#include "Interpolator.h"

template <int dim>
class LinearInterpolator: public Interpolator<dim>
{
	typedef Vector<dim> Point;
	typedef Vector<dim> WorldPoint;
	typedef Vector<dim, int> Node;

public:
	LinearInterpolator(RegularGrid<dim>* parent): parent(parent)
	{}

	virtual shared_ptr<Interpolator<dim>> clone(RegularGrid<dim>* parent) const
	{
		return shared_ptr<Interpolator<dim>>(new LinearInterpolator<dim>(parent));
	}
	
	virtual SparseVector interpolatePoint(const Point &v) const
	{
		Node seed;
		return interpolateDimension(v, seed, 0);
	}
	
protected:
	RegularGrid<dim>* parent;

	virtual double interpolationFunction(const Vector<dim> &x0, const Vector<dim> &x) const
	{
		double result = 1.0;
		for (int i = 0; i < (int)dim; i++)
		{
			result *= LinearSpline(x0[i], x[i], i);
		}
		return result;
	}
	
private:
	SparseVector interpolateDimension(const Point &v, Node &id, int d) const
	{
		SparseVector result;
		if (d < dim)
		{
			Point origin = parent->getOrigin();
			Node nodes = parent->getNodeCount();
			double x = (v[d] - origin[d]) / parent->getDelta(d);
			int start = clamp(int(floor(x)), 0, nodes[d] - 1);
			int end = clamp(int(ceil(x)), 0, nodes[d] - 1);
			for (int n = start; n <= end; n++)
			{
				id[d] = n;
				result += interpolateDimension(v, id, d+1);
			}
		}
		else
		{
			Point xn = parent->getNodeCoords(id);
			int n = parent->getNodeID(id);
			double weight = interpolationFunction(xn, v);
			if (weight != 0.0)
				result[n] = weight;
		}
		return result;
	}	
	
	int clamp(int val, int min, int max) const
	{
        return (val < min) ? min: ((val > max) ? max: val);
	}

	double LinearSpline(double x0, double x, int d) const
	{
		double delta = parent->getDelta(d);
		double dx = std::abs(x - x0);
		if (dx < delta)
			return 1 - dx / delta;
		else
			return 0.0;
	}
};

