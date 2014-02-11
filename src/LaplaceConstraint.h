#pragma once

#include <vector>

#include "RegularGrid.h"
#include "LinearConstraint.h"

using namespace std;

class LaplaceConstraint: public LinearConstraint
{
public:
	template<int dim>
	LaplaceConstraint(const RegularGrid<dim> &grid)
    {
		typedef Vector<dim, int> Node;
		
		vector<SparseVector> rows;
				
		grid.forEachNode([this, &rows, grid](const Node& node) 
			{	
				SparseVector row;
				auto incrementLambda = 
				[&row, grid](int d, const Node& node)
				{
					if (node[d] < grid.getNodeCount(d) &&
						node[d] >= 0)
					{							
						row[grid.getNodeID(node)] = 1.0;
						return 1.0;
					}
					return 0.0;
				};
				double total = 0.0;
				for (int d = 0; d < dim; d++)
				{
					auto temp(node);
					temp[d] += 1;
					total += incrementLambda(d, temp);
					temp[d] -= 2;
					total += incrementLambda(d, temp);
				}
				row[grid.getNodeID(node)] = -total;
				rows.push_back(row);
				b.push_back(0.0);
			}
			);
			
        A = SparseMatrix(rows);
		generateCache();
	}
};
