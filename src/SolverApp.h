#pragma once

#include <string>
#include <vector>
#include <functional>
#include <limits>

#include "Configuration.h"
#include "MatrixBuilder.h"
#include "RegularGrid.h"
#include "Solver.h"
#include "Ray.h"

using namespace std;

class SolverApp
{
private:
	Configuration config;
	
public:
	SolverApp(Configuration);
	
	void run();
	
private:
	void loadData(const string& filename, vector<Ray<2>>& out);
	function<Vector<2>(const Vector<2>&)> setTransform();
	void detectRegion(const vector<Ray<2>>& data, Vector<2>& origin, Vector<2>& size, function<Vector<2>(const Vector<2>&)>& transform);
	template <class TSolver> void compute(const RegularGrid<2>& baseGrid, const vector<Ray<2>>& data, function<double(const Vector<2>&)> baseModel);
	void writeResults(string filename, const vector<vector<double>>& result, const RegularGrid<2>& grid);
};
