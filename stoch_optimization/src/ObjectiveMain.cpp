#include "..\include\ObjectiveMain.h"

double Rosenbrock::getCost(std::vector<double> &vect) {
	// Example of using discretize()
	// Corresponding parameters from bo1 are set in Objective.h
	double cost = 0;
	std::vector<double> v = vect;
	int n = v.size();

	//	std::vector<double> vd(v.begin(), v.begin() + n / 2);
	//	discretize(vd, bo1);
	//	std::copy(vd.begin(), vd.end(), v.begin());

	for (int i = 0; i < n - 1; ++i) {
		cost += 100 * pow(v[i + 1] - pow(v[i], 2), 2) + pow((v[i] - 1), 2);
	}
	return cost;
}

double Sphere::getCost(std::vector<double> &vect) {
	double cost = 0;
	for (auto &z : vect)
	{
		cost += z * z;
	}
	cost = sqrt(cost);
	return cost;
}


