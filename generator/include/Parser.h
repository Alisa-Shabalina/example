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

	std::vector<VariableValue> ReadVariableValues();
	std::map<std::string, double> ReadRealValues();
	std::map<std::string, double> ReadRestrictions();
private:
	Json::Value Doc;

};