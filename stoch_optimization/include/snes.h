#ifndef SNES_H
#define SNES_H

#include "StochOptimization.h"
#include "xnes_math.h"
#include <vector>
#include <algorithm>
#include <random>
#include <ctime>

namespace stochopt {

	struct SNESBorders : public Borders {
		SNESBorders(int nParam_, int min_, int max_);
		SNESBorders(const std::vector<std::pair<double, double>>& ranges_);
		std::vector<double> xmin;
		std::vector<double> xmax;
		std::vector<std::pair<double, double>> ranges;
	};

	//class CostObject {
	//public:
	//	virtual double getCost(std::vector<double> &vect) = 0;
	//};

	class SNES : public Algorithm {
	public:
		SNES(int nDim, int nCalls, CostObject* costobj, 
			SNESBorders* bo, int nPart, double mean_lr, double sigma_lr);
		SNES(int nDim, int nCalls, CostObject* costobj, 
			double accur, SNESBorders* bo, int nPart, double mean_lr, double sigma_lr);
		void solve();

		//void SetStopCriterion(StopCriterion c) { criterion = c; }
		//void SetTolerance(double tol) { tolerance = tol; }
		std::vector<double> getSol() const { return xBestSolution; }
		//double getCost() const { return costBest; }
		void printGraph();
		//std::vector<int> callsGraph;
		//std::vector<std::vector<double>> getxBestOnEachSol() const { return xBestOnEachStep; }
		//std::vector<double> getBestSolOnEachStep() const { return BestSolOnEachStep; }

	private:
		//int nParticles, nDimensions, nMaxCalls = 300;
		//costBest = -std::numeric_limits<double>::infinity();
		//int calls = 0;
		
		//CostObject* costObject;
		SNESBorders* borders;
		//std::vector<double> xBestSolution;
		//std::vector<double> costGraph;
		//std::vector<std::vector<double>> xBestOnEachStep;
		//std::vector<double> BestSolOnEachStep;
		double learn_rate_mean;
		double learn_rate_sigma;
		//double tolerance = 1.e-3;
		

		Matrix curr_samples;
		Matrix curr_members;
		Matrix S;
		Matrix U;
		Matrix mean_vec; // вектор
		Matrix var_vec; // вектор
		Matrix mean_grad; // вектор
		Matrix var_grad; // вектор

	private:
		
		void DisplayInRealBords(std::vector<double>& old_point);
	};

	std::vector<int> getOrder(const std::vector<double>& v_);
}

#endif // SNES_H