#pragma once
#include <memory>

template <int dim>
class RegularGrid;

template <int dim>
class Interpolator
{	
public:	
	virtual SparseVector interpolatePoint(const Vector<dim>&) const = 0;
	virtual shared_ptr<Interpolator<dim>> clone(RegularGrid<dim>* parent) const = 0;
	virtual ~Interpolator()
	{}
protected:
private:	
};