#pragma once
//#include "header.h"
#include <vector>
#include "StochOptimization.h"
#include "pso.h"
#include "C:/Libs/rapidjson-master/include/rapidjson/document.h"
#include "C:/Libs/rapidjson-master/include/rapidjson/istreamwrapper.h"
//#include "../../generator/include/VariableValue.h"
//#include "../../generator/include/Recalculator.h"
//#include "../../fracture_model/include/FracModel.h"
#define _USE_MATH_DEFINES
#include <math.h>

class Rosenbrock : public stochopt::CostObject {
public:
	double getCost(std::vector<double> &vect);

	//	stochopt::PSOBorders bo1{ 10, { -5, -3, -1, 0, 1, 3, 5 }, 0 };
};

class Sphere : public stochopt::CostObject {
public:
	double getCost(std::vector<double> &vect);
};

class Rastrigin : public stochopt::CostObject {
public:
	double getCost(std::vector<double> &vect);
};

class StyblinskiTang : public stochopt::CostObject {
public:
	double getCost(std::vector<double> &vect);

	//	stochopt::PSOBorders bo2{ 20, -5, 5, 0 };
};













































