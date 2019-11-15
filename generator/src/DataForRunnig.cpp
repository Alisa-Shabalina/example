#include "../include/DataForRunning.h"

void CreateValues(VariableValue& c) { c.CreateValues(); }

//bool DataForRunning::IsAppropiateItem(const std::vector<int>& item, const std::vector<int>& model) {
//	int res = 0;
//	for (int i = 0; i < item.size(); ++i) {
//		if (item[i] < model[i]) {
//			res += 1;
//		} 
//	}
//
//	if (res == model.size()) {
//		return true;
//	}
//	else {
//		return false;
//	}
//}
//
//void DataForRunning::CreateTemplateSetOfValues(const int& number_of_constants, const int& number_of_values) {
//	int length = pow(number_of_values, number_of_constants);
//	std::vector<std::vector<int>> result;
//	std::vector<int> number_of_combinations;
//	for (int i = 0; i <= length - 1; ++i) {
//		number_of_combinations.push_back(i);
//	}
//	for (const auto& i : number_of_combinations) {
//		if (i == 0) {
//			std::vector<int> item(number_of_constants, 0);
//			result.push_back(item);
//		}
//		else {
//			std::vector<int> item;
//			int num = i;
//			while (num > 0) {
//				item.push_back(num % number_of_values);
//				num /= number_of_values;
//			}
//			if (static_cast<int>(item.size()) < number_of_constants) {
//				item.resize(number_of_constants, 0);
//			}
//			std::reverse(begin(item), end(item));
//			result.push_back(item);
//		}
//	}
//	TemplateSet = result;
//}
//
//void DataForRunning::CreateTemplateSetOfValues(std::vector<std::shared_ptr<VariableValue>>& constants) {
//	std::vector<std::vector<int>> result;
//
//	std::sort(constants.begin(), constants.end(), 
//	[](std::shared_ptr<VariableValue> a, std::shared_ptr<VariableValue> b) {
//		int num1 = GetNumberOfValues(*a);
//		int num2 = GetNumberOfValues(*b);
//		return num1 < num2;
//	});
//
//	int power = constants.size();
//	std::vector<int> bases_in_order;
//	for (const auto& item : constants) {
//		bases_in_order.push_back(GetNumberOfValues(*item));
//	}
//	int base = bases_in_order.back();
//	int length = pow(base, power);
//	std::vector<int> number_of_combinations;
//	for (int i = 0; i <= length - 1; ++i) {
//		number_of_combinations.push_back(i);
//	}
//
//	for (const auto& i : number_of_combinations) {
//		if (i == 0) {
//			std::vector<int> item(power, 0);
//			result.push_back(item);
//		}
//		else {
//			std::vector<int> item;
//			int num = i;
//			while (num > 0) {
//				item.push_back(num % base);
//				num /= base;
//			}
//			if (static_cast<int>(item.size()) < power) {
//				item.resize(power, 0);
//			}
//			std::reverse(begin(item), end(item));
//			if (IsAppropiateItem(item, bases_in_order)) {
//				result.push_back(item);
//			}
//		}
//	}
//	TemplateSet = result;
//}
//
//void DataForRunning::CreateValuesSet(std::vector<std::shared_ptr<VariableValue>>& constants) {
//	std::sort(constants.begin(), constants.end(),
//		[](std::shared_ptr<VariableValue> a, std::shared_ptr<VariableValue> b) {
//		int num1 = GetNumberOfValues(*a);
//		int num2 = GetNumberOfValues(*b);
//		return num1 < num2;
//	});
//
//
//
//	std::map<int, std::map<int, double>> new_data;
//	int incr = 0;
//	for (const auto& item : constants) {
//		ValuesNames.push_back(GetValueName(*item));
//		CreateValues(*item);
//		std::map<int, double> temp1;
//		std::vector<double> temp2 = GetValues(*item);
//		for (int i = 0; i < temp2.size(); ++i) {
//			temp1[i] = temp2[i];
//		}
//		new_data[incr] = temp1;
//		incr++;
//	}
//
//	//for (const auto& i : new_data) {
//	//	std::cout << i.first << ' ';
//	//	for (const auto& j : i.second) {
//	//		std::cout << j.first << ' ' << j.second << ' ';
//	//	}
//	//	std::cout << std::endl;
//	//}
//
//	CreateTemplateSetOfValues(constants);
//	std::vector<double> internal(constants.size(), 0.);
//	std::vector<std::vector<double>> result(TemplateSet.size(), internal);
//
//	for (size_t i = 0; i < TemplateSet.size(); ++i) {
//		for (size_t j = 0; j < TemplateSet[i].size(); ++j) {
//			for (const auto& k : new_data[j]) {
//				if (TemplateSet[i][j] == k.first) {
//					result[i][j] = k.second;
//				}
//			}
//		}
//	}
//	ValuesSet = result;
//}
//
//void DataForRunning::CreateData(Json::Value VarValues, Json::Value obj) {
//	//создаем наборы всевозможных комбинаций значений величин
//	Parser p(VarValues);
//	std::vector<std::shared_ptr<VariableValue>> VariableValues = p.ReadVariableValues();
//	CreateTemplateSetOfValues(VariableValues.size(), GetNumberOfValues(*VariableValues[0]));
//	CreateValuesSet(VariableValues);
//	std::cout << ValuesSet.size() << std::endl;
//	
//	//считываем базовый файл
//	//std::ifstream ifs(BaseFile);
//	//Json::Reader reader;
//	//Json::Value obj;
//	//reader.parse(ifs, obj);
//
//	for (int i = 0; i < ValuesSet.size(); ++i) {
//		auto temp = obj;
//		for (int j = 0; j < ValuesSet[i].size(); ++j) {
//			//std::cout << i << ' ' << j << ' ' << Data.size() << std::endl;
//
//			switch (ValuesNames[j]) {
//				case ValueName::FLUID_LOSS: 
//				{	
//					Json::Value& val = temp["Design"]["Stages"][0]["Ports"][0];
//					double z_top = val["z_top"].asDouble();
//					double z_bot = val["z_bottom"].asDouble();
//					Json::Value& val1 = val["ReservoirFormation"];
//					int TVD_top_key, TVD_bot_key, FL_key;
//					for (const auto& id : val1["columnKeys"].getMemberNames()) {
//						if (val1["columnKeys"][id].asString() == "TVD_TOP") TVD_top_key = std::stoi(id);
//						if (val1["columnKeys"][id].asString() == "TVD_BOT") TVD_bot_key = std::stoi(id);
//						if (val1["columnKeys"][id].asString() == "FLUID_LOSS") FL_key = std::stoi(id);
//					}
//					for (int k = 0; k < val1["data"].size(); ++k) {
//						if (std::stod(val1["data"][k][TVD_top_key].asString()) <= z_top ||
//							std::stod(val1["data"][k][TVD_bot_key].asString()) >= z_bot) {
//							val1["data"][k][FL_key] = std::to_string(ValuesSet[i][j]);
//						}
//						else {
//							val1["data"][k][FL_key] = std::to_string(std::stod(val1["data"][k][FL_key].asString()) * std::stod(val1["data"][k][FL_key].asString()) / ValuesSet[i][j]);
//						}
//					}
//					break;
//
//				//Json::Value& val = obj["Design"]["Stages"][0]["Ports"][0]["ReservoirFormation"];
//				//int key;
//				//for (const auto& id : val["columnKeys"].getMemberNames()) {
//				//	if (val["columnKeys"][id].asString() == "FLUID_LOSS") {
//				//		key = std::stoi(id);
//				//	}
//				//}
//				//for (size_t k = 0; k < val["data"].size(); ++k) {
//				//	val["data"][k][key] = item[j];
//				//}
//				//Data.push_back(obj);
//				//break;//переписать как коэф Пуассона
//				}
//
//				case ValueName::SLURRY_RATE:
//				{
//					Json::Value& val = temp["Design"]["Stages"][0]["FracturePumpingSchedule"];
//					int key1, key2;
//					for (const auto& id : val["columnKeys"].getMemberNames()) {
//						if (val["columnKeys"][id].asString() == "SLURRY_RATE") key1 = std::stoi(id);
//						if (val["columnKeys"][id].asString() == "SLURRY_VOLUME") key2 = std::stoi(id);
//					}
//					for (int k = 0; k < val["data"].size(); ++k) {
//						double Q_old = std::stod(val["data"][k][key1].asString());
//						double V_old = std::stod(val["data"][k][key2].asString());
//						val["data"][k][key1] = std::to_string(ValuesSet[i][j]);
//						val["data"][k][key2] = std::to_string(ValuesSet[i][j] * V_old / Q_old);
//
//					}
//					break;
//				}
//
//				case ValueName::POISSON_RATIO: 
//				{
//					Json::Value& val = temp["Design"]["Stages"][0]["Ports"][0];
//					double z_top = val["z_top"].asDouble();
//					double z_bot = val["z_bottom"].asDouble();
//					Json::Value& val1 = val["ReservoirFormation"];
//					int TVD_top_key, TVD_bot_key, POI_key;
//					for (const auto& id : val1["columnKeys"].getMemberNames()) {
//						if (val1["columnKeys"][id].asString() == "TVD_TOP") TVD_top_key = std::stoi(id);
//						if (val1["columnKeys"][id].asString() == "TVD_BOT") TVD_bot_key = std::stoi(id);
//						if (val1["columnKeys"][id].asString() == "POISSON_RATIO") POI_key = std::stoi(id);
//					}
//					for (int k = 0; k < val1["data"].size(); ++k) {
//						if (std::stod(val1["data"][k][TVD_top_key].asString()) <= z_top ||
//							std::stod(val1["data"][k][TVD_bot_key].asString()) >= z_bot) {
//							val1["data"][k][POI_key] = std::to_string(ValuesSet[i][j]);
//						}
//						else {
//							val1["data"][k][POI_key] = std::to_string(std::stod(val1["data"][k][POI_key].asString()) * std::stod(val1["data"][k][POI_key].asString()) / ValuesSet[i][j]);
//						}
//					}
//					break;
//				}
//
//				case ValueName::POWER_OF_FLUID:
//				{
//					Json::Value& val = temp["FluidBase"]["columnKeys"];
//					int key;
//					for (const auto& id : val.getMemberNames()) {
//						if (val[id].asString() == "RHEOLOGY_TABLE") key = std::stoi(id);
//					}
//					Json::Value& val1 = temp["FluidBase"]["data"][key]["data"];
//					for (int k = 0; k < val1.size(); ++k) {
//						for (int l = 0; l < val1[k][0]["data"].size(); ++l) {
//							val1[k][0]["data"][l][2] = std::to_string(ValuesSet[i][j]);
//						}
//					}
//					break;
//				}
//
//				case ValueName::PROPANT_CONCENTRATION_START: 
//				{
//					Json::Value& val = temp["Design"]["Stages"][0]["FracturePumpingSchedule"];
//					int start_key, end_key;
//					for (const auto& id : val["columnKeys"].getMemberNames()) {
//						if (val["columnKeys"][id].asString() == "PROPANT_CONCENTRATION_START") start_key = std::stoi(id);
//						if (val["columnKeys"][id].asString() == "PROPANT_CONCENTRATION_END") end_key = std::stoi(id);
//					}
//					for (int k = 0; k < val["data"].size(); ++k) {
//						val["data"][k][start_key] = std::to_string(ValuesSet[i][j]);
//						val["data"][k][end_key] = std::to_string(ValuesSet[i][j]);
//					}
//					break;
//				}
//
//				case ValueName::TIME:
//				{
//					Json::Value& val = temp["Design"]["Stages"][0]["FracturePumpingSchedule"];
//					int key1, key2;
//					for (const auto& id : val["columnKeys"].getMemberNames()) {
//						if (val["columnKeys"][id].asString() == "TIME") key1 = std::stoi(id);
//						if (val["columnKeys"][id].asString() == "SLURRY_VOLUME") key2 = std::stoi(id);
//					}
//					for (int k = 0; k < val["data"].size(); ++k) {
//						double T_old = std::stod(val["data"][k][key1].asString());
//						double V_old = std::stod(val["data"][k][key2].asString());
//						val["data"][k][key1] = std::to_string(ValuesSet[i][j]);
//						val["data"][k][key2] = std::to_string(ValuesSet[i][j] * V_old / T_old);
//					}
//					break;
//				}
//
//				case ValueName::YOUNG_MODULUS: 
//				{
//					Json::Value& val = temp["Design"]["Stages"][0]["Ports"][0];
//					double z_top = val["z_top"].asDouble();
//					double z_bot = val["z_bottom"].asDouble();
//					Json::Value& val1 = val["ReservoirFormation"];
//					int TVD_top_key, TVD_bot_key, YM_key;
//					for (const auto& id : val1["columnKeys"].getMemberNames()) {
//						if (val1["columnKeys"][id].asString() == "TVD_TOP") TVD_top_key = std::stoi(id);
//						if (val1["columnKeys"][id].asString() == "TVD_BOT") TVD_bot_key = std::stoi(id);
//						if (val1["columnKeys"][id].asString() == "YOUNG_MODULUS") YM_key = std::stoi(id);
//					}
//					for (int k = 0; k < val1["data"].size(); ++k) {
//						if (std::stod(val1["data"][k][TVD_top_key].asString()) <= z_top ||
//							std::stod(val1["data"][k][TVD_bot_key].asString()) >= z_bot) {
//							val1["data"][k][YM_key] = std::to_string(ValuesSet[i][j]);
//						}
//						else {
//							val1["data"][k][YM_key] = std::to_string(std::stod(val1["data"][k][YM_key].asString()) * std::stod(val1["data"][k][YM_key].asString()) / ValuesSet[i][j]);
//						}
//					}
//					break;
//				}
//
//				case ValueName::СONSISTENCY:
//				{
//					Json::Value& val = temp["FluidBase"]["columnKeys"];
//					int key;
//					for (const auto& id : val.getMemberNames()) {
//						if (val[id].asString() == "RHEOLOGY_TABLE") key = std::stoi(id);
//					}
//					Json::Value& val1 = temp["FluidBase"]["data"][key]["data"];
//					for (int k = 0; k < val1.size(); ++k) {
//						for (int l = 0; l < val1[k][0]["data"].size(); ++l) {
//							val1[k][0]["data"][l][1] = std::to_string(ValuesSet[i][j]);
//						}
//					}
//					break;
//				}
//
//			}
//
//		}
//		Data.push_back(temp);
//		//std::cout << i + 1 << ' ' << Data.size() << std::endl;
//	}
//}