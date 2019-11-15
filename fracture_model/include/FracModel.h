#pragma once

#define _USE_MATH_DEFINES
#define NOMINMAX

#include "../../jsoncpp/include/json/forwards.h"
#include "../../jsoncpp/include/json/json.h"
#include <iomanip>
#include <fstream>
#include <cmath>

class FracModel {
public:
	FracModel() {};
	virtual ~FracModel() {}
	virtual void Run(Json::Value data) = 0;
	
};

class Pseudo3D : public FracModel {
public:
	void Run(Json::Value data) override;
};

class Planar3D : public FracModel {
public:
	void Run(Json::Value data) override;
};