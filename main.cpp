#include "generator/include/DataForRunning.h"
#include "fracture_model/include/FracModel.h"
#include "generator/include/Recalculator.h"
#include "stoch_optimization/include/snes.h"
#include "C:/Libs/rapidjson-master/include/rapidjson/document.h"
#include "C:/Libs/rapidjson-master/include/rapidjson/istreamwrapper.h"
#include <chrono>
#include <numeric>
#include "stoch_optimization/include/snes.h"
#include "stoch_optimization/include/ObjectiveMain.h"

std::vector<std::pair<double, double>> GetBounds_1(VariableValue c) {
	auto data = c.GetValBoundsNum();
	std::vector<std::pair<double, double>> res;
	for (const auto& item : data) {
		if (item.second[2] != 0) {
			res.push_back({ item.second[0], item.second[1] });
		}
	}
	return res;
}

std::vector<std::vector<double>> GetBounds_2(VariableValue c) {
	auto data = c.GetValBoundsNum();
	std::vector<std::vector<double>> res;
	for (const auto& item : data) {
		if (item.second[2] != 0) {
			res.push_back({ item.second[0], item.second[1] });
		}
	}
	return res;
}

std::vector<std::vector<double>> GetBounds1(VariableValue c) {
	auto data = c.GetValBoundsNum();
	std::vector<std::vector<double>> res;
	for (const auto& item : data) {
		if (item.second[2] != 0) {
			res.push_back({ item.second[0], item.second[1] });
		}
	}
	return res;
}

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
	std::vector<VariableValue> values = p1.ReadVariableValues();
	
	Parser p2(obj3);
	std::map<std::string, double> real_values = p2.ReadRealValues();
	//std::map<std::string, double> restrinctions = p2.ReadRestrictions();
	for (const auto& item : real_values) {
		std::cout << item.first << ' ' << item.second << '\n';
	}
	
	RealCase fun(obj1, values, real_values, std::map<std::string, double>(),
		std::make_shared<Pseudo3D>(), { OptimizingValue::WOPT });
	double accuracy = 1.e-3;
	std::vector<std::pair<double, double>> boards;
	for (const auto& i : values) {
		auto item = GetBounds_1(i);
		boards.insert(boards.end(), item.begin(), item.end());
	}
	int nParam = static_cast<int>(boards.size());
	
	for (const auto& item : boards) {
		std::cout << item.first << ' ' << item.second << std::endl;
	}
	stochopt::SNESBorders bo(boards);
	int MaxCalls = 150;
	stochopt::SNES snes(nParam, MaxCalls, &fun, &bo, 0, 0., 0.);
	snes.SetStopCriterion(stochopt::StopCriterion::CALLS);
	auto t1 = std::chrono::high_resolution_clock::now();
	snes.solve();
	auto t2 = std::chrono::high_resolution_clock::now();
	auto dt = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
	std::cout << " ****** " << '\n';
	std::cout << "Time elapsed: " << dt.count() << " seconds" << '\n';
	double time = static_cast<double>(dt.count());
	
	auto data1 = snes.getxCostGraph();
	auto x_solutions = data1.back();
	auto data2 = snes.getCostGraph();
	auto solutions = data2.back();
	
	auto calls = snes.getCalls();
	std::cout << "++++++++++++++++++++++++++++++++++" << '\n';
	
	for (size_t i = 0; i < data1.size(); ++i) {
		for (auto j : data1[i]) {
			std::cout << j << ' ';
		}
		std::cout << data2[i] << '\n';
	}
	std::cout << "++++++++++++++++++++++++++++++++++" << '\n';
	
	std::ofstream output("snes_qtotal.txt");
	output << snes.getCalls() << '\n';
	output << dt.count() << '\n';
	for (const auto& item : snes.getFinalSolution()) {
		output << item << ' ';
	}
	output << '\n';
	output << snes.getFinalCost() << std::endl;
	for (size_t i = 0; i < data1.size(); ++i) {
		for (auto j : data1[i]) {
			output << j << ' ';
		}
		output << data2[i] << '\n';
	}

	auto residuals = snes.getResiduals();
	output << "****************************************\n";
	for (const auto& item : residuals) {
		output << item << '\n';
	}
	

	return 0;
}