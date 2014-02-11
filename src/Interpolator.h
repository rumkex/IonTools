#pragma once

template <int dim>
class Interpolator
{	
public:	
	virtual SparseVector interpolatePoint(const Vector<dim>&) const = 0;
protected:
private:	
};