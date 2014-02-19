#include <vector>
#include <memory>
#include <functional>
#include <fstream>
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
	
public:
	MGSolver(const Configuration& config, const Grid& baseGrid, const vector<Ray<2>>& data,
		const function<double(const Vector<2>&)>& baseModel = [](const Vector<2>&){return 0.0;}): config(config)
	{
		int resX = baseGrid.getNodeCount(0);
		int resY = baseGrid.getNodeCount(1);
		for (int layer = 0; layer < config.Layers; layer++)
		{
			shared_ptr<Grid> currentGrid(new Grid(baseGrid));
			currentGrid->setNodeCount(vector2(resX, resY));
			grids.push_back(currentGrid);
			
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
			if (layer == config.Layers-1) 
				currentSolver->addConstraint(new PositiveConstraint());		
			
			if (config.UseWeights) 
			{
				int nmax = currentGrid->getNodeCount(0);
				double e1 = 15. * nmax / 128.;
				double e2 = 20. * nmax / 128.;
				double e3 = 60. * nmax / 128.;
				double e4 = 80. * nmax / 128.;
				auto weightFunc = [e1, e2, e3, e4](double nx, double)
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
			 
			if (layer == config.Layers-1)
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
		// Coarse >> fine
		for (auto it = solvers.rbegin(); it != solvers.rend(); it++)
		{
			auto solver = *it;
			//Correct the error from previous layer, if present
			if (it != solvers.rbegin())
			{
				// Compute the error on previous layer
				auto prevSolver = *(it-1);
				LinearConstraint* constraint = (LinearConstraint*)prevSolver->getConstraint(0);
				auto A = constraint->getMatrix();
				auto b = constraint->getRightSide();
				auto err = A.transform(prevSolver->getResult());
				for (int k = 0; k < (int)b.size(); k++)
				{
					err[k] = b[k] - err[k];
				}

				// And solve it instead in current layer
				((LinearConstraint*)solver->getConstraint(0))->setRightSide(err);
			}

			// Calculate the correction
			for (int iter = 0; iter < config.MaxIter; iter++) solver->iterate();
		}
	}

	vector<vector<double>> getResult()
	{
		return getResult(-1);
	}

	vector<vector<double>> getResult(int layer)
	{
		vector<vector<double>> result(grids.front()->getNodeCount(0), vector<double>(grids.front()->getNodeCount(1)));
		for (int i = 0; i < (int)grids.front()->getNodeCount(0); i++)
		{
			for (int j = 0; j < (int)grids.front()->getNodeCount(1); j++)
			{
				auto v = grids.front()->getNodeCoords(vector2(i, j));
				double value = 0.0;
				if (layer == -1)
					for (int l = 0; l < (int)solvers.size(); l++)
						value += grids[l]->getValueAtLocal(v, solvers[l]->getResult());
				else
					value = grids[layer]->getValueAtLocal(v, solvers[layer]->getResult());
				result[i][j] = value;
			}
		}
		return result;
	}
};
