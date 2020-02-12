#include "..\include\ObjectiveMain.h"

double Rosenbrock::getCost(std::vector<double> &vect) {
	// Example of using discretize()
	// Corresponding parameters from bo1 are set in Objective.h
	double cost = 0;
	std::vector<double> v = vect;
	int n = v.size();

	//	std::vector<double> vd(v.begin(), v.begin() + n / 2);
	//	discretize(vd, bo1);
	//	std::copy(vd.begin(), vd.end(), v.begin());

	for (int i = 0; i < n - 1; ++i) {
		cost += 100 * pow(v[i + 1] - pow(v[i], 2), 2) + pow((v[i] - 1), 2);
	}
	return cost;
}

double Sphere::getCost(std::vector<double> &vect) {
	double cost = 0;
	for (auto &z : vect)
	{
		cost += z * z;
	}
	cost = sqrt(cost);
	return cost;
}


std::map<ValueName, std::vector<double>> RealCase::ConvertVectorPoint(std::vector<double>& point) {
	std::map<ValueName, std::vector<double>> new_point;
	size_t i = 0;
	for (const auto& value : Values) {
		auto trans = value->GetVaryType();
		if (trans == VaryType::coef_lin_trans) {
			std::vector<double> for_comp(3, 0.);
			if (value->GetValBoundsNum()[VaryValue::b] == for_comp) {
				new_point[value->GetValueName()] = { point[i], 0. };
				i++;
			}
			else {
				new_point[value->GetValueName()] = { point[i], point[i + 1] };
				i += 2;
			}
		}
		if (trans == VaryType::min_max_values) {
			if (point[i] < point[i + 1]) {
				new_point[value->GetValueName()] = { point[i], point[i + 1] };
			}
			else {
				new_point[value->GetValueName()] = { point[i + 1], point[i] };
			}
			i += 2;
		}
		if (trans == VaryType::non) {
			new_point[value->GetValueName()] = { point[i] };
			i++;
		}
	}
	return new_point;
}

double RealCase::SingleOptimization(rapidjson::Value& res) {
	double adapt;
	switch (ToOptimize[0])
	{
	case OptimizingValue::NetPressure:
	{
		rapidjson::SizeType iconst;
		double net_pressure = std::numeric_limits<double>::infinity();
		for (iconst = 0; iconst < res.Size(); ++iconst) {
			rapidjson::Value &temp = res[iconst]["{00000000-0000-0000-0000-000000000000}"]["Ports"][0]["Results"]["accumulated data"];
			if (temp["rate"].GetDouble() == 0.) {
				net_pressure = temp["net pressure"].GetDouble();
				break;
			}
		}
		std::cout << net_pressure << std::endl;
		double val = net_pressure - RealValues["net pressure"];
		adapt = pow(val, 2.) / pow(RealValues["net pressure"], 2.);
		break;
	}

	case OptimizingValue::FluidEfficiency:
	{
		rapidjson::SizeType iconst;
		double fluid_efficiency = std::numeric_limits<double>::infinity();
		for (iconst = 0; iconst < res.Size(); ++iconst) {
			rapidjson::Value &temp = res[iconst]["{00000000-0000-0000-0000-000000000000}"]["Ports"][0]["Results"]["accumulated data"];
			if (temp["rate"].GetDouble() == 0.) {
				fluid_efficiency = temp["fluid efficiency"].GetDouble();
				break;
			}
		}
		std::cout << fluid_efficiency << std::endl;
		double val = fluid_efficiency - RealValues["fluid efficiency"];
		adapt = pow(val, 2.) / pow(RealValues["fluid efficiency"], 2.);
		break;
	}

	case OptimizingValue::ShutInTime:
	{
		rapidjson::SizeType iconst;
		double shut_in_time_start = std::numeric_limits<double>::infinity();
		double shut_in_time_end = std::numeric_limits<double>::infinity();
		for (iconst = 0; iconst < res.Size(); ++iconst) {
			rapidjson::Value &temp = res[iconst]["{00000000-0000-0000-0000-000000000000}"]
				["Ports"][0]["Results"]["accumulated data"];
			if (temp["rate"].GetDouble() == 0.) {
				shut_in_time_start = temp["time"].GetDouble();
				break;
			}
		}
		shut_in_time_end = res[res.Size() - 1]["{00000000-0000-0000-0000-000000000000}"]
			["Ports"][0]["Results"]["accumulated data"]["time"].GetDouble();
		double shut_in_time = shut_in_time_end - shut_in_time_start;
		std::cout << shut_in_time << std::endl;
		double val = shut_in_time - RealValues["shut-in time"];
		adapt = pow(val, 2.) / pow(RealValues["shut-in time"], 2.);
		break;
	}

	case OptimizingValue::WOPT:
	{
		Solver s;
		auto res = s.Run();
		adapt = -res.GetWOPT().back();

		break;
	}

	case OptimizingValue::NPV:
	{
		Solver s;
		auto res = s.Run();
		auto WorkTime = res.GetTime().back();

		//тут конкретный кейс, и в нем плотность нефти 600 кг/куб.м + плюс еще надо интерполировать на каждый день
		auto WOPR = res.GetWOPR(); //тут куб.м/сутки 
		auto WGPR = res.GetWGPR(); //куб.м/сутки
		auto WWPR = res.GetWWPR(); //куб.м/сутки
		WOPR.erase(begin(WOPR));
		WGPR.erase(begin(WGPR));
		WWPR.erase(begin(WWPR));
		std::vector<double> ProductionOil, ProductionGas, ProductionWater;
		for (size_t i = 0; i < WOPR.size(); ++i) {
			std::vector<double> temp1(WorkTime / WOPR.size(), WOPR[i] * 600 / 1000),
				temp2(WorkTime / WGPR.size(), WGPR[i]),
				temp3(WorkTime / WWPR.size(), WWPR[i] * 600 / 1000);
			ProductionOil.insert(end(ProductionOil), begin(temp1), end(temp1));//теперь т/сутки
			ProductionGas.insert(end(ProductionGas), begin(temp2), end(temp2));
			ProductionWater.insert(end(ProductionWater), begin(temp3), end(temp3));
		}

		std::vector<double> PricesOil(WorkTime);
		std::vector<double> PricesGas(WorkTime, 0.);
		for (size_t i = 0; i < WorkTime; ++i) {
			if (i <= 365) {
				PricesOil[i] = 5032 * 600 / 1000; //руб/т
			}
			else {
				PricesOil[i] = 4495 * 600 / 1000;
			}
		}

		double ExpensesBasic = 0., ExpensesRelated = 0., ExpensesUtilizationWater = 0.0; //..., ..., затраты на утилизацию воды
		double ExpensesUtilizationGas = 0., ExpensesTax = 0.; //затраты на утиризацию газа руб/куб.м, налоги на добычу нефти руб/куб.м?
		double ExtractedOil = 0., ExtractedWater = 0., ExtractedGas = 0.0; //извлеченная нефть, вода и газ (в чем? в тоннах?)
		double Revenue = 0., AverageProfit = 0.0; //выручка, полученная в год в рублях?

		//if (ProductionTime.isEmpty())
		//	return;

		//double FracPrice = FracProppants + FracFluids + FracEquipment + FracOther; //цена одной трещины руб? стоимость тонны пропанта * массу пропанта,  
		double FracPrice = 5'000'000 + TotalPropMass * 10'000 + TotalSlurryVolume * 1'000 + 50'000'000;
		double NetProfit = -FracPrice; //затраты на трещину одну

		//double ExpensesBasic = BasicExpensesPerDay * WorkTime;
		//ExpensesRelated = RelatedExpensesOneTime + RelatedExpensesPerDay * WorkTime;

		//PlotTime.clear();
		//NV.clear();
		//NPV.clear();
		//PI.clear();
		//QVector<double> CF;
		//QVector<double> TIME_DAY;
		std::vector<double> CF;
		std::vector<double> NPV;
		std::vector<double> NV;
		std::vector<double> PI;

		for (int i = 0; i < WorkTime; ++i)
		{
			double oP = ProductionOil[i] * PricesOil[i];
			double gP = ProductionGas[i] * PricesGas[i];

			//пока зануляем, а потом будем все это считывать из json файла (если понадобится)
			//double wU = ProductionWater.value(i) * UtilizationWater.value(i);
			//double gU = ProductionGas.value(i) * UtilizationGas.value(i);

			double oT = ProductionOil[i] * 3'000;

			//double exR = (i == 0 ? RelatedExpensesPerDay + RelatedExpensesOneTime : RelatedExpensesPerDay);
			//double ex = BasicExpensesPerDay + exR;

			//double nv = (oP - wU - gU - oT - ex);
			double nv = (oP - oT);
			double tax = 0;
			if (nv > 0)
			{
				tax = nv * (0. / 100);
				nv -= tax;
			}

			ExtractedOil += ProductionOil[i];
			ExtractedWater += ProductionWater[i];
			ExtractedGas += ProductionGas[i];

			ExpensesTax += oT + tax;
			//ExpensesUtilizationWater += wU;
			//ExpensesUtilizationGas += gU;
			//ExpensesBasic += BasicExpensesPerDay;
			//ExpensesRelated += exR;

			//CF << nv;
			CF.push_back(nv);
			NetProfit += nv;
			Revenue += oP + gP;

		}
		AverageProfit = NetProfit / WorkTime * 30.4;

		double DR = pow((1 + 0.2), 720 / 365) - 1;//discountPeriod = 2 года???
		double np_period;
		double npv_period;
		double npv;

		NPV.push_back(-FracPrice);
		NV.push_back(0.0);
		//PI << qQNaN();
		//PlotTime << 0.0;

		int index = 0;
		int it;
		for (int i = 0; i < WorkTime; ++i)
		{
			if (i >= index * 2)
			{
				index++;

				np_period = 0.0;
				it = 0;
				for (int j = i; j < index * 2 && j < WorkTime; ++j) {
					np_period += CF[j];
					it = j;
				}
				npv_period = np_period / pow((1 + DR), index);
				npv = npv_period + NPV.back();
				NPV.push_back(npv);
				PI.push_back(npv / FracPrice);
				NV.push_back(np_period + NV.back());
				//PlotTime << it / discountPeriod;// + 1;
			}
		}

		double DiscountNetProfit = NPV.at(NPV.size() - 1);

		//for (int i = 0; i < PlotTime.size(); ++i) {
		//	TIME_DAY << discountPeriod * PlotTime[i];
		//}

		//ZeroProfitDay = qQNaN();
		//if (NPV.back() >= 0.0)
		//{
		//	MathUtils::LinearInterpolation npvToTimeInterpolation(NPV.toStdVector(), TIME_DAY.toStdVector());
		//	ZeroProfitDay = npvToTimeInterpolation.interpolate(0.0);
		//}
	
		adapt = -NPV.back();

		break;
	}

	case OptimizingValue::FractureLength:
	{
		rapidjson::SizeType iconst;
		double fracture_length = std::numeric_limits<double>::infinity();
		for (iconst = 0; iconst < res.Size(); ++iconst) {
			rapidjson::Value &temp = res[iconst]["{00000000-0000-0000-0000-000000000000}"]["Ports"][0]["Results"];
			if (temp["accumulated data"]["rate"].GetDouble() == 0.) {
				rapidjson::Value &new_temp = temp["geometry"]["branches"][0]["cells"];
				rapidjson::SizeType jconst;
				std::vector<double> cells_indexes_across_x(new_temp.Size());
				double dx;
				for (jconst = 0; jconst < new_temp.Size(); jconst++) {
					if (jconst == 0) dx = new_temp[jconst]["dx"].GetDouble();
					cells_indexes_across_x[jconst] = new_temp[jconst]["i"].GetDouble();
				}
				double max_i = *std::max_element(cells_indexes_across_x.begin(), cells_indexes_across_x.end());
				fracture_length = max_i * dx;
				std::cout << max_i << ' '
					<< dx << ' '
					<< fracture_length << '\n';
				break;
			}
		}
		double val = fracture_length - RealValues["fracture length"];
		adapt = pow(val, 2.) / pow(RealValues["fracture length"], 2.);
		break;
	}
	}
	return adapt;
}

double RealCase::MultiOptimization(rapidjson::Value& res) {
	double adapt;
	if (ToOptimize.size() == 2) {
		if (std::find(begin(ToOptimize), end(ToOptimize), OptimizingValue::NetPressure) != end(ToOptimize) &&
			std::find(begin(ToOptimize), end(ToOptimize), OptimizingValue::FluidEfficiency) != end(ToOptimize)) {
			rapidjson::SizeType iconst;
			double net_pressure = std::numeric_limits<double>::infinity();
			double fluid_efficiency = std::numeric_limits<double>::infinity();
			for (iconst = 0; iconst < res.Size(); ++iconst) {
				rapidjson::Value &temp = res[iconst]["{00000000-0000-0000-0000-000000000000}"]["Ports"][0]["Results"]["accumulated data"];
				if (temp["rate"].GetDouble() == 0.) {
					net_pressure = temp["net pressure"].GetDouble();
					fluid_efficiency = temp["fluid efficiency"].GetDouble();
					break;
				}
			}
			std::cout << net_pressure << std::endl;
			std::cout << fluid_efficiency << std::endl;
			double val1 = net_pressure - RealValues["net pressure"];
			double val2 = fluid_efficiency - RealValues["fluid efficiency"];
			adapt = .5 * pow(val1, 2.) / pow(RealValues["net pressure"], 2.) +
				.5 * pow(val2, 2.) / pow(RealValues["fluid efficiency"], 2.);
		}
	}
	return adapt;
}

double RealCase::getCost(std::vector<double>& point) {
	std::map<ValueName, std::vector<double>> new_point = ConvertVectorPoint(point);
	Recalculator r(Values, InitialData);
	Json::Value new_obj = r.Recalculate(new_point);
	Model->Run(new_obj);

	std::ifstream fs;
	fs.open("result.json");
	rapidjson::IStreamWrapper iwrp(fs);
	rapidjson::Document data;
	data.ParseStream(iwrp);
	rapidjson::Value &res = data;

	double adapt;
	if (ToOptimize.size() == 1) {
		adapt = SingleOptimization(res);
	}
	else {
		adapt = MultiOptimization(res);
	}
	if (!Restrinctions.empty()) {
		if (Restrinctions.count("TVD top") > 0 &&
			Restrinctions.count("TVD bottom") > 0) {
				rapidjson::Value &last = res[res.Size() - 1]["{00000000-0000-0000-0000-000000000000}"]
					["Ports"][0]["Results"]["geometry"]["branches"][0]["cells"];
				std::vector<double> TVDs(last.Size());
				rapidjson::SizeType iconst;
				for (iconst = 0; iconst < last.Size(); ++iconst) {
					TVDs[iconst] = last[iconst]["z"].GetDouble();
				}
				double max_TVD_top = *std::min_element(begin(TVDs), end(TVDs));
				double max_TVD_bottom = *std::max_element(begin(TVDs), end(TVDs));
				//rapidjson::SizeType jconst;
				//for (jconst = 0; jconst < last.Size(); ++jconst) {
				//	if (last[jconst]["i"].GetInt() == 1) {
				//		max_TVD_bottom = last[jconst - 1]["z"].GetDouble();
				//		break;
				//	}
				//}
				std::cout << max_TVD_top << ' ' << max_TVD_bottom << '\n';
				//double penalty1 = std::max(0., Restrinctions["TVD top"] - max_TVD_top);
				//double penalty2 = std::max(0., max_TVD_bottom - Restrinctions["TVD bottom"]);
				//adapt += 10. * pow(penalty1, 2.) +	10. * pow(penalty2, 2.);
		}
		else {
			if (Restrinctions.count("TVD top") > 0) {
				rapidjson::Value &last = res[res.Size() - 1]["{00000000-0000-0000-0000-000000000000}"]
					["Ports"][0]["Results"]["geometry"]["branches"][0]["cells"];
				std::vector<double> TVDs(last.Size());
				rapidjson::SizeType iconst;
				for (iconst = 0; iconst < last.Size(); ++iconst) {
					TVDs[iconst] = last[iconst]["z"].GetDouble();
				}
				double max_TVD_top = *std::min_element(begin(TVDs), end(TVDs));
				double penalty = std::max(0., Restrinctions["TVD top"] - max_TVD_top);
				adapt += 10000 * penalty;
			}
		}
	}
	std::cout << adapt << '\n';
	return adapt;

}

std::map<ValueName, std::vector<double>> SyntheticCase::ConvertVectorPoint(std::vector<double>& point) {
	std::map<ValueName, std::vector<double>> new_point;
	size_t i = 0;
	for (const auto& value : Values) {
		auto trans = value->GetVaryType();
		if (trans == VaryType::coef_lin_trans) {
			std::vector<double> for_comp(3, 0.);
			if (value->GetValBoundsNum()[VaryValue::b] == for_comp) {
				new_point[value->GetValueName()] = { point[i], 0. };
				i++;
			}
			else {
				new_point[value->GetValueName()] = { point[i], point[i + 1] };
				i += 2;
			}
		}
		if (trans == VaryType::min_max_values) {
			if (point[i] < point[i + 1]) {
				new_point[value->GetValueName()] = { point[i], point[i + 1] };
			}
			else {
				new_point[value->GetValueName()] = { point[i + 1], point[i] };
			}
			i += 2;
		}
	}
	return new_point;
}

double SyntheticCase::SingleOptimization(rapidjson::Value& res) {
	double adapt;
	switch (ToOptimize[0])
	{
	case OptimizingValue::NetPressure:
	{
		rapidjson::SizeType iconst;
		double net_pressure = std::numeric_limits<double>::infinity();
		for (iconst = 0; iconst < res.Size(); ++iconst) {
			rapidjson::Value &temp = res[iconst]["{00000000-0000-0000-0000-000000000000}"]["Ports"][0]["Results"]["accumulated data"];
			if (temp["rate"].GetDouble() == 0.) {
				net_pressure = temp["net pressure"].GetDouble();
				break;
			}
		}
		std::cout << net_pressure << std::endl;
		double val = net_pressure - RealValues["net pressure"];
		adapt = pow(val, 2.) / pow(RealValues["net pressure"], 2.);
		break;
	}

	case OptimizingValue::FluidEfficiency:
	{
		rapidjson::SizeType iconst;
		double fluid_efficiency = std::numeric_limits<double>::infinity();
		for (iconst = 0; iconst < res.Size(); ++iconst) {
			rapidjson::Value &temp = res[iconst]["{00000000-0000-0000-0000-000000000000}"]["Ports"][0]["Results"]["accumulated data"];
			if (temp["rate"].GetDouble() == 0.) {
				fluid_efficiency = temp["fluid efficiency"].GetDouble();
				break;
			}
		}
		std::cout << fluid_efficiency << std::endl;
		double val = fluid_efficiency - RealValues["fluid efficiency"];
		adapt = pow(val, 2.) / pow(RealValues["fluid efficiency"], 2.);
		break;
	}

	case OptimizingValue::ShutInTime:
	{
		rapidjson::SizeType iconst;
		double shut_in_time_start = std::numeric_limits<double>::infinity();
		double shut_in_time_end = std::numeric_limits<double>::infinity();
		for (iconst = 0; iconst < res.Size(); ++iconst) {
			rapidjson::Value &temp = res[iconst]["{00000000-0000-0000-0000-000000000000}"]
				["Ports"][0]["Results"]["accumulated data"];
			if (temp["rate"].GetDouble() == 0.) {
				shut_in_time_start = temp["time"].GetDouble();
				break;
			}
		}
		shut_in_time_end = res[res.Size() - 1]["{00000000-0000-0000-0000-000000000000}"]
			["Ports"][0]["Results"]["accumulated data"]["time"].GetDouble();
		double shut_in_time = shut_in_time_end - shut_in_time_start;
		std::cout << shut_in_time << std::endl;
		double val = shut_in_time - RealValues["shut-in time"];
		adapt = pow(val, 2.) / pow(RealValues["shut-in time"], 2.);
		break;
	}

	case OptimizingValue::WOPT:
	{
		Solver s;
		auto res = s.Run();
		adapt = -res.GetWOPT().back();

		break;
	}

	case OptimizingValue::FractureLength:
	{
		rapidjson::SizeType iconst;
		double fracture_length = std::numeric_limits<double>::infinity();
		for (iconst = 0; iconst < res.Size(); ++iconst) {
			rapidjson::Value &temp = res[iconst]["{00000000-0000-0000-0000-000000000000}"]["Ports"][0]["Results"];
			if (temp["accumulated data"]["rate"].GetDouble() == 0.) {
				rapidjson::Value &new_temp = temp["geometry"]["branches"][0]["cells"];
				rapidjson::SizeType jconst;
				std::vector<double> cells_indexes_across_x(new_temp.Size());
				double dx;
				for (jconst = 0; jconst < new_temp.Size(); jconst++) {
					if (jconst == 0) dx = new_temp[jconst]["dx"].GetDouble();
					cells_indexes_across_x[jconst] = new_temp[jconst]["i"].GetDouble();
				}
				double max_i = *std::max_element(cells_indexes_across_x.begin(), cells_indexes_across_x.end());
				fracture_length = max_i * dx;
				std::cout << max_i << ' '
					<< dx << ' '
					<< fracture_length << '\n';
				break;
			}
		}
		double val = fracture_length - RealValues["fracture length"];
		adapt = pow(val, 2.) / pow(RealValues["fracture length"], 2.);
		break;
	}
	}
	return adapt;
}

double SyntheticCase::MultiOptimization(rapidjson::Value& res) {
	double adapt;
	if (ToOptimize.size() == 2) {
		if (std::find(begin(ToOptimize), end(ToOptimize), OptimizingValue::NetPressure) != end(ToOptimize) &&
			std::find(begin(ToOptimize), end(ToOptimize), OptimizingValue::FluidEfficiency) != end(ToOptimize)) {
			rapidjson::SizeType iconst;
			double net_pressure = std::numeric_limits<double>::infinity();
			double fluid_efficiency = std::numeric_limits<double>::infinity();
			for (iconst = 0; iconst < res.Size(); ++iconst) {
				rapidjson::Value &temp = res[iconst]["{00000000-0000-0000-0000-000000000000}"]["Ports"][0]["Results"]["accumulated data"];
				if (temp["rate"].GetDouble() == 0.) {
					net_pressure = temp["net pressure"].GetDouble();
					fluid_efficiency = temp["fluid efficiency"].GetDouble();
					break;
				}
			}
			std::cout << net_pressure << std::endl;
			std::cout << fluid_efficiency << std::endl;
			double val1 = net_pressure - RealValues["net pressure"];
			double val2 = fluid_efficiency - RealValues["fluid efficiency"];
			adapt = .5 * pow(val1, 2.) / pow(RealValues["net pressure"], 2.) +
				.5 * pow(val2, 2.) / pow(RealValues["fluid efficiency"], 2.);
		}
	}
	return adapt;
}

double SyntheticCase::getCost(std::vector<double>& point) {
	std::map<ValueName, std::vector<double>> new_point = ConvertVectorPoint(point);
	Recalculator r(Values, InitialData);
	Json::Value new_obj = r.Recalculate(new_point);
	Model->Run(new_obj);

	std::ifstream fs;
	fs.open("result.json");
	rapidjson::IStreamWrapper iwrp(fs);
	rapidjson::Document data;
	data.ParseStream(iwrp);
	rapidjson::Value &res = data;

	double adapt;
	if (ToOptimize.size() == 1) {
		adapt = SingleOptimization(res);
	}
	else {
		adapt = MultiOptimization(res);
	}
	std::cout << adapt << '\n';
	return adapt;

}