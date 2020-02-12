#include "generator/include/DataForRunning.h"
#include "fracture_model/include/FracModel.h"
#include "generator/include/Recalculator.h"
#include "stoch_optimization/include/snes.h"
#include "C:/Libs/rapidjson-master/include/rapidjson/document.h"
#include "C:/Libs/rapidjson-master/include/rapidjson/istreamwrapper.h"
#include <chrono>
#include <numeric>
#include <future>
#include "stoch_optimization/include/snes.h"
#include "stoch_optimization/include/ObjectiveMain.h"

//std::vector<std::pair<double, double>> GetBounds_1(VariableValue c) {
//	auto data = c.GetValBoundsNum();
//	std::vector<std::pair<double, double>> res;
//	for (const auto& item : data) {
//		if (item.second[2] != 0) {
//			res.push_back({ item.second[0], item.second[1] });
//		}
//	}
//	return res;
//}
//
//std::vector<std::vector<double>> GetBounds_2(VariableValue c) {
//	auto data = c.GetValBoundsNum();
//	std::vector<std::vector<double>> res;
//	for (const auto& item : data) {
//		if (item.second[2] != 0) {
//			res.push_back({ item.second[0], item.second[1] });
//		}
//	}
//	return res;
//}
//
//std::vector<std::vector<double>> GetBounds1(VariableValue c) {
//	auto data = c.GetValBoundsNum();
//	std::vector<std::vector<double>> res;
//	for (const auto& item : data) {
//		if (item.second[2] != 0) {
//			res.push_back({ item.second[0], item.second[1] });
//		}
//	}
//	return res;
//}

void plotSolution(std::vector<double> v) {
	std::cout << "Solution vector: " << '\n';
	for (auto s : v) {
		std::cout << s << " ";
	}
	std::cout << '\n';
}

double GetMean(std::vector<double>& v) {
	auto sum = std::accumulate(v.begin(), v.end(), 0.);
	return sum / v.size();
}

int main() {

	std::ifstream ifs1("fracture_solver_init_main.json");
	Json::Reader reader1;
	Json::Value obj1;
	reader1.parse(ifs1, obj1);
	
	std::ifstream ifs2("VariableValues.json");
	Json::Reader reader2;
	Json::Value obj2;
	reader2.parse(ifs2, obj2);
	
	std::ifstream ifs3("RealValues.json");
	Json::Reader reader3;
	Json::Value obj3;
	reader3.parse(ifs3, obj3);
	
	Parser p1(obj2);
	std::vector<std::shared_ptr<VariableValue>> values = p1.ReadVariableValues();
	Parser main(obj1);
	std::map<std::string, std::string> prop_base = main.ReadPropantBase();
	for (const auto& item : values) {
		auto name = item->GetValueName();
		if (name == ValueName::FLUID_TYPE || name == ValueName::PROPANT_TYPE) {
			//std::shared_ptr<CategoricalVariableValue> it = std::make_shared<CategoricalVariableValue>(item);
			item->FillId(prop_base);
		}

	}

	Parser p2(obj3);
	std::map<std::string, double> real_values = p2.ReadRealValues();
	std::map<std::string, double> restrinctions = p2.ReadRestrictions();
	//for (const auto& item : real_values) {
	//	std::cout << item.first << ' ' << item.second << '\n';
	//}
	
	//Recalculator r(values, obj1);
	////auto init_obj = r.Recalculate(std::map<ValueName, std::vector<double>>({ {ValueName::PROPANT_MASS, {1, 0}},
	////	{ValueName::SLURRY_VOLUME, {1, 0} },
	////	{ValueName::PROPANT_TYPE, {1} } }
	////));
	//auto init_obj = r.Recalculate(std::map<ValueName, std::vector<double>>({
	//	{ValueName::PROPANT_TYPE, {1} },
	//	{ValueName::SLURRY_VOLUME, {1, 0} } }
	//));
	//
	//Planar3D sol;
	//sol.Run(init_obj);

	std::ifstream input("input.txt");
	std::vector<double> point;
	std::string line;
	while (std::getline(input, line)) {
		std::cout << line << '\n';
		point.push_back(std::stod(line));
	}
	
	RealCase fun(obj1, values, 
		std::map<std::string, double>(), 
		std::map<std::string, double>(),
		std::make_shared<Planar3D>(), 
		{ OptimizingValue::NPV });
	double qtotal = fun.getCost(point);
	
	std::ofstream output("output.txt");
	output << qtotal << '\n';
	
	//RealCase fun(obj1, values, 
	//	std::map<std::string, double>(), 
	//	restrinctions,
	//	std::make_shared<Planar3D>(), 
	//	{ OptimizingValue::WOPT });
	//std::vector<std::pair<double, double>> boards;
	//for (const auto& i : values) {
	//	auto item = GetBounds_1(i);
	//	boards.insert(boards.end(), item.begin(), item.end());
	//}
	//int nParam = static_cast<int>(boards.size());
	//
	//for (const auto& item : boards) {
	//	std::cout << item.first << ' ' << item.second << std::endl;
	//}
	//stochopt::SNESBorders bo(boards);
	//int MaxCalls = 100;
	//stochopt::SNES snes(nParam, MaxCalls, &fun, &bo, 0, 0, 0);
	//snes.SetStopCriterion(stochopt::StopCriterion::CALLS);
	//auto t1 = std::chrono::high_resolution_clock::now();
	//snes.solve();
	//auto t2 = std::chrono::high_resolution_clock::now();
	//auto dt = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
	//std::cout << " ****** " << '\n';
	//std::cout << "Time elapsed: " << dt.count() << " seconds" << '\n';
	//double time = static_cast<double>(dt.count());
	//
	//auto data = snes.getCallsGraph();
	//auto data1 = snes.getxCostGraph();
	//auto x_solutions = data1.back();
	//auto data2 = snes.getCostGraph();
	//auto solutions = data2.back();
	//
	//auto calls = snes.getCalls();
	//std::cout << "++++++++++++++++++++++++++++++++++" << '\n';
	//
	//for (size_t i = 0; i < data1.size(); ++i) {
	//	for (auto j : data1[i]) {
	//		std::cout << j << ' ';
	//	}
	//	std::cout << data2[i] << '\n';
	//}
	//std::cout << "++++++++++++++++++++++++++++++++++" << '\n';
	//
	//std::ofstream output("snes_qtotal_planar_restr.txt");
	//output << snes.getCalls() << '\n';
	//output << dt.count() << '\n';
	//for (const auto& item : snes.getFinalSolution()) {
	//	output << item << ' ';
	//}
	//output << '\n';
	//output << snes.getFinalCost() << std::endl;
	//for (size_t i = 0; i < data1.size(); ++i) {
	//	output << data[i] << ' ';
	//	for (auto j : data1[i]) {
	//		output << j << ' ';
	//	}
	//	output << data2[i] << '\n';
	//}
	//
	//auto residuals = snes.getResiduals();
	//output << "****************************************\n";
	//for (const auto& item : residuals) {
	//	output << item << '\n';
	//}
	
	//Sphere s;
	//
	//stochopt::SNESBorders bo(std::vector<std::pair<double, double>>(10, {-1, 1}));
	//stochopt::SNES snes(10, 1000, &s, &bo, 0, 0, 0);
	//snes.solve();
	return 0;
}