#pragma once

#include <vector>
#include <unordered_map>

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
	NON
};

enum class VaryType {
	min_max_values,
	coef_lin_trans
};

enum class VaryValue {
	min,
	max,
	k,
	b
};

class VariableValue {
public:
	VariableValue(const ValueName& name, const VaryType& type, const std::unordered_map<VaryValue, 
		std::vector<double>>& val_bounds_num, const ValueName& external_realtion) 
		: Name(name), Type(type), ValBoundsNum(val_bounds_num), ExternalRelation(external_realtion)
	{
	}
	void CreateValues();

	const ValueName GetValueName() const { return Name; }
	const VaryType GetVaryType() const { return Type; }
	std::unordered_map<VaryValue, std::vector<double>> GetValBoundsNum() const { return ValBoundsNum; }
	const ValueName GetExternalRelation() const { return ExternalRelation; }

private:
	const ValueName Name;
	const VaryType Type;
	const std::unordered_map<VaryValue, std::vector<double>> ValBoundsNum;
	std::unordered_map<VaryValue, std::vector<double>> Values;
	const ValueName ExternalRelation;
};

//class SLURRY_VOLUME : public VariableValue {
//public:
//	SLURRY_VOLUME(const ValueName& name, const VaryType& type, const std::unordered_map<VaryValue, std::vector<double>>& val_bounds_num, const ValueName& external_relation) :
//		VariableValue(name, type, val_bounds_num), ExternalRelation(external_relation)
//	{
//	}
//	ValueName GetExternalRelation() override { return ExternalRelation; }
//private:
//	const ValueName ExternalRelation;
//};
//
//class SLURRY_RATE : public VariableValue {
//public:
//	SLURRY_RATE(const ValueName& name, const VaryType& type, const std::unordered_map<VaryValue, std::vector<double>>& val_bounds_num, const ValueName& external_relation) :
//		VariableValue(name, type, val_bounds_num), ExternalRelation(external_relation)
//	{
//	}
//	ValueName GetExternalRelation() override { return ExternalRelation; }
//private:
//	const ValueName ExternalRelation;
//};
//
//class TIME : public VariableValue {
//public:
//	TIME(const ValueName& name, const VaryType& type, const std::unordered_map<VaryValue, std::vector<double>>& val_bounds_num, const ValueName& external_relation) :
//		VariableValue(name, type, val_bounds_num), ExternalRelation(external_relation)
//	{
//	}
//	ValueName GetExternalRelation() override { return ExternalRelation; }
//private:
//	const ValueName ExternalRelation;
//};
//
//class PROPANT_CONCENTRATION : public VariableValue {
//public:
//	PROPANT_CONCENTRATION(const ValueName& name, const VaryType& type, const std::unordered_map<VaryValue, std::vector<double>>& val_bounds_num) :
//		VariableValue(name, type, val_bounds_num)
//	{
//	}
//};
////
////class PROPANT_MASS : public VariableValue {
////public:
////	PROPANT_MASS(const ValueName& name, const VaryType& type, const std::unordered_map<VaryValue, std::vector<double>>& val_bounds_num) :
////		VariableValue(name, type, val_bounds_num)
////	{
////	}
////};
//
//class POISSON_RATIO : public VariableValue {
//public:
//	POISSON_RATIO(const ValueName& name, const VaryType& type, const std::unordered_map<VaryValue, std::vector<double>>& val_bounds_num) : 
//		VariableValue(name, type, val_bounds_num) 
//	{
//	}
//};
//
//class YOUNG_MODULUS : public VariableValue {
//public:
//	YOUNG_MODULUS(const ValueName& name, const VaryType& type, const std::unordered_map<VaryValue, std::vector<double>>& val_bounds_num) : 
//		VariableValue(name, type, val_bounds_num) 
//	{
//	}
//};
//
//class FLUID_LOSS : public VariableValue {
//public:
//	FLUID_LOSS(const ValueName& name, const VaryType& type, const std::unordered_map<VaryValue, std::vector<double>>& val_bounds_num) : 
//		VariableValue(name, type, val_bounds_num) 
//	{
//	}
//};
//
//class STRESS_MIN : public VariableValue {
//public:
//	STRESS_MIN(const ValueName& name, const VaryType& type, const std::unordered_map<VaryValue, std::vector<double>>& val_bounds_num) : 
//		VariableValue(name, type, val_bounds_num) 
//	{
//	}
//};







