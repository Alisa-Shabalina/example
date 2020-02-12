#pragma once

#include <vector>
#include <unordered_map>
#include <map>
#include <algorithm>

enum class ValueName {
	SLURRY_VOLUME,
	SLURRY_RATE,
	TIME,
	PROPANT_CONCENTRATION,
	PROPANT_MASS,
	POISSON_RATIO,
	YOUNG_MODULUS,
	FLUID_LOSS,
	STRESS_MIN,
	FLUID_VOLUME,
	FLUID_TYPE,
	PROPANT_TYPE,
	NON
};

enum class VaryType {
	min_max_values,
	coef_lin_trans, 
	non
};

enum class VaryValue {
	min,
	max,
	k,
	b, 
	non
};

class VariableValue {
public:
	VariableValue(const ValueName& name) : Name(name)
	{
	}
	virtual void CreateValues() {};

	ValueName GetValueName() { return Name; }
	virtual VaryType GetVaryType() = 0;
	virtual std::unordered_map<VaryValue, std::vector<double>> GetValBoundsNum() = 0;
	virtual ValueName GetExternalRelation() = 0;
	virtual std::map<int, std::string> GetIdName() = 0;
	virtual void FillId(std::map<std::string, std::string>& base) = 0;

//protected:
	const ValueName Name;
	//const VaryType Type;
	//const std::unordered_map<VaryValue, std::vector<double>> ValBoundsNum;
	//std::unordered_map<VaryValue, std::vector<double>> Values;
	//const ValueName ExternalRelation;
};

class ContiniousVariableValue : public VariableValue {
public:
	ContiniousVariableValue(const ValueName& name,
		const VaryType& type,
		const std::unordered_map<VaryValue, std::vector<double>>& val_bounds_num,
		const ValueName& external_relation)
		: VariableValue(name),
		Type(type),
		ValBoundsNum(val_bounds_num),
		ExternalRelation(external_relation)
	{
	}

	VaryType GetVaryType() override { return Type; }
	std::unordered_map<VaryValue, std::vector<double>> GetValBoundsNum() override { return ValBoundsNum; }
	ValueName GetExternalRelation() override { return ExternalRelation; }

	void CreateValues() override {

		for (const auto& item : ValBoundsNum) {
			double LowerBound = item.second[0];

			double UpperBound = item.second[1];
			double NumberOfValues = item.second[2];
			if (NumberOfValues == 0) {
				Values[item.first].push_back(0);
			}
			else {
				if (NumberOfValues == 1) {
					double temp = LowerBound + (UpperBound - LowerBound) / 2.;
					Values[item.first].push_back(temp);
				}
				else {
					if (NumberOfValues == 2) {
						Values[item.first].push_back(LowerBound);
						Values[item.first].push_back(UpperBound);
					}
					else {
						double incr = (UpperBound - LowerBound) / NumberOfValues;
						for (int i = 0; i < NumberOfValues - 1; ++i) {
							Values[item.first].push_back(LowerBound + i * incr);
						}
						Values[item.first].push_back(UpperBound);
					}
				}
			}
		}

	}

	std::map<int, std::string> GetIdName() override {
		return std::map<int, std::string>();
	}

	void FillId(std::map<std::string, std::string>& base) override {};
private:
	VaryType Type;
	std::unordered_map<VaryValue, std::vector<double>> ValBoundsNum;
	std::unordered_map<VaryValue, std::vector<double>> Values;
	ValueName ExternalRelation;
};

class CategoricalVariableValue : public VariableValue {
public:
	CategoricalVariableValue(const ValueName& name, 
		const std::vector<std::string>& names) : 
		VariableValue(name),
		Names(names)
	{
	}
	
	void CreateValues() override {
		//Values.resize(IdName.size());
		for (const auto& item : IdName) {
			Values.push_back(item.first);
		}
	}

	VaryType GetVaryType() override { return VaryType::non; }

	std::unordered_map<VaryValue, std::vector<double>> GetValBoundsNum() override {
		return std::unordered_map<VaryValue, std::vector<double>>();
	}

	ValueName GetExternalRelation() override { return ValueName::NON; }

	std::vector<std::string> GetNames() { return Names; }
	std::map<int, std::string> GetIdName() override { return IdName; }
	std::vector<unsigned int> GetValues()  { return Values; }

	void FillId(std::map<std::string, std::string>& base) override {
		//int i = 0;
		//for (const auto& item : base) {
		//	auto iter = std::find(Names.begin(), Names.end(), item.first);
		//	IdName[i] = base[*iter];
		//	i++;
		//}
		
		for (size_t i = 0; i < Names.size(); ++i) {
			IdName.insert({ i, base[Names[i]] });
		}
	}

private:
	std::map<int, std::string> IdName;
	std::vector<std::string> Names;
	std::vector<unsigned int> Values;
};




