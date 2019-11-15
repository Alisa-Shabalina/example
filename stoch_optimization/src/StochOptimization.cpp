#include "..\include\StochOptimization.h"

namespace stochopt {

	Borders::Borders(int nParam_, int min_, int max_)
	{
		std::vector<double> xmin1(nParam_, min_);
		std::vector<double> xmax1(nParam_, max_);
		xmin = xmin1;
		xmax = xmax1;
	}

	Algorithm::Algorithm(int nDim, int callsm, CostObject* obj) : nDimensions(nDim), nMaxCalls(callsm), costObject(obj) {}
	Algorithm::~Algorithm() {}

	std::vector<bool> transformToBinary(const std::vector<double>& v_) {
		// v_ - vector of BPSO parameters (usually from -4 to 4)
		// b - stochastic vector of bool parameters. bigger v_[i] value has higher chance to produce b[i]=true
		unsigned int s = v_.size();
		std::vector<bool> b(s);

		for (unsigned int i = 0; i < s; ++i) {
			b[i] = (stochopt::random() < 1 / (1 + exp(-v_[i]))); // sigmoid function, could be 0.5 + 0.5*tanh(0.5*v_[i]), 1/(1 + pow(0.3678749025,v_[i])), ...
		}
		return b;
	}
	std::vector<int> transformToSequence(const std::vector<double>& v_) {
		// v_ - vector of PSO parameters (usually from 0 to 1)
		// idx - sequence of integers from 0 to v_.size()-1, v_ determines the order
		unsigned int s = v_.size();

		std::vector<int> idx(s);
		std::iota(idx.begin(), idx.end(), 0);

		// sort indices based on comparing values in v_
		sort(idx.begin(), idx.end(),
			[&v_](int i1, int i2) {return v_[i1] < v_[i2]; });

		return idx;
	}

	std::vector<double> transformFromSequence(const std::vector<int> & indx)
	{
		// v_ - vector of PSO parameters (usually from 0 to 1)
		// indx - sequence of integers from 0 to v_.size()-1, v_ determines the order
		unsigned int s = indx.size();
		std::vector<double> v_(s);
		double d = 1.0 / s;
		double v = 0;
		for (unsigned int i = 0; i < s; ++i) {
			v_[indx.at(i)] = v;
			v += d;
		}
		return v_;
	}

	bool isEqual(const std::vector<double>& a, const std::vector<double>& b) {
		unsigned int la = a.size();
		unsigned int lb = b.size();
		if (la != lb) {
			return false;
		}
		for (unsigned int i = 0; i < la; ++i) {
			if (a.at(i) != b.at(i)) {
				return false;
			}
		}

		return true;
	}

	bool Algorithm::isConverged() {
		//if (criterion == StopCriterion::SIGMA) {
		//	for (int i = 0; i < nDimensions; i++) {
		//		if (std::abs(var_vec[i]) > tolerance)
		//			return false;
		//	}
		//}

		if (criterion == StopCriterion::CALLS) {
			if (calls < nMaxCalls)
				return false;
		}

		//if (criterion == StopCriterion::COMBO_1) {
		//	if (calls < nMaxCalls) {
		//		for (int i = 0; i < nDimensions; i++) {
		//			if (std::abs(var_vec[i]) > tolerance)
		//				return false;
		//		}
		//	}
		//	else {
		//		return true;
		//	}
		//}

		if (criterion == StopCriterion::ACCURACY) {
			if (std::abs(costBestSolution) > accuracy) {
				return false;
			}
		}

		//if (criterion == StopCriterion::COMBO_2) {
		//	if (std::abs(costBestSolution) > accuracy) {
		//		for (int i = 0; i < nDimensions; i++) {
		//			if (std::abs(var_vec[i]) > tolerance)
		//				return false;
		//		}
		//	}
		//	else {
		//		return true;
		//	}
		//}

		if (criterion == StopCriterion::ACCURACY_CALLS) {
			if (std::abs(costBestSolution) > accuracy) {
				if (calls >= nMaxCalls) return true;
				return false;
			}
			else {
				return true;
			}
		}

		if (criterion == StopCriterion::TOLERANCE) {
			if (costGraph.size() <= 30) {
				return false;
			}
			else {
				if (residuals.back() <= tolerance) {
					return true;
				}
				else {
					return false;
				}
			}
		}
		//return true;
	}

}
