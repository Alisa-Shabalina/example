#include "..\include\snes.h"
#include <numeric>
#include <fstream>
#include <conio.h>
#include <random>
#include <iostream>
#include <algorithm>

namespace stochopt {

	std::vector<int> getOrder(const std::vector<double>& v_) {
		unsigned int s = v_.size();

		std::vector<int> idx(s);
		std::iota(idx.begin(), idx.end(), 0);

		sort(idx.begin(), idx.end(),
			[&v_](int i1, int i2) {return v_[i1] > v_[i2]; });

		return idx;
	}

	SNESBorders::SNESBorders(int nParam_, int min_, int max_) {
		std::vector<double> xmin1(nParam_, min_);
		std::vector<double> xmax1(nParam_, max_);
		xmin = xmin1;
		xmax = xmax1;
		std::pair<double, double> p = { min_, max_ };
		std::vector<std::pair<double, double>> v_p(nParam_, p);
		ranges = v_p;
	}

	SNESBorders::SNESBorders(const std::vector<std::pair<double, double>> &ranges_) {
		ranges = ranges_;
	}

	SNES::SNES(int nDim, int nCalls, CostObject* costobj, 
		SNESBorders* bo, int nPart, double mean_lr, double sigma_lr) 
		: Algorithm(nDim, nCalls, costobj),
		borders(bo)
	{
		//nDimensions = nDim;
		//nMaxCalls = nCalls;
		//costObject = &costobj;
		//borders = &bo;
		//costBestSolution = -std::numeric_limits<double>::infinity();
		nParticles = (nPart == 0) ? 4 + floor(3 * log(nDim)) : nPart;
		learn_rate_mean = (mean_lr == 0) ? 1 : mean_lr;
		learn_rate_sigma = (sigma_lr == 0) ? (3 + log(nDim)) / (5 * sqrt(nDim)) : sigma_lr;
		criterion = CALLS;
		xBestSolution.resize(nDimensions);
		//std::cout << "constructor with maxCalls have worked...\n";
	}

	SNES::SNES(int nDim, int nCalls, CostObject* costobj, 
		double accur, SNESBorders* bo, int nPart, double mean_lr, double sigma_lr)
		: Algorithm(nDim, nCalls, costobj),
		borders(bo)
	{
		//nDimensions = nDim;
		//accuracy = accur;
		//costObject = &costobj;
		//borders = &bo;
		//costBestSolution = -std::numeric_limits<double>::infinity();
		nParticles = (nPart == 0) ? 4 + floor(3 * log(nDim)) : nPart;
		learn_rate_mean = (mean_lr == 0) ? 1 : mean_lr;
		learn_rate_sigma = (sigma_lr == 0) ? (3 + log(nDim)) / (5 * sqrt(nDim)) : sigma_lr;
		criterion = ACCURACY_CALLS;
		xBestSolution.resize(nDimensions);
		SetAccuracy(accuracy);
		//std::cout << "constructor with accuracy have worked...\n";
	}

	void SNES::solve()
	{
		int t = 0;

		std::default_random_engine gen;
		std::random_device rd;
		auto gauss = new std::normal_distribution<>(0, 1);
		gen.seed(rd());

		curr_samples.resize(nParticles, nDimensions);
		curr_members.resize(nParticles, nDimensions);
		U.resize(1, nParticles);
		mean_vec.resize(nDimensions);
		var_vec.resize(nDimensions);

		double threshold = floor(nParticles / 2.);
		double stepsize = 1 / threshold;
		double u = 1;

		for (int i = nParticles - 1; i >= 0; --i) {
			U[i] = u > 0 ? u : 0;
			u -= stepsize;
		}
		U /= U.norm(1);

		//for (int i = 0; i < nDimensions; ++i) {
		//	mean_vec[i] = (*gauss)(gen);
		//	while (mean_vec[i] > borders->ranges[i].second || mean_vec[i] < borders->ranges[i].first)
		//		mean_vec[i] = (*gauss)(gen);
		//	var_vec[i] = 1;
		//	std::cout << "here" << ' ';
		//}

		for (int i = 0; i < nDimensions; ++i) {
			mean_vec[i] = (*gauss)(gen);
			while (mean_vec[i] > 1. || mean_vec[i] < 0.)
				mean_vec[i] = (*gauss)(gen);
			var_vec[i] = 1;
			//std::cout << "here1" << ' ';
		}

		while (!isConverged())
		{
			//std::cout << "here2" << ' ';
			++t;
			//for (int i = 0; i < nParticles; ++i) {
			//	for (int j = 0; j < nDimensions; ++j) {
			//		double g = (*gauss)(gen);
			//		while (g*var_vec[j] + mean_vec[j] > borders->ranges[j].second || g * var_vec[j] + mean_vec[j] < borders->ranges[j].first)
			//			g = (*gauss)(gen);
			//		curr_samples(i, j) = g;
			//		curr_members(i, j) = g * var_vec[j] + mean_vec[j];
			//	}
			//}
			for (int i = 0; i < nParticles; ++i) {
				for (int j = 0; j < nDimensions; ++j) {
					double g = (*gauss)(gen);
					while (g*var_vec[j] + mean_vec[j] > 1. || g * var_vec[j] + mean_vec[j] < 0.)
						g = (*gauss)(gen);
					curr_samples(i, j) = g;
					curr_members(i, j) = g * var_vec[j] + mean_vec[j];
				}
			}
			S = curr_samples.T();

			std::vector<double> fit(nParticles);

			for (int i = 0; i < nParticles; ++i) {
				Matrix row = curr_members.row(i);
				std::vector<double> x;
				for (unsigned int j = 0; j < row.size(); ++j) {
					x.push_back(row[j]);
				}
				DisplayInRealBords(x);

				//for (const auto& item : x) {
				//	std::cout << item << ' ';
				//}
				//std::cout << std::endl;
				fit.at(i) = costObject->getCost(x);
				++calls;
			}

			std::vector<int> order = getOrder(fit);

			Matrix S1(S.rows(), S.cols());
			for (int i = 0; i < nParticles; ++i) {
				Matrix col1 = S.col(order[i]);
				for (int j = 0; j < nDimensions; ++j) {
					S1(j, i) = col1[j];
				}
			}

			mean_grad = U * S1.T();

			for (unsigned int i = 0; i < S1.size(); ++i) {
				S1[i] = S1[i] * S1[i] - 1;
			}

			var_grad = U * S1.T();

			for (int i = 0; i < nDimensions; ++i) {
				mean_vec[i] += learn_rate_mean * var_vec[i] * mean_grad[i];
				var_vec[i] *= exp(learn_rate_sigma / 2. * var_grad[i]);
				xBestSolution[i] = mean_vec[i];
			}
			DisplayInRealBords(xBestSolution);


			costBestSolution = costObject->getCost(xBestSolution);
			if (!costGraph.empty()) {
				if (costBestSolution > costGraph.back()) {
					xBestSolution = xCostGraph.back();
					costBestSolution = costGraph.back();
				}
			}

			xCostGraph.push_back(xBestSolution);
			costGraph.push_back(costBestSolution);

			std::cout << "Solution vector: " << '\n';
			for (auto s : xBestSolution) {
				std::cout << s << " ";
			}
			//std::cout << '\n';
			std::cout << "Cost at solution vector: " << costBestSolution << '\n';

			size_t size = costGraph.size();
			if (size > 2) {
				double residual = std::abs(costGraph[size - 1] - costGraph[size - 2]) / 
					std::abs(costGraph[size - 2]);
				residuals.push_back(std::move(residual));
			}
			++calls;
			callsGraph.push_back(calls);

			//std::cout << "Solution vector: " << '\n';
			//for (auto s : xBestSolution) {
			//	std::cout << s << " ";
			//}
			//std::cout << '\n';

		}

		delete gauss;
	}




	void SNES::DisplayInRealBords(std::vector<double>& old_point) {
		std::vector<double> new_point(old_point.size());
		for (size_t i = 0; i < old_point.size(); ++i) {
			new_point[i] = (borders->ranges[i].second - borders->ranges[i].first) * old_point[i] + borders->ranges[i].first;
		}
		old_point = new_point;
	}

	void SNES::printGraph() {
		std::ofstream out("CostGraph.csv");
		if (out.is_open()) {
			for (int i = 0; i < costGraph.size(); i++)
				out << i + 1 << ";"
				<< costGraph[i] << std::endl;
		}
		out.close();
	}
}
