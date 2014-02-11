#pragma once
#include <unordered_map>
#include <ostream>
#include <vector>

using namespace std;

class SparseVector
{
private:
	unordered_map<int, double> values;
public:

	SparseVector(void)
	{}

	SparseVector(unordered_map<int, double> val): values(val)
	{}

	unordered_map<int, double>::iterator begin()
	{
		return values.begin();
	}

	unordered_map<int, double>::iterator end()
	{
		return values.end();
	}

	unordered_map<int, double>::const_iterator cbegin() const
	{
		return values.cbegin();
	}

	unordered_map<int, double>::const_iterator cend() const
	{
		return values.cend();
	}

	int size()
	{
		return values.size();
	}

	double& operator[](int key)
	{
		return values[key];
	}
	double operator[](int key) const
	{
		return values.count(key) ? values.at(key): 0;
	}


	const SparseVector& operator*= (double other)
	{
		for(auto it = values.begin(); it != values.end(); it++)
			values[(*it).first] = other * (*it).second;
		return *this;
	}

	const SparseVector& operator+= (const SparseVector& other)
	{
		for(auto it = other.values.begin(); it != other.values.end(); it++)
			values[(*it).first] += (*it).second;
		return *this;
	}

	const SparseVector& operator-= (const SparseVector& other)
	{
		for(auto it = other.values.begin(); it != other.values.end(); it++)
			values[(*it).first] -= (*it).second;
		return *this;
	}

	SparseVector operator* (double other) const
	{
		SparseVector result(values);
		return result *= other;
	}

	SparseVector operator+ (const SparseVector& other) const
	{
		SparseVector result(values);
		return (result += other);
	}

	SparseVector operator- (const SparseVector& other) const
	{
		SparseVector result(values);
		return (result -= other);
	}

	template <class TVector>
	double dot(const TVector& other) const
	{
		double sum = 0.0;
		for(auto it = values.begin(); it != this->values.end(); it++)
		{
			int key = (*it).first;
			sum += other[key] * (*it).second;
		}
		return sum;
	}

	double sum() const
	{
		double sum = 0.0;
		for(auto it = values.begin(); it != this->values.end(); it++)
			sum += (*it).second;
		return sum;
	}

	void clear()
	{
		values.clear();
	}

	double lengthSquared() const
	{
		double sum = 0.0;
		for(auto it = values.begin(); it != this->values.end(); it++)
			sum += (*it).second * (*it).second;
		return sum;
	}

	~SparseVector(void)
	{
	}
};

ostream& operator<<(ostream& out, SparseVector &v);
