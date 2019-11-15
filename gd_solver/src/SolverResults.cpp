#include "../include/SolverResults.h"

void SolverResults::ReadParamsFromTxt(const std::string& path) {
	std::ifstream file(path);
	std::string line;
	while (getline(file, line)) {
		//cout << line << endl;
		std::istringstream iss(line);
		while (iss) {
			double time, one, two, three, four, five, six, seven;

			iss >> time;
			iss.ignore(1);
			Time.push_back(time);

			iss >> one;
			iss.ignore(1);
			WOPT.push_back(one);

			iss >> two;
			iss.ignore(1);
			WOPR.push_back(two);

			iss >> three;
			iss.ignore(1);
			WWPT.push_back(three);

			iss >> four;
			iss.ignore(1);
			WWPR.push_back(four);

			iss >> five;
			iss.ignore(1);
			WWIT.push_back(five);

			iss >> six;
			iss.ignore(1);
			WWIR.push_back(six);

			iss >> seven;
			iss.ignore(1);
			WBHP.push_back(seven);

		}
	}
	//Time.erase(begin(Time));
	//WOPT.erase(begin(WOPT));
	//WOPR.erase(begin(WOPR));

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