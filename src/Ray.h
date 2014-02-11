#pragma once
#include "Vector.h"

template<int dim>
struct Ray
{
	Vector<dim> start, end;
	double time, value;

	Ray()
	{
	}
	
	Ray(Vector<dim> start, Vector<dim> end, double time, double value): start(start), end(end), time(time), value(value)
	{
	}
};
