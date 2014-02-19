#include "SolverApp.h"

#include "Vector.h"
#include "SparseVector.h"
#include "LinearInterpolator.h"
#include "CubicInterpolator.h"
#include "SIRTSolver.h"
#include "ARTSolver.h"
#include "SARTSolver.h"
#include "MGSolver.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <cmath>


SolverApp::SolverApp(Configuration config): config(config)
{

}

void SolverApp::run()
{
	auto transform = setTransform();

	vector<Ray<2>> data;
	loadData(config.InFilename, data);
	cout << "Loaded " << data.size() << " rays." << endl;

	Vector<2> origin, size;
	detectRegion(data, origin, size, transform);
	cout << "Determined origin: (" << origin[0] << ", " << origin[1] << ")" << endl;
	cout << "Determined size: (" << size[0] << ", " << size[1] << ")" << endl;

	RegularGrid<2> baseGrid(origin, size, vector2(config.NodeCountX, config.NodeCountY));
	if (config.Interpolation == InterpolationMode::Cubic)
		baseGrid.setInterpolator<CubicInterpolator<2>>();
	else
		baseGrid.setInterpolator<LinearInterpolator<2>>();
	baseGrid.setTransform(transform);
	const double r0 = 6800.0;
	const double w = 100.0;
	auto baseModel = [&] (const Vector<2>& v)
	{
		auto r = v[0];
		return exp(- (r-r0)*(r-r0) / (2.0 * w * w));
	};
	cout << "Using solver: " << config.SolverType << endl;
	if (config.SolverType == "ART")
		compute<ARTSolver>(baseGrid, data, baseModel);
	else if (config.SolverType == "SIRT")
		compute<SIRTSolver>(baseGrid, data, baseModel);
	else if (config.SolverType == "SART")
		compute<SARTSolver>(baseGrid, data, baseModel);
	else
		throw std::runtime_error("Unknown solver type: " + config.SolverType);
}

void SolverApp::loadData(const string& filename, vector<Ray<2>>& out)
{
	fstream filestr(filename);
	int nrays;
	string format;
	filestr >> format >> nrays;
	for (int i = 0; i < (int)nrays; i++)
	{
		Ray<2> ray;
		filestr >> ray.start[0] >> ray.start[1] >>
		        ray.end[0] >> ray.end[1] >> ray.value;
		out.push_back(ray);
	}
	filestr.close();
}

template <class TSolver>
void SolverApp::compute(const RegularGrid<2>& baseGrid, const vector<Ray<2>>& data, function<double(const Vector<2>&)> baseModel)
{
	MGSolver<TSolver> solver(config, baseGrid, data, baseModel);
	clock_t start = clock();

	solver.solve();

	cout << "Finished in " << (float)(clock() - start) / CLOCKS_PER_SEC << "s" << endl;
	
	for (int layer = 0; layer < config.Layers; layer++ )
		writeResults("debug-" + to_string(layer) + ".txt", solver.getResult(layer), baseGrid);
	
	writeResults(config.OutFilename, solver.getResult(), baseGrid);
}

void SolverApp::writeResults(string filename, const vector<vector<double>>& result, const RegularGrid<2>& grid)
{
	fstream filestr(filename, fstream::out);
	
	filestr << "# " << grid.getOrigin()[0] << '\t' << grid.getSize()[0] << '\t' 
					 << grid.getOrigin()[1] << '\t' << grid.getSize()[1] << endl;
	
	for (int i = 0; i < (int)result.size(); i++)
	{
		for (int j = 0; j < (int)result[i].size(); j++)
		{
			if (j != 0) filestr << '\t';
			filestr << result[i][j];
		}
		filestr << endl;
	}
	filestr.flush();
	filestr.close();
}

void SolverApp::detectRegion(const vector<Ray<2>>& data, Vector<2>& origin, Vector<2>& size, function<Vector<2>(const Vector<2>&)>& transform)
{
	auto min = transform(data[0].start);
	auto max = transform(data[0].start);
	for (int i = 0; i < (int)data.size(); i++)
	{
		Vector<2> v1 = transform(data[i].start);
		min[0] = std::min(v1[0], min[0]);
		min[1] = std::min(v1[1], min[1]);
		max[0] = std::max(v1[0], max[0]);
		max[1] = std::max(v1[1], max[1]);
		Vector<2> v2 = transform(data[i].end);
		min[0] = std::min(v2[0], min[0]);
		min[1] = std::min(v2[1], min[1]);
		max[0] = std::max(v2[0], max[0]);
		max[1] = std::max(v2[1], max[1]);
	}
	origin = min;
	size = max - min;
}

function<Vector<2>(const Vector<2>&)> SolverApp::setTransform()
{
	unordered_map<string, function<Vector<2>(const Vector<2>&)> > transforms;
	transforms["polar"] = [](const Vector<2>& v)
	{
		return vector2(sqrt(v[0]*v[0] + v[1]*v[1]), atan2(v[1], v[0]));
	};
	transforms["cartesian"] = [](const Vector<2>& v)
	{
		return v;
	};
	if (transforms.count(config.TransformType) == 0) throw std::runtime_error("Unknown transform type: " + config.TransformType);
	return transforms[config.TransformType];
}
