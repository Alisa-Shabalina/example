#include "../include/Parser.h"

//std::vector<std::shared_ptr<VariableValue>> Parser::ReadVariableValues() {
//
//	const Json::Value& values = Doc["VariableValue"];
//	std::vector<std::shared_ptr<VariableValue>> InitialData;
//	std::map<ValueName, ValueName> relations;
//	for (int i = 0; i < values.size(); ++i) {
//		std::string name = values[i]["ValueName"].asString();
//		std::string vary_type = values[i]["VaryType"].asString();
//		//std::cout << name << ", " << vary_type << std::endl;
//		VaryType type;
//		std::unordered_map<VaryValue, std::vector<double>> val_bounds_num;
//		if (vary_type == "min_max_values") {
//			type = VaryType::min_max_values;
//			const Json::Value& min = values[i]["min"];
//			const Json::Value& max = values[i]["max"];
//			std::vector<double> temp1, temp2;
//			for (int j = 0; j < min.size(); ++j) {
//				temp1.push_back(min[j].asDouble());
//				temp2.push_back(max[j].asDouble());
//			}
//			val_bounds_num[VaryValue::min] = temp1;
//			val_bounds_num[VaryValue::max] = temp2;
//		}
//		else {
//			if (vary_type == "coef_lin_trans") {
//				type = VaryType::coef_lin_trans;
//				const Json::Value& k = values[i]["k"];
//				const Json::Value& b = values[i]["b"];
//				std::vector<double> temp1, temp2;
//				for (int j = 0; j < k.size(); ++j) {
//					temp1.push_back(k[j].asDouble());
//					temp2.push_back(b[j].asDouble());
//				}
//				val_bounds_num[VaryValue::k] = temp1;
//				val_bounds_num[VaryValue::b] = temp2;
//			}
//		}
//
//		ValueName external_relation;
//		if (values[i].isMember("Relation")) {
//			std::string rel = values[i]["Relation"].asString();
//			if (rel == "SLURRY_VOLUME") external_relation = ValueName::SLURRY_VOLUME;
//			if (rel == "SLURRY_RATE") external_relation = ValueName::SLURRY_RATE;
//			if (rel == "TIME") external_relation = ValueName::TIME;
//		}
//
//		if (name == "POISSON_RATIO") {
//			auto item = std::make_shared<POISSON_RATIO>(ValueName::POISSON_RATIO, type, val_bounds_num);
//			InitialData.push_back(item);
//		}
//		if (name == "YOUNG_MODULUS") {
//			auto item = std::make_shared<YOUNG_MODULUS>(ValueName::YOUNG_MODULUS, type, val_bounds_num);
//			InitialData.push_back(item);
//		}
//		if (name == "FLUID_LOSS") {
//			auto item = std::make_shared<FLUID_LOSS>(ValueName::FLUID_LOSS, type, val_bounds_num);
//			InitialData.push_back(item);
//		}
//		if (name == "STRESS_MIN") {
//			auto item = std::make_shared<STRESS_MIN>(ValueName::STRESS_MIN, type, val_bounds_num);
//			InitialData.push_back(item);
//		}
//		if (name == "SLURRY_VOLUME") {
//			auto item = std::make_shared<SLURRY_VOLUME>(ValueName::SLURRY_VOLUME, type, val_bounds_num, external_relation);
//			InitialData.push_back(item);
//			relations[ValueName::SLURRY_VOLUME] = external_relation;
//		}
//		if (name == "SLURRY_RATE") {
//			auto item = std::make_shared<SLURRY_RATE>(ValueName::SLURRY_RATE, type, val_bounds_num, external_relation);
//			InitialData.push_back(item);
//			relations[ValueName::SLURRY_RATE] = external_relation;
//		}
//		if (name == "TIME") {
//			auto item = std::make_shared<TIME>(ValueName::TIME, type, val_bounds_num, external_relation);
//			InitialData.push_back(item);
//			relations[ValueName::TIME] = external_relation;
//		}
//		if (name == "PROPANT_CONCENTRATION") {
//			auto item = std::make_shared<PROPANT_CONCENTRATION>(ValueName::PROPANT_CONCENTRATION, type, val_bounds_num);
//			InitialData.push_back(item);
//			relations[ValueName::PROPANT_CONCENTRATION] = ValueName::PROPANT_MASS;
//		}
//		//if (name == "PROPANT_MASS") {
//		//	auto item = std::make_shared<PROPANT_MASS>(ValueName::PROPANT_MASS, type, val_bounds_num);
//		//	InitialData.push_back(item);
//		//	relations[ValueName::PROPANT_MASS] = ValueName::PROPANT_CONCENTRATION;
//		//}
//	}
//
//	//тут должно быть прописан выброс исключения, чтобы не было перекрестных пересчетов
//	if (relations.count(ValueName::PROPANT_CONCENTRATION) > 0
//		&& relations.count(ValueName::PROPANT_MASS) > 0) {
//		throw std::logic_error("You can't recalculate prorrant mass and concetration at the same time");
//	}
//	else {
//		relations.erase(ValueName::PROPANT_CONCENTRATION);
//		relations.erase(ValueName::PROPANT_MASS);
//	}
//	if (relations.count(ValueName::SLURRY_VOLUME) > 0
//		&& relations.count(ValueName::SLURRY_RATE) > 0
//		&& relations.count(ValueName::TIME) > 0) {
//		throw std::logic_error("You can't recalculate slurry rate, volume and time at the same time");
//	}
//	else {
//		if (relations.size() == 2) {
//			if ((*relations.begin()).second != (*prev(relations.end())).second) {
//				throw std::logic_error("You can't recalculate these value(s) in different values");
//			}
//		}
//	}
//
//	return InitialData;
//}

std::vector<VariableValue> Parser::ReadVariableValues() {
	const Json::Value& values = Doc["VariableValue"];
	std::vector<VariableValue> InitialData;
	std::map<ValueName, ValueName> relations;
	for (int i = 0; i < values.size(); ++i) {
		std::string name = values[i]["ValueName"].asString();
		std::string vary_type = values[i]["VaryType"].asString();
		//std::cout << name << ", " << vary_type << std::endl;
		VaryType type;
		std::unordered_map<VaryValue, std::vector<double>> val_bounds_num;
		if (vary_type == "min_max_values") {
			type = VaryType::min_max_values;
			const Json::Value& min = values[i]["min"];
			const Json::Value& max = values[i]["max"];
			std::vector<double> temp1, temp2;
			for (int j = 0; j < min.size(); ++j) {
				temp1.push_back(min[j].asDouble());
				temp2.push_back(max[j].asDouble());
			}
			val_bounds_num[VaryValue::min] = temp1;
			val_bounds_num[VaryValue::max] = temp2;
		}
		else {
			if (vary_type == "coef_lin_trans") {
				type = VaryType::coef_lin_trans;
				const Json::Value& k = values[i]["k"];
				const Json::Value& b = values[i]["b"];
				std::vector<double> temp1, temp2;
				for (int j = 0; j < k.size(); ++j) {
					temp1.push_back(k[j].asDouble());
					temp2.push_back(b[j].asDouble());
				}
				val_bounds_num[VaryValue::k] = temp1;
				val_bounds_num[VaryValue::b] = temp2;
			}
		}

		ValueName external_relation;
		if (values[i].isMember("Relation")) {
			std::string rel = values[i]["Relation"].asString();
			if (rel == "SLURRY_VOLUME") external_relation = ValueName::SLURRY_VOLUME;
			if (rel == "SLURRY_RATE") external_relation = ValueName::SLURRY_RATE;
			if (rel == "TIME") external_relation = ValueName::TIME;
		}

		if (name == "POISSON_RATIO") {
			auto item = VariableValue(ValueName::POISSON_RATIO, type, val_bounds_num, ValueName::NON);
			InitialData.push_back(item);
		}
		if (name == "YOUNG_MODULUS") {
			auto item = VariableValue(ValueName::YOUNG_MODULUS, type, val_bounds_num, ValueName::NON);
			InitialData.push_back(item);
		}
		if (name == "FLUID_LOSS") {
			auto item = VariableValue(ValueName::FLUID_LOSS, type, val_bounds_num, ValueName::NON);
			InitialData.push_back(item);
		}
		if (name == "STRESS_MIN") {
			auto item = VariableValue(ValueName::STRESS_MIN, type, val_bounds_num, ValueName::NON);
			InitialData.push_back(item);
		}
		if (name == "SLURRY_VOLUME") {
			auto item = VariableValue(ValueName::SLURRY_VOLUME, type, val_bounds_num, external_relation);
			InitialData.push_back(item);
			relations[ValueName::SLURRY_VOLUME] = external_relation;
		}
		if (name == "SLURRY_RATE") {
			auto item = VariableValue(ValueName::SLURRY_RATE, type, val_bounds_num, external_relation);
			InitialData.push_back(item);
			relations[ValueName::SLURRY_RATE] = external_relation;
		}
		if (name == "TIME") {
			auto item = VariableValue(ValueName::TIME, type, val_bounds_num, external_relation);
			InitialData.push_back(item);
			relations[ValueName::TIME] = external_relation;
		}
		if (name == "PROPANT_CONCENTRATION") {
			auto item = VariableValue(ValueName::PROPANT_CONCENTRATION, type, val_bounds_num, ValueName::NON);
			InitialData.push_back(item);
			relations[ValueName::PROPANT_CONCENTRATION] = ValueName::PROPANT_MASS;
		}
		if (name == "PROPANT_MASS") {
			auto item = VariableValue(ValueName::PROPANT_MASS, type, val_bounds_num, ValueName::NON);
			InitialData.push_back(item);
			relations[ValueName::PROPANT_MASS] = ValueName::PROPANT_CONCENTRATION;
		}
	}

	//тут должно быть прописан выброс исключения, чтобы не было перекрестных пересчетов
	if (relations.count(ValueName::PROPANT_CONCENTRATION) > 0
		&& relations.count(ValueName::PROPANT_MASS) > 0) {
		throw std::logic_error("You can't recalculate prorrant mass and concetration at the same time");
	}
	else {
		relations.erase(ValueName::PROPANT_CONCENTRATION);
		relations.erase(ValueName::PROPANT_MASS);
	}
	if (relations.count(ValueName::SLURRY_VOLUME) > 0
		&& relations.count(ValueName::SLURRY_RATE) > 0
		&& relations.count(ValueName::TIME) > 0) {
		throw std::logic_error("You can't recalculate slurry rate, volume and time at the same time");
	}
	else {
		if (relations.size() == 2) {
			if ((*relations.begin()).second != (*prev(relations.end())).second) {
				throw std::logic_error("You can't recalculate these value(s) in different values");
			}
		}
	}

	return InitialData;
}

std::map<std::string, double> Parser::ReadRealValues() {
	const Json::Value& values = Doc["RealValues"];
	std::map<std::string, double> res;
	for (int i = 0; i < values.size(); ++i) {
		res[values[i]["Name"].asString()] = values[i]["Value"].asDouble();
	}

	return res;
}

std::map<std::string, double> Parser::ReadRestrictions() {
	const Json::Value& values = Doc["Restrictions"];
	std::map<std::string, double> res;
	for (int i = 0; i < values.size(); ++i) {
		res[values[i]["Name"].asString()] = values[i]["Value"].asDouble();
	}

	return res;
}