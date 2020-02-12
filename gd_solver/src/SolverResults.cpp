#include "../include/SolverResults.h"

void SolverResults::ReadParamsFromTxt(const std::string& path) {
	std::ifstream file(path);
	std::string line;
	while (getline(file, line)) {
		//cout << line << endl;
		std::istringstream iss(line);
		while (iss) {
			double tmp1, tmp2;
			std::vector<double> temp;
			double time, wopt, wopr, wgpt, wgpr, wwpt, wwpr;

			iss >> time;
			//iss.ignore(1);
			Time.push_back(time);
			//iss.ignore(1);

			iss >> wopt;
			iss.ignore(1);
			WOPT.push_back(wopt);
			//iss.ignore(1);
			//iss.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //игнорим все до конца строки
			iss >> wopr;
			iss.ignore(1);
			WOPR.push_back(wopr);
			//iss.ignore(1);
			//
			iss >> wgpt;
			iss.ignore(1);
			WGPT.push_back(wgpt);
			//iss.ignore(1);
			//
			iss >> wgpr;
			iss.ignore(1);
			WGPR.push_back(wgpr);
			//iss.ignore(1);

			//убираем мусор
			iss >> tmp1;
			iss.ignore(1);
			temp.push_back(tmp1);
			iss >> tmp2;
			iss.ignore(1);
			temp.push_back(tmp2);
			//
			iss >> wwpt;
			iss.ignore(1);
			WWPT.push_back(wwpt);
			
			iss >> wwpr;
			iss.ignore(1);
			WWPR.push_back(wwpr);
			//iss.ignore(1);
			iss.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			iss.ignore(1);
			//
			//iss >> four;
			//iss.ignore(1);
			//WWPR.push_back(four);
			//
			//iss >> five;
			//iss.ignore(1);
			//WWIT.push_back(five);
			//
			//iss >> six;
			//iss.ignore(1);
			//WWIR.push_back(six);
			//
			//iss >> seven;
			//iss.ignore(1);
			//WBHP.push_back(seven);

		}
	}

	Time.erase(begin(Time));
	WOPT.erase(begin(WOPT));
	WOPR.erase(begin(WOPR));
	WGPT.erase(begin(WGPT));
	WGPR.erase(begin(WGPR));
	WWPT.erase(begin(WWPT));
	WWPR.erase(begin(WWPR));

}

void SolverResults::PrintTime() {
	for (const auto& item : Time) {
		std::cout << item << '\n';
	}
	std::cout << '\n';
}

void SolverResults::PrintWOPR() {
	for (const auto& item : WOPR) {
		std::cout << item << '\n';
	}
	std::cout << '\n';
}

void SolverResults::PrintWOPT() {
	for (const auto& item : WOPT) {
		std::cout << item << '\n';
	}
	std::cout << '\n';
}