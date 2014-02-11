#pragma once

#include <cmath>
#include <iostream>

template<int dim, class T = double>
struct Vector
{
	T coords[dim] = {T(0.0)};

	Vector(T* coords)
	{
		for(int i = 0; i < dim; i++)
			this->coords[i] = coords[i];
	}

	Vector(void)
	{}

	~Vector(void)
	{}

	Vector<dim, T> operator* (const T &other) const
	{
		Vector<dim, T> c;
		for(int i = 0; i < dim; i++)
			c[i] = this->coords[i] * other;
		return c;
	}

	Vector<dim, T> operator/ (const T &other) const
	{
		return (*this) * ((T)1 / other);
	}

	Vector<dim, T> operator+ (const Vector<dim, T> &other) const
	{
		Vector<dim, T> c;
		for(int i = 0; i < dim; i++)
			c[i] = this->coords[i] + other[i];
		return c;
	}

	Vector<dim, T> operator- (const Vector<dim, T> &other) const
	{
		Vector<dim, T> c;
		for(int i = 0; i < dim; i++)
			c[i] = this->coords[i] - other[i];
		return c;
	}

	T& operator[](int d)
	{
		return coords[d];
	}
	T operator[](int d) const
	{
		return coords[d];
	}

	T getLengthSquared()
	{
		T l = (T)0;
		for (int i = 0; i < dim; i++)
		{
			l += coords[i] * coords[i];
		}
		return l;
	}

	T getLength()
	{
		return sqrt(getLengthSquared());
	}
};

template <int dim, class T>
std::ostream& operator<< (std::ostream& out, Vector<dim, T> &v)
{
	out << "(";
	for (int i = 0; i < dim; i++)
		out << v[i] << (i < dim - 1 ? ", ": "");
	out << ")";
	return out;
}

template <class T>
Vector<2, T> vector2(T x, T y)
{
	T c[2] = {x, y};
	return Vector<2, T>(c);
}

template <class T>
Vector<3, T> vector3(T x, T y, T z)
{
	T c[3] = {x, y, z};
	return Vector<3, T>(c);
}
