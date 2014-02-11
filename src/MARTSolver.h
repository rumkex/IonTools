#pragma once
#include "Solver.h"

class MARTSolver: public Solver
{
    public:
    MARTSolver(): iterations(16) {}
    MARTSolver(int iter): iterations(iter) {}
    virtual ~MARTSolver() {}

    virtual void Solve(const SparseMatrix& A, const vector<double>& b, vector<double>& x)
    {
        for (int i = 0; i < x.size(); i++)
            if (x[i] <= 0.0) x[i] = 1.0;
        for (int i = 0; i < iterations; i++) Iterate(A, b, x);
    }

    int getIterations()
    {
        return iterations;
    }

private:
    int iterations;
    vector<double> normCache;

    void Iterate(const SparseMatrix& A, const vector<double>& b, vector<double>& x)
    {
        double lambda = 1.0;
        for (int i = 0; i < (int)b.size(); i++)
        {
            double mult = b[i] / A[i].dot(x);
            for (auto it = A[i].cbegin(); it != A[i].cend(); it++)
            {
                x[(*it).first] *= pow(mult, lambda * (*it).second);
            }
        }
    }
};
