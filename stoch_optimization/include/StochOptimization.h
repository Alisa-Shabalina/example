#pragma once

#include <vector>
#include <algorithm>
#include <numeric>
#include "math.h"
#include "time.h"
#include <random>

namespace stochopt {

	class CostObject {
	public:
		virtual double getCost(std::vector<double> &vect) = 0; // must return legit double value or std::numeric_limits<double>::infinity()
	};

	enum StopCriterion {
		//SIGMA,
		CALLS,
		//COMBO_1,
		ACCURACY,
		//COMBO_2,
		ACCURACY_CALLS,
		TOLERANCE
	};

	class Algorithm {
	public:
		Algorithm(int nDim, int callsm, CostObject* obj);
		virtual ~Algorithm();

		virtual void solve() = 0;

		std::vector<double>		getFinalSolution()	const { return xBestSolution; }
		double					getFinalCost()		const { return costBestSolution; }
		std::vector<double>		getCostGraph()		const { return costGraph; }
		std::vector<std::vector<double>> getxCostGraph() const { return xCostGraph; }
		std::vector<int>		getCallsGraph()		const { return callsGraph; }
		int						getCalls()			const { return calls; }
		std::vector<double> getResiduals() const { return residuals; }

		void setStop(bool &b) { useStop = b; }
		void SetStopCriterion(StopCriterion c) { criterion = c; }
		void SetAccuracy(double accur) { accuracy = accur; }
		void SetTolerance(double tol) { tolerance = tol; }

	protected:
		class entity {
		public:
			std::vector<double> x;
			double cost = std::numeric_limits<double>::infinity();
		};

		int nParticles, nDimensions, nMaxCalls;

		double costBestSolution = std::numeric_limits<double>::infinity();
		int calls = 0;
		double accuracy = 1.e-4;
		double tolerance = 1.e-3;
		CostObject* costObject;
		std::vector<double> xBestSolution;
		std::vector<double> costGraph;
		std::vector<std::vector<double>> xCostGraph;
		std::vector<int> callsGraph;
		std::vector<double> residuals;
		int useSol;
		std::vector<double> initSol;
		bool useStop = false;
		StopCriterion criterion = CALLS;
		bool isConverged();
	};

	struct Borders {
		Borders() {}
		Borders(int nParam_, int min_, int max_);
		std::vector<double> xmin;
		std::vector<double> xmax;
	};


	inline double random()
	{
		return (double)(rand()) / RAND_MAX;
	}



	std::vector<bool> transformToBinary(const std::vector<double>& v_);
	std::vector<int> transformToSequence(const std::vector<double>& v_);
	std::vector<double> transformFromSequence(const std::vector<int>& indx);
	bool isEqual(const std::vector<double>& a, const std::vector<double>& b);


	template <typename T>
	void mutate(std::vector<double>& x, const Borders& borders, T n) {
		int l = x.size();

		std::vector<int> v(l);
		std::iota(v.begin(), v.end(), 0);
		std::random_shuffle(v.begin(), v.end());

		std::vector<double> bxmax = borders.xmax;
		std::vector<double> bxmin = borders.xmin;

		int nMutations = 0;
		if (n >= 1) { // int n - number of elements in x to be mutated
			nMutations = (int)n;
		}
		else if (n > 0) { // double n - % of elements in x to be mutated
			nMutations = (int)(std::ceil(n*l));
		}

		for (int j = 0; j < nMutations; ++j) {
			x.at(v.at(j)) = stochopt::random() * (bxmax.at(v.at(j)) - bxmin.at(v.at(j))) + bxmin.at(v.at(j));
		}
	}

}
