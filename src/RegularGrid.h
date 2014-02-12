#pragma once
#include <vector>
#include <functional>
#include "Vector.h"
#include "SparseVector.h"

template <int dim>
class LinearInterpolator;

template<int dim, class TInterp = LinearInterpolator<dim>>
class RegularGrid
{
	typedef Vector<dim> Point;
	typedef Vector<dim> WorldPoint;
	typedef Vector<dim, int> Node;

protected:
	// Grid parameters
	Point origin;
	Point size;
	Node nodes;

	std::function<Point(const WorldPoint&)> transform;

	TInterp interpolator;

public:
	RegularGrid(Point origin, Point size, Node nodes): origin(origin), size(size), nodes(nodes), interpolator(TInterp(this))
	{
		transform = [](const WorldPoint& v)
		{
			return v;
		};
	}

	void setTransform(std::function<Point(const WorldPoint&)> transform)
	{
		this->transform = transform;
	}

	bool isInside(const WorldPoint &v) const
	{
		Point vec = transform(v);
		for (int d = 0; d < dim; d++)
		{
			if (vec[d] < origin[d] || vec[d] > origin[d] + size[d]) return false;
		}
		return true;
	}

	void forEachNode(const std::function<void(const Node&)>& func) const
	{
		Node seed;
		computeDimension(func, seed, 0);
	}

	SparseVector getLocalPointWeights(const Point &v) const
	{
		return interpolator.interpolatePoint(v);
	}

	SparseVector getPointWeights(const WorldPoint &v) const
	{
		return getLocalPointWeights(transform(v));
	}

	double getValueAt(const WorldPoint &v, const std::vector<double>& values) const
	{
		return getPointWeights(v).dot(values);
	}

	double getValueAtLocal(const Point &v, const std::vector<double>& values) const
	{
		return getLocalPointWeights(v).dot(values);
	}

	double getValueAtNode(const Node &n, const std::vector<double>& values) const
	{
		return values[getNodeID(n)];
	}

	Point getNodeCoords(const Node &n) const
	{
		Point result;
		for (int i = 0; i < (int)dim; i++)
			result[i] = origin[i] + (double) n[i] * getDelta(i);
		return result;
	}

	int getNodeID(const Node &n) const
	{
		int mult = 1;
		int result = 0;
		for (int i = 0; i < (int)dim; i++)
		{
			result += mult * n[i];
			mult *= nodes[i];
		}
		return result;
	}

	double getDelta(int d) const
	{
		return size[d] / double(nodes[d] - 1);
	}

	std::function<Point(const WorldPoint&)> getTransform() const
	{
		return transform;
	}

	const Point& getOrigin() const
	{
		return origin;
	}

	const Point& getSize() const
	{
		return size;
	}

	int getNodeCount(int d) const
	{
		return nodes[d];
	}

	const Node& getNodeCount() const
	{
		return nodes;
	}

	long getTotalNodeCount() const
	{
		long total = 1;
		for (int i = 0; i < (int)dim; i++)
			total *= nodes[i];
		return total;
	}

private:
	void computeDimension(const std::function<void(const Node&)> &func, Node &id, int d) const
	{
		if (d < dim)
		{
			for (int i = 0; i < (int)nodes[d]; i++)
			{
				id[d] = i;
				computeDimension(func, id, d+1);
			}
		}
		else
			func(id);
	}
};
