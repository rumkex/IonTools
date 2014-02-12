#include <vector>
#include <memory>
#include <functional>
#include <fstream>
#ifdef DEBUG
	#include <sstream>
#endif
#include <assert.h>

#include "RegularGrid.h"
#include "LinearConstraint.h"
#include "LaplaceConstraint.h"
#include "PositiveConstraint.h"

using namespace std;

template <class TSolver>
class MGSolver
{	
	typedef RegularGrid<2> Grid;
	
private:
	const Configuration& config;
	vector<shared_ptr<Grid>> grids;
	vector<shared_ptr<TSolver>> solvers;
	
#ifdef DEBUG
	void writeResults(int layer, string filename)
	{
		fstream filestr(filename, fstream::out);
		for (int i = 0; i < (int)grids[layer]->getNodeCount(0); i++)
		{
			for (int j = 0; j < grids[layer]->getNodeCount(1); j++)
			{
				auto v = vector2(i, j);
				filestr << grids[layer]->getValueAtNode(v, solvers[layer]->getResult());
				if (j != grids[layer]->getNodeCount(1)-1) filestr << '\t';
			}
			filestr << endl;
		}
		filestr.flush();
		filestr.close();
	}
#endif
	
public:
	MGSolver(const Configuration& config, const Grid& baseGrid, const vector<Ray<2>>& data,
		const function<double(const Vector<2>&)>& baseModel = [](const Vector<2>&){return 0.0;}): config(config)
	{
		int resX = baseGrid.getNodeCount(0);
		int resY = baseGrid.getNodeCount(1);
		for (int layer = 0; layer < config.Layers; layer++)
		{
			shared_ptr<Grid> currentGrid(new Grid(baseGrid.getOrigin(), baseGrid.getSize(), vector2(resX, resY)));
			grids.push_back(currentGrid);
			currentGrid->setTransform(baseGrid.getTransform());
			
			shared_ptr<TSolver> currentSolver(new TSolver(currentGrid->getTotalNodeCount()));
			solvers.push_back(currentSolver);
			
			MatrixBuilder<2> mb(currentGrid);	
			for (int i = 0; i < (int)data.size(); i++)
			{
				mb.addRay(data[i]);
			}
			
			currentSolver->addConstraint(mb.getConstraint());
			if (config.UseAA) 
				currentSolver->addConstraint(new LaplaceConstraint(*currentGrid));
			currentSolver->addConstraint(new PositiveConstraint());		
			
			if (config.UseWeights) 
			{
				int nmax = currentGrid->getNodeCount(0);
				double e1 = 15. * nmax / 128.;
				double e2 = 20. * nmax / 128.;
				double e3 = 60. * nmax / 128.;
				double e4 = 80. * nmax / 128.;
				auto weightFunc = [e1, e2, e3, e4](double nx, double ny)
				{
					if (nx < e1 || nx > e4) return 0.0;
					if (nx > e2 && nx < e3) return 1.0;
					double t = 0.0;
					if (nx >= e1 && nx <= e2) 
						t = (nx - e1) / (e2 - e1);
					if (nx >= e3 && nx <= e4)
						t = (e4 - nx) / (e4 - e3);
					return 3*t*t - 2*t*t*t;
				};
				vector<double> weights(currentGrid->getTotalNodeCount());
				currentGrid->forEachNode([weightFunc, &weights, currentGrid](const Vector<2, int>& v)
				{ 
					weights[currentGrid->getNodeID(v)] = weightFunc(v[0], v[1]); 
				});				
				currentSolver->setWeights(weights);
			}
			 
			currentGrid->forEachNode([baseModel, currentSolver, currentGrid](const Vector<2, int>& n)
			{
				auto v = currentGrid->getNodeCoords(n);
				currentSolver->getResult()[currentGrid->getNodeID(n)] = baseModel(v);
			});
			
			resX /= 2;
			resY /= 2;
		}
	}
	
	void solve()
	{		  
		for (int i = solvers.size()-1; i >= 0; i--)
		{			
#ifdef DEBUG		
			writeResults(i, "debug-" + std::to_string(i) + "-presolve.txt");
#endif
			int iter = 0;
			double delta = numeric_limits<double>::max();
			double newdelta = delta;    

			while ((iter < config.MaxIter))
			{
				solvers[i]->iterate();
				delta = newdelta;
				newdelta = solvers[i]->getConstraint(0)->getDifference();
				iter++;
#ifndef DEBUG	
				if (iter % 100 == 0)
#endif
					cout << "Iteration " << iter << ": " << newdelta << endl;
			}
			cout << "Layer " << grids[i]->getNodeCount(0) << "x" << grids[i]->getNodeCount(1) << " took " << iter << " iterations" << endl;
			if (i > 0) cherrypick(i, i-1);
				
#ifdef DEBUG		
			writeResults(i, "debug-" + std::to_string(i) + "-postsolve.txt");
#endif
		}
	}
	
	shared_ptr<TSolver> getTopSolver()
	{
		return solvers.front();
	}
	
	shared_ptr<Grid> getTopGrid()
	{
		return grids.front();
	}
	
private:
	void cherrypick(int layerFrom, int layerTo) 
	{
		auto srcSolver = solvers[layerFrom];
		auto srcGrid = grids[layerFrom];
		auto dstSolver = solvers[layerTo];
		auto dstGrid = grids[layerTo];
		
		int counter = 0;
		
		std::function<void(const Vector<2, int>&)> interpFunc = [srcGrid, dstGrid, srcSolver, dstSolver, &counter](const Vector<2, int>& n)
		{
			counter++;
			Vector<2> v = dstGrid->getNodeCoords(n);
			int id = dstGrid->getNodeID(n);
			double val = srcGrid->getValueAtLocal(v, srcSolver->getResult());
			dstSolver->getResult()[id] = val;
		};
		
		dstGrid->forEachNode(interpFunc);
		cout << endl;
		cout << "Interpolated to " << counter << " nodes." << endl;
	}
};
