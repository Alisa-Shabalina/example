#pragma once

#include "VariableValue.h"
#include "../../jsoncpp/include/json/json.h"
#include "../../jsoncpp/include/json/forwards.h"
#include <memory>
#include <algorithm>
#include <iostream>
#include <numeric>

class Recalculator {
public:
	Recalculator(std::vector<std::shared_ptr<VariableValue>> values, Json::Value& base_file) 
		: Values(values), BaseFile(base_file) 
	{
	}

	Json::Value Recalculate(const std::map<ValueName, std::vector<double>>& point);
private:
	std::vector<std::shared_ptr<VariableValue>> Values;
	Json::Value BaseFile;

	std::vector<double> GetValuesByKey(Json::Value val, const int& key);
	std::vector<double> CreateNewValues(const VaryType& type, const std::vector<double>& values, const std::vector<double>& wanted_values);
	std::vector<double> ClearFields(const std::vector<double>& input, const std::vector<int>& indexes);

};
