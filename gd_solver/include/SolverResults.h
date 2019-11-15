#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class SolverResults {
public:
	std::vector<double> GetTime() const { return Time; }
	std::vector<double> GetWOPT() const { return WOPT; }
	std::vector<double> GetWOPR() const { return WOPR; }
	void ReadParamsFromTxt(const std::string& path);
	void PrintTime();
	void PrintWOPT();
	void PrintWOPR();
private:
	std::vector<double> Time;
	std::vector<double> WOPT;
	std::vector<double> WOPR;
	std::vector<double> WWPT;
	std::vector<double> WWPR;
	std::vector<double> WWIT;
	std::vector<double> WWIR;
	std::vector<double> WBHP;
};