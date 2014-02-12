#pragma once
#include <vector>
#include "SparseVector.h"

using namespace std;

class SparseMatrix
{
private:
	vector<SparseVector> rows;

public:
	SparseMatrix() {}
	SparseMatrix(const vector<SparseVector> &rows): rows(rows)
	{}

	const SparseVector& operator[](int index) const
	{
		return rows[index];
	}

	SparseVector& operator[](int index)
	{
		return rows[index];
	}

	vector<double> transform(const vector<double>& src) const
	{
		vector<double> result(rows.size());
		for(int i = 0; i < (int)rows.size(); i++)
		{
			result[i] = rows[i].dot(src);
		}
		return result;
	}

	int size() const
	{
		return rows.size();
	}
};
