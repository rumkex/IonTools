#pragma once
#include <cmath>
#include "LinearInterpolator.h"

template <int dim>
class CubicInterpolator: public LinearInterpolator<dim>
{
	typedef Vector<dim> Point;
	typedef Vector<dim> WorldPoint;
	typedef Vector<dim, int> Node;
	
public:
	CubicInterpolator(RegularGrid<dim>* parent): LinearInterpolator<dim>(parent)
	{}

	virtual shared_ptr<Interpolator<dim>> clone(RegularGrid<dim>* parent) const
	{
		return shared_ptr<Interpolator<dim>>(new CubicInterpolator<dim>(parent));
	}

protected:
	virtual double interpolationFunction(const Vector<dim> &x0, const Vector<dim> &x) const
	{
		double result = 1.0;
		for (int i = 0; i < (int)dim; i++)
		{
			result *= CubicSpline(x0[i], x[i], i);
		}
		return result;
	}

	virtual double getRadius() const
	{
		return 1;
	}

private:
	double CubicSpline(double x0, double x, int d) const
	{
		double delta = this->parent->getDelta(d);
		double dx = std::abs(x - x0);
		if (dx < delta)
		{
			dx = 1 - dx/delta;
			return -0.75 * dx*dx*dx + 0.75 * dx*dx + 0.75 * dx + 0.25;
		}
		else if (dx < 2*delta)
		{
			dx = 2 - dx/delta;
			return 0.25 * dx*dx*dx;
		}
		else
			return 0.0;
	}
};