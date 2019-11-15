#include "../include/VariableValue.h"

void VariableValue::CreateValues() {

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