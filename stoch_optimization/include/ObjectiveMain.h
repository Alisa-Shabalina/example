#pragma once
//#include "header.h"
#include <vector>
#include "StochOptimization.h"
#include "pso.h"
#include "C:/Libs/rapidjson-master/include/rapidjson/document.h"
#include "C:/Libs/rapidjson-master/include/rapidjson/istreamwrapper.h"
#include "../../generator/include/VariableValue.h"
#include "../../generator/include/Recalculator.h"
#include "../../fracture_model/include/FracModel.h"
#include "../../gd_solver/include/Solver.h"
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

enum class OptimizingValue {
	NetPressure,
	FluidEfficiency,
	ShutInTime,
	WOPT,
	FractureLength,
	NPV                    //net present value
};

class RealCase : public stochopt::CostObject {
public:


	RealCase(const Json::Value& initial_data, const std::vector<std::shared_ptr<VariableValue>>& values,
		const std::map<std::string, double>& real_values, const std::map<std::string, double>& restrictions,
		std::shared_ptr<FracModel> model, const std::vector<OptimizingValue>& to_optimize) :
		InitialData(initial_data), Values(values), RealValues(real_values),
		Restrinctions(restrictions), Model(model), ToOptimize(to_optimize)
	{
		NumberOfProps = InitialData["ProppantBase"]["data"].size();
		if (to_optimize.size() == 1 && to_optimize.at(0) == OptimizingValue::NPV) {
			Json::Value& temp = InitialData["Design"]["Stages"][0]["FracturePumpingSchedule"];
			int t_s_v_key, t_p_m_key;
			for (const auto& id : temp["columnKeys"].getMemberNames()) {
				if (temp["columnKeys"][id].asString() == "TOTAL_SLURRY_VOLUME")
					t_s_v_key = std::stoi(id);
				if (temp["columnKeys"][id].asString() == "TOTAL_PROPANT_MASS")
					t_p_m_key = std::stoi(id);
			}
			size_t last = temp["data"].size();
			TotalPropMass = temp["data"][last][t_p_m_key].asDouble();
			TotalSlurryVolume = temp["data"][last][t_s_v_key].asDouble();
		}
	}

	std::map<ValueName, std::vector<double>> ConvertVectorPoint(std::vector<double>& point);

	double SingleOptimization(rapidjson::Value& res);

	double MultiOptimization(rapidjson::Value& res);

	double getCost(std::vector<double>& point);
private:
	Json::Value InitialData;
	std::vector<std::shared_ptr<VariableValue>> Values;
	std::map<std::string, double> RealValues;
	std::map<std::string, double> Restrinctions;
	unsigned int NumberOfProps;

	std::shared_ptr<FracModel> Model;

	std::vector<OptimizingValue> ToOptimize;

	double TotalPropMass;
	double TotalSlurryVolume;
};

class SyntheticCase : public stochopt::CostObject {
public:


	SyntheticCase(const Json::Value& initial_data, const std::vector<std::shared_ptr<VariableValue>>& values,
		const std::map<std::string, double>& real_values, const std::map<std::string, double>& restrictions,
		std::shared_ptr<FracModel> model, const std::vector<OptimizingValue>& to_optimize) :
		InitialData(initial_data), Values(values), RealValues(real_values),
		Restrinctions(restrictions), Model(model), ToOptimize(to_optimize)
	{
		RealValues["net pressure"] = 3.8e6;
		RealValues["fluid efficiency"] = 0.4;
		RealValues["shut-in time"] = 2000.;
		NumberOfProps = InitialData["ProppantBase"]["data"].size();
	}

	std::map<ValueName, std::vector<double>> ConvertVectorPoint(std::vector<double>& point);

	double SingleOptimization(rapidjson::Value& res);

	double MultiOptimization(rapidjson::Value& res);

	double getCost(std::vector<double>& point);
private:
	Json::Value InitialData;
	std::vector<std::shared_ptr<VariableValue>> Values;
	std::map<std::string, double> RealValues;
	std::map<std::string, double> Restrinctions;
	unsigned int NumberOfProps;

	std::shared_ptr<FracModel> Model;

	std::vector<OptimizingValue> ToOptimize;
};















































