#pragma once

#include "VariableValue.h"
#include "Parser.h"
#include <cmath>
#include <algorithm>
#include <memory>
#include <map>
#include <iostream>
#include <string>
#include <numeric>

void CreateValues(VariableValue& c);

class DataForRunning {
public:
	std::vector<std::vector<double>> GetValuesSet() const { return ValuesSet; }
	std::vector<std::vector<int>> GetTemplateSet() const { return TemplateSet; }
	std::vector<ValueName> GetValuesNames() const { return ValuesNames; }
	std::vector<Json::Value> GetData() const { return Data; }

	void CreateTemplateSetOfValues(const int& number_of_constants, const int& number_of_values);
	void CreateTemplateSetOfValues(std::vector<std::shared_ptr<VariableValue>>& constants);
	void CreateValuesSet(std::vector<std::shared_ptr<VariableValue>>& constants);

	void CreateData(Json::Value VarValues, Json::Value obj);
private:
	//переменные для создания набора переменных
	std::vector<std::vector<int>> TemplateSet;
	std::vector<std::vector<double>> ValuesSet;
	std::vector<ValueName> ValuesNames;

	//вспомогательные функции
	bool IsAppropiateItem(const std::vector<int>& item, const std::vector<int>& model);

	//вектор из пусковых файлов
	std::vector<Json::Value> Data;
};

template<typename T>
std::vector<std::vector<T>> MatrixTranspose(const std::vector<std::vector<T>>& matrix) {
	std::vector<std::vector<T>> result(matrix[0].size(), std::vector<T>());

	for (size_t i = 0; i < matrix.size(); i++) {
		for (size_t j = 0; j < matrix[i].size(); j++) {
			result[j].push_back(matrix[i][j]);
		}
	}
	return result;
}

