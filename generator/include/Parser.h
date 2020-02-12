#pragma once

#include "VariableValue.h"
#include "../../jsoncpp/include/json/forwards.h"
#include "../../jsoncpp/include/json/json.h"
#include <string>
#include <fstream>
#include <memory>
#include <iostream>
#include <map>
#include <stdexcept>

class Parser {
public:
	Parser(Json::Value doc) : Doc(doc) {}

	std::vector<std::shared_ptr<VariableValue>> ReadVariableValues();
	std::map<std::string, double> ReadRealValues();
	std::map<std::string, double> ReadRestrictions();
	std::map<std::string, std::string> ReadPropantBase();
	std::map<std::string, std::string> ReadFluidBase();

private:
	Json::Value Doc;

};