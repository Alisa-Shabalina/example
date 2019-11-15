#include "../include/Recalculator.h"

std::vector<double> Recalculator::GetValuesByKey(Json::Value val, const int& key) {
	std::vector<double> res;
	for (int i = 0; i < val.size(); ++i) {
		double new_val = std::stod(val[i][key].asString());
		res.push_back(new_val);
	}
	return res;
}

std::vector<double> Recalculator::CreateNewValues(const VaryType& type, const std::vector<double>& values, const std::vector<double>& wanted_values) {
	std::vector<double> res(values.size());
	switch (type)
	{
		case VaryType::min_max_values:
		{
			auto min_val = std::min_element(values.begin(), values.end());
			auto max_val = std::max_element(values.begin(), values.end());
			double min_val_new, max_val_new;
			if (wanted_values[0] < wanted_values[1]) {
				min_val_new = wanted_values[0];
				max_val_new = wanted_values[1];
			}
			else {
				min_val_new = wanted_values[1];
				max_val_new = wanted_values[0];
			}
			if (min_val == max_val) {
				for (size_t i = 0; i < res.size(); ++i) {
					res[i] = (min_val_new + max_val_new) / 2.;
				}
			}
			else {
				for (size_t i = 0; i < values.size(); ++i) {
					res[i] = min_val_new + (values[i] - *min_val) / (*max_val - *min_val) * (max_val_new - min_val_new);
				}
			}
			break;
		}

		case VaryType::coef_lin_trans:
		{
			double k = wanted_values[0];
			double b = wanted_values[1];
			for (size_t i = 0; i < values.size(); ++i) {
				res[i] = k * values[i] + b;
			}
			break;
		}
	}
	return res;
}

std::vector<double> Recalculator::ClearFields(const std::vector<double>& input, const std::vector<int>& indexes) {
	std::vector<double> output = input;
	for (const auto& i : indexes) {
		for (size_t j = 0; j < output.size(); ++j) {
			if (i == j) output[j] = 0.;
		}
	}
	return output;
}

Json::Value Recalculator::Recalculate(const std::map<ValueName, std::vector<double>>& point) {
	Json::Value file = BaseFile;
	for (const auto& item : Values) {
		ValueName name = item.GetValueName();
		switch (name) {
			case ValueName::STRESS_MIN: 
			{
				Json::Value& temp = file["Design"]["Stages"][0]["Ports"][0]["ReservoirFormation"];
				int grad_key, stress_key, tvd_top_key, tvd_bot_key;
				for (const auto& id : temp["columnKeys"].getMemberNames()) {
					if (temp["columnKeys"][id].asString() == "TVD_TOP") tvd_top_key = std::stoi(id);
					if (temp["columnKeys"][id].asString() == "TVD_BOT") tvd_bot_key = std::stoi(id);
					if (temp["columnKeys"][id].asString() == "GRAD_STRESS_MIN") grad_key = std::stoi(id);
					if (temp["columnKeys"][id].asString() == "STRESS_MIN") stress_key = std::stoi(id);
				}
				std::vector<double> tvd_top = GetValuesByKey(temp["data"], tvd_top_key);
				std::vector<double> tvd_bot = GetValuesByKey(temp["data"], tvd_bot_key);
				std::vector<double> grad = GetValuesByKey(temp["data"], grad_key);
				std::vector<double> stress_min = GetValuesByKey(temp["data"], stress_key);
				stress_min = CreateNewValues(item.GetVaryType(), stress_min, point.at(name));
				for (size_t i = 0; i < grad.size(); ++i) {
					grad[i] = stress_min[i] / (tvd_top[i] + (tvd_bot[i] - tvd_top[i]) / 2.);
				}
				
				for (int i = 0; i < temp["data"].size(); ++i) {
					temp["data"][i][grad_key] = std::to_string(grad[i]);
					temp["data"][i][stress_key] = std::to_string(stress_min[i]);
				}

				break;
			}

			case ValueName::POISSON_RATIO: 
			{
				Json::Value& temp = file["Design"]["Stages"][0]["Ports"][0]["ReservoirFormation"];
				int poisson_key;
				for (const auto& id : temp["columnKeys"].getMemberNames()) {
					if (temp["columnKeys"][id].asString() == "POISSON_RATIO") poisson_key = std::stoi(id);
				}
				std::vector<double> poisson = GetValuesByKey(temp["data"], poisson_key);
				poisson = CreateNewValues(item.GetVaryType(), poisson, point.at(name));

				for (int i = 0; i < temp["data"].size(); ++i) {
					temp["data"][i][poisson_key] = std::to_string(poisson[i]);
				}

				break;
			}

			case ValueName::FLUID_LOSS:
			{
				Json::Value& temp = file["Design"]["Stages"][0]["Ports"][0]["ReservoirFormation"];
				int loss_key;
				for (const auto& id : temp["columnKeys"].getMemberNames()) {
					if (temp["columnKeys"][id].asString() == "FLUID_LOSS") loss_key = std::stoi(id);
				}
				std::vector<double> fluid_loss = GetValuesByKey(temp["data"], loss_key);
				fluid_loss = CreateNewValues(item.GetVaryType(), fluid_loss, point.at(name));

				for (int i = 0; i < temp["data"].size(); ++i) {
					temp["data"][i][loss_key] = std::to_string(fluid_loss[i]);
				}

				break;
			}

			case ValueName::YOUNG_MODULUS:
			{
				Json::Value& temp = file["Design"]["Stages"][0]["Ports"][0]["ReservoirFormation"];
				int young_key;
				for (const auto& id : temp["columnKeys"].getMemberNames()) {
					if (temp["columnKeys"][id].asString() == "YOUNG_MODULUS") young_key = std::stoi(id);
				}
				std::vector<double> young_modulus = GetValuesByKey(temp["data"], young_key);
				young_modulus = CreateNewValues(item.GetVaryType(), young_modulus, point.at(name));

				for (int i = 0; i < temp["data"].size(); ++i) {
					temp["data"][i][young_key] = std::to_string(young_modulus[i]);
				}

				break;
			}

			case ValueName::SLURRY_VOLUME:
			{
				Json::Value& temp = file["Design"]["Stages"][0]["FracturePumpingSchedule"];
				if (item.GetExternalRelation() == ValueName::TIME) {
					//сначала получим все необходимые значения для пересчета
					int s_v_key, t_s_v_key, time_key, t_time_key, s_r_key, f_v_key, t_f_v_key,
						f_m_key, p_v_key, f_t_key;
					for (const auto& id : temp["columnKeys"].getMemberNames()) {
						if (temp["columnKeys"][id].asString() == "SLURRY_VOLUME") 
							s_v_key = std::stoi(id);
						if (temp["columnKeys"][id].asString() == "TOTAL_SLURRY_VOLUME") 
							t_s_v_key = std::stoi(id);
						if (temp["columnKeys"][id].asString() == "TIME") 
							time_key = std::stoi(id);
						if (temp["columnKeys"][id].asString() == "TOTAL_PUMP_TIME") 
							t_time_key = std::stoi(id);
						if (temp["columnKeys"][id].asString() == "SLURRY_RATE") 
							s_r_key = std::stoi(id);
						if (temp["columnKeys"][id].asString() == "FLUID_VOLUME") 
							f_v_key = std::stoi(id);
						if (temp["columnKeys"][id].asString() == "TOTAL_FLUID_VOLUME") 
							t_f_v_key = std::stoi(id);
						if (temp["columnKeys"][id].asString() == "FLUID_MASS") 
							f_m_key = std::stoi(id);
						if (temp["columnKeys"][id].asString() == "PROPANT_VOLUME") 
							p_v_key = std::stoi(id);
						if (temp["columnKeys"][id].asString() == "FLUID_TYPE") 
							f_t_key = std::stoi(id);
					}
					std::vector<double> slurry_vol = GetValuesByKey(temp["data"], s_v_key);
					std::vector<double> t_slurry_vol = GetValuesByKey(temp["data"], t_s_v_key);
					std::vector<double> time = GetValuesByKey(temp["data"], time_key);
					std::vector<double> t_time = GetValuesByKey(temp["data"], t_time_key);
					std::vector<double> slurry_rate = GetValuesByKey(temp["data"], s_r_key);
					std::vector<double> fluid_vol = GetValuesByKey(temp["data"], f_v_key);
					std::vector<double> t_fluid_vol = GetValuesByKey(temp["data"], t_f_v_key);
					std::vector<double> fluid_mass = GetValuesByKey(temp["data"], f_m_key);
					std::vector<double> propant_vol = GetValuesByKey(temp["data"], p_v_key);
					std::vector<std::string> fluid_type(fluid_vol.size());
					for (int i = 0; i < temp["data"].size(); ++i) {
						std::string new_val = temp["data"][i][f_t_key].asString();
						fluid_type[i]= move(new_val);
					}
					int density_key, key_key;
					for (const auto& id : file["FluidBase"]["columnKeys"].getMemberNames()) {
						if (file["FluidBase"]["columnKeys"][id].asString() == "SPECIFIC_GRAVITY")
							density_key = std::stoi(id);
						if (file["FluidBase"]["columnKeys"][id].asString() == "KEY")
							key_key = std::stoi(id);
					}
					std::vector<double> f_densities(fluid_type.size());
					for (size_t i = 0; i < f_densities.size(); ++i) {
						for (int j = 0; j < file["FluidBase"]["data"].size(); ++j) {
							if (fluid_type[i] == file["FluidBase"]["data"][j][key_key].asString()) {
								f_densities[i] = std::stod(file["FluidBase"]["data"][j][density_key].asString()) * 1000.;
							}
						}
					}
					//пересчитаем саму величину
					slurry_vol = CreateNewValues(item.GetVaryType(), slurry_vol, point.at(name));

					//а теперь все остальное от нее зависящее
					for (size_t i = 0; i < time.size(); ++i) {
						t_slurry_vol[i] = std::accumulate(slurry_vol.begin(), slurry_vol.begin() + i, 0.);
						time[i] = slurry_vol[i] / slurry_rate[i];
						t_time[i] = std::accumulate(time.begin(), time.begin() + i + 1, 0.);
						fluid_vol[i] = slurry_vol[i] - propant_vol[i];
						t_fluid_vol[i] = std::accumulate(fluid_vol.begin(), fluid_vol.begin() + i + 1, 0.);
						fluid_mass[i] = fluid_vol[i] * f_densities[i];
					}

					for (int i = 0; i < temp["data"].size(); ++i) {
						temp["data"][i][s_v_key] = std::to_string(slurry_vol[i]);
						temp["data"][i][t_s_v_key] = std::to_string(t_slurry_vol[i]);
						temp["data"][i][time_key] = std::to_string(time[i]);
						temp["data"][i][t_time_key] = std::to_string(t_time[i]);
						temp["data"][i][f_v_key] = std::to_string(fluid_vol[i]);
						temp["data"][i][t_f_v_key] = std::to_string(t_fluid_vol[i]);
						temp["data"][i][f_m_key] = std::to_string(fluid_mass[i]);
					}
				}
				if (item.GetExternalRelation() == ValueName::SLURRY_RATE) {
					//сначала получим все необходимые значения для пересчета
					int s_v_key, t_s_v_key, time_key, s_r_key, f_v_key, t_f_v_key,
						f_m_key, f_r_key, p_v_key, f_t_key;
					for (const auto& id : temp["columnKeys"].getMemberNames()) {
						if (temp["columnKeys"][id].asString() == "SLURRY_VOLUME")
							s_v_key = std::stoi(id);
						if (temp["columnKeys"][id].asString() == "TOTAL_SLURRY_VOLUME")
							t_s_v_key = std::stoi(id);
						if (temp["columnKeys"][id].asString() == "TIME")
							time_key = std::stoi(id);
						if (temp["columnKeys"][id].asString() == "SLURRY_RATE")
							s_r_key = std::stoi(id);
						if (temp["columnKeys"][id].asString() == "FLUID_VOLUME")
							f_v_key = std::stoi(id);
						if (temp["columnKeys"][id].asString() == "TOTAL_FLUID_VOLUME")
							t_f_v_key = std::stoi(id);
						if (temp["columnKeys"][id].asString() == "FLUID_MASS")
							f_m_key = std::stoi(id);
						if (temp["columnKeys"][id].asString() == "FLUID_RATE")
							f_r_key = std::stoi(id);
						if (temp["columnKeys"][id].asString() == "PROPANT_VOLUME")
							p_v_key = std::stoi(id);
						if (temp["columnKeys"][id].asString() == "FLUID_TYPE")
							f_t_key = std::stoi(id);
					}
					std::vector<double> slurry_vol = GetValuesByKey(temp["data"], s_v_key);
					std::vector<double> t_slurry_vol = GetValuesByKey(temp["data"], t_s_v_key);
					std::vector<double> time = GetValuesByKey(temp["data"], time_key);
					std::vector<double> slurry_rate = GetValuesByKey(temp["data"], s_r_key);
					std::vector<double> fluid_vol = GetValuesByKey(temp["data"], f_v_key);
					std::vector<double> t_fluid_vol = GetValuesByKey(temp["data"], t_f_v_key);
					std::vector<double> fluid_mass = GetValuesByKey(temp["data"], f_m_key);
					std::vector<double> fluid_rate = GetValuesByKey(temp["data"], f_r_key);
					std::vector<double> propant_vol = GetValuesByKey(temp["data"], p_v_key);
					std::vector<std::string> fluid_type(fluid_vol.size());
					for (int i = 0; i < temp["data"].size(); ++i) {
						std::string new_val = temp["data"][i][f_t_key].asString();
						fluid_type[i] = move(new_val);
					}
					int density_key, key_key;
					for (const auto& id : file["FluidBase"]["columnKeys"].getMemberNames()) {
						if (file["FluidBase"]["columnKeys"][id].asString() == "SPECIFIC_GRAVITY")
							density_key = std::stoi(id);
						if (file["FluidBase"]["columnKeys"][id].asString() == "KEY")
							key_key = std::stoi(id);
					}
					std::vector<double> f_densities(fluid_type.size());
					for (size_t i = 0; i < f_densities.size(); ++i) {
						for (int j = 0; j < file["FluidBase"]["data"].size(); ++j) {
							if (fluid_type[i] == file["FluidBase"]["data"][j][key_key].asString()) {
								f_densities[i] = std::stod(file["FluidBase"]["data"][j][density_key].asString()) * 1000.;
							}
						}
					}
					//пересчитаем саму величину
					slurry_vol = CreateNewValues(item.GetVaryType(), slurry_vol, point.at(name));

					//а теперь все остальное от нее зависящее
					for (size_t i = 0; i < time.size(); ++i) {
						t_slurry_vol[i] = std::accumulate(slurry_vol.begin(), slurry_vol.begin() + i + 1, 0.);
						slurry_rate[i] = slurry_vol[i] / time[i];
						fluid_vol[i] = slurry_vol[i] - propant_vol[i];
						t_fluid_vol[i] = std::accumulate(fluid_vol.begin(), fluid_vol.begin() + i + 1, 0.);
						fluid_mass[i] = fluid_vol[i] * f_densities[i];
						fluid_rate[i] = fluid_vol[i] / time[i];
					}

					for (int i = 0; i < temp["data"].size(); ++i) {
						temp["data"][i][s_v_key] = std::to_string(slurry_vol[i]);
						temp["data"][i][t_s_v_key] = std::to_string(t_slurry_vol[i]);
						temp["data"][i][s_r_key] = std::to_string(slurry_rate[i]);
						temp["data"][i][f_v_key] = std::to_string(fluid_vol[i]);
						temp["data"][i][t_f_v_key] = std::to_string(t_fluid_vol[i]);
						temp["data"][i][f_m_key] = std::to_string(fluid_mass[i]);
						temp["data"][i][f_r_key] = std::to_string(fluid_rate[i]);
					}
				}

				break;
			}

			case ValueName::SLURRY_RATE:
			{
				Json::Value& temp = file["Design"]["Stages"][0]["FracturePumpingSchedule"];
				if (item.GetExternalRelation() == ValueName::TIME) {
					//сначала получим все необходимые значения для пересчета
					int s_v_key, time_key, t_time_key, s_r_key, f_v_key, f_r_key;
					for (const auto& id : temp["columnKeys"].getMemberNames()) {
						if (temp["columnKeys"][id].asString() == "SLURRY_VOLUME")
							s_v_key = std::stoi(id);
						if (temp["columnKeys"][id].asString() == "TIME")
							time_key = std::stoi(id);
						if (temp["columnKeys"][id].asString() == "TOTAL_PUMP_TIME")
							t_time_key = std::stoi(id);
						if (temp["columnKeys"][id].asString() == "SLURRY_RATE")
							s_r_key = std::stoi(id);
						if (temp["columnKeys"][id].asString() == "FLUID_VOLUME")
							f_v_key = std::stoi(id);
						if (temp["columnKeys"][id].asString() == "FLUID_RATE")
							f_r_key = std::stoi(id);
					}
					std::vector<double> slurry_vol = GetValuesByKey(temp["data"], s_v_key);
					std::vector<double> time = GetValuesByKey(temp["data"], time_key);
					std::vector<double> t_time = GetValuesByKey(temp["data"], t_time_key);
					std::vector<double> slurry_rate = GetValuesByKey(temp["data"], s_r_key);
					std::vector<double> fluid_vol = GetValuesByKey(temp["data"], f_v_key);
					std::vector<double> fluid_rate = GetValuesByKey(temp["data"], f_r_key);
					//пересчитаем саму величину
					slurry_rate = CreateNewValues(item.GetVaryType(), slurry_rate, point.at(name));

					//а теперь все остальное от нее зависящее
					for (size_t i = 0; i < time.size(); ++i) {
						time[i] = slurry_vol[i] / slurry_rate[i];
						t_time[i] = std::accumulate(time.begin(), time.begin() + i + 1, 0.);
						fluid_rate[i] = fluid_vol[i] / time[i];
					}

					for (int i = 0; i < temp["data"].size(); ++i) {
						temp["data"][i][s_r_key] = std::to_string(slurry_rate[i]);
						temp["data"][i][time_key] = std::to_string(time[i]);
						temp["data"][i][t_time_key] = std::to_string(t_time[i]);
						temp["data"][i][f_r_key] = std::to_string(fluid_rate[i]);
					}
				}

				if (item.GetExternalRelation() == ValueName::SLURRY_VOLUME) {
					//сначала получим все необходимые значения для пересчета
					int s_v_key, t_s_v_key, time_key, s_r_key, f_v_key, t_f_v_key,
						f_m_key, f_r_key, p_v_key, f_t_key;
					for (const auto& id : temp["columnKeys"].getMemberNames()) {
						if (temp["columnKeys"][id].asString() == "SLURRY_VOLUME")
							s_v_key = std::stoi(id);
						if (temp["columnKeys"][id].asString() == "TOTAL_SLURRY_VOLUME")
							t_s_v_key = std::stoi(id);
						if (temp["columnKeys"][id].asString() == "TIME")
							time_key = std::stoi(id);
						if (temp["columnKeys"][id].asString() == "SLURRY_RATE")
							s_r_key = std::stoi(id);
						if (temp["columnKeys"][id].asString() == "FLUID_VOLUME")
							f_v_key = std::stoi(id);
						if (temp["columnKeys"][id].asString() == "TOTAL_FLUID_VOLUME")
							t_f_v_key = std::stoi(id);
						if (temp["columnKeys"][id].asString() == "FLUID_MASS")
							f_m_key = std::stoi(id);
						if (temp["columnKeys"][id].asString() == "FLUID_RATE")
							f_r_key = std::stoi(id);
						if (temp["columnKeys"][id].asString() == "PROPANT_VOLUME")
							p_v_key = std::stoi(id);
						if (temp["columnKeys"][id].asString() == "FLUID_TYPE")
							f_t_key = std::stoi(id);
					}
					std::vector<double> slurry_vol = GetValuesByKey(temp["data"], s_v_key);
					std::vector<double> t_slurry_vol = GetValuesByKey(temp["data"], t_s_v_key);
					std::vector<double> time = GetValuesByKey(temp["data"], time_key);
					std::vector<double> slurry_rate = GetValuesByKey(temp["data"], s_r_key);
					std::vector<double> fluid_vol = GetValuesByKey(temp["data"], f_v_key);
					std::vector<double> t_fluid_vol = GetValuesByKey(temp["data"], t_f_v_key);
					std::vector<double> fluid_mass = GetValuesByKey(temp["data"], f_m_key);
					std::vector<double> fluid_rate = GetValuesByKey(temp["data"], f_r_key);
					std::vector<double> propant_vol = GetValuesByKey(temp["data"], p_v_key);
					std::vector<std::string> fluid_type(fluid_vol.size());
					for (int i = 0; i < temp["data"].size(); ++i) {
						std::string new_val = temp["data"][i][f_t_key].asString();
						fluid_type[i] = move(new_val);
					}
					int density_key, key_key;
					for (const auto& id : file["FluidBase"]["columnKeys"].getMemberNames()) {
						if (file["FluidBase"]["columnKeys"][id].asString() == "SPECIFIC_GRAVITY")
							density_key = std::stoi(id);
						if (file["FluidBase"]["columnKeys"][id].asString() == "KEY")
							key_key = std::stoi(id);
					}
					std::vector<double> f_densities(fluid_type.size());
					for (size_t i = 0; i < f_densities.size(); ++i) {
						for (int j = 0; j < file["FluidBase"]["data"].size(); ++j) {
							if (fluid_type[i] == file["FluidBase"]["data"][j][key_key].asString()) {
								f_densities[i] = std::stod(file["FluidBase"]["data"][j][density_key].asString()) * 1000.;
							}
						}
					}

					
					//пересчитаем саму величину
					slurry_rate = CreateNewValues(item.GetVaryType(), slurry_rate, point.at(name));

					//а теперь все остальное от нее зависящее
					for (size_t i = 0; i < time.size(); ++i) {
						slurry_vol[i] = slurry_rate[i] * time[i];
						t_slurry_vol[i] = std::accumulate(slurry_vol.begin(), slurry_vol.begin() + i + 1, 0.);
						fluid_vol[i] = slurry_vol[i] - propant_vol[i];
						t_fluid_vol[i] = std::accumulate(fluid_vol.begin(), fluid_vol.begin() + i + 1, 0.);
						fluid_mass[i] = fluid_vol[i] * f_densities[i];
						fluid_rate[i] = fluid_vol[i] / time[i];
					}

					//std::cout << "Before\n";
					//std::cout << "Sluury rate: \n";
					//for (int i = 0; i < temp["data"].size(); ++i) {
					//	std::cout << temp["data"][i][s_r_key] << ' ';
					//
					//}
					//std::cout << std::endl;
					//
					//std::cout << "Slurry volume: \n";
					//for (int i = 0; i < temp["data"].size(); ++i) {
					//	std::cout << temp["data"][i][s_v_key] << ' ';
					//}
					//std::cout << std::endl;
					//
					//std::cout << "Total slurry volume: \n";
					//for (int i = 0; i < temp["data"].size(); ++i) {
					//	std::cout << temp["data"][i][t_s_v_key] << ' ';
					//}
					//std::cout << std::endl;
					//
					//std::cout << "Fluid volume: \n";
					//for (int i = 0; i < temp["data"].size(); ++i) {
					//	std::cout << temp["data"][i][f_v_key] << ' ';
					//}
					//std::cout << std::endl;
					//
					//std::cout << "Total fluid volume: \n";
					//for (int i = 0; i < temp["data"].size(); ++i) {
					//	std::cout << temp["data"][i][t_f_v_key] << ' ';
					//}
					//std::cout << std::endl;
					//
					//std::cout << "Fluid mass: \n";
					//for (int i = 0; i < temp["data"].size(); ++i) {
					//	std::cout << temp["data"][i][f_m_key] << ' ';
					//}
					//std::cout << std::endl;
					//
					//std::cout << "Fluid rate: \n";
					//for (int i = 0; i < temp["data"].size(); ++i) {
					//	std::cout << temp["data"][i][f_r_key] << ' ';
					//}
					//std::cout << std::endl;

					for (int i = 0; i < temp["data"].size(); ++i) {
						temp["data"][i][s_r_key] = std::to_string(slurry_rate[i]);
						temp["data"][i][s_v_key] = std::to_string(slurry_vol[i]);
						temp["data"][i][t_s_v_key] = std::to_string(t_slurry_vol[i]);
						temp["data"][i][f_v_key] = std::to_string(fluid_vol[i]);
						temp["data"][i][t_f_v_key] = std::to_string(t_fluid_vol[i]);
						temp["data"][i][f_m_key] = std::to_string(fluid_mass[i]);
						temp["data"][i][f_r_key] = std::to_string(fluid_rate[i]);
					}

					//	std::cout << "After\n";
					//	std::cout << "Sluury rate: \n";
					//	for (int i = 0; i < temp["data"].size(); ++i) {
					//		std::cout << temp["data"][i][s_r_key] << ' ';
					//
					//	}
					//	std::cout << std::endl;
					//
					//	std::cout << "Slurry volume: \n";
					//	for (int i = 0; i < temp["data"].size(); ++i) {
					//		std::cout << temp["data"][i][s_v_key] << ' ';
					//	}
					//	std::cout << std::endl;
					//
					//	std::cout << "Total slurry volume: \n";
					//	for (int i = 0; i < temp["data"].size(); ++i) {
					//		std::cout << temp["data"][i][t_s_v_key] << ' ';
					//	}
					//	std::cout << std::endl;
					//
					//	std::cout << "Fluid volume: \n";
					//	for (int i = 0; i < temp["data"].size(); ++i) {
					//		std::cout << temp["data"][i][f_v_key] << ' ';
					//	}
					//	std::cout << std::endl;
					//
					//	std::cout << "Total fluid volume: \n";
					//	for (int i = 0; i < temp["data"].size(); ++i) {
					//		std::cout << temp["data"][i][t_f_v_key] << ' ';
					//	}
					//	std::cout << std::endl;
					//
					//	std::cout << "Fluid mass: \n";
					//	for (int i = 0; i < temp["data"].size(); ++i) {
					//		std::cout << temp["data"][i][f_m_key] << ' ';
					//	}
					//	std::cout << std::endl;
					//
					//	std::cout << "Fluid rate: \n";
					//	for (int i = 0; i < temp["data"].size(); ++i) {
					//		std::cout << temp["data"][i][f_r_key] << ' ';
					//	}
					//	std::cout << std::endl;
				}



				break;
			}

			case ValueName::TIME:
			{
				Json::Value& temp = file["Design"]["Stages"][0]["FracturePumpingSchedule"];

				if (item.GetExternalRelation() == ValueName::TIME) {
					//сначала получим все необходимые значения для пересчета
					int s_v_key, time_key, t_time_key, s_r_key, f_v_key, f_r_key;
					for (const auto& id : temp["columnKeys"].getMemberNames()) {
						if (temp["columnKeys"][id].asString() == "SLURRY_VOLUME")
							s_v_key = std::stoi(id);
						if (temp["columnKeys"][id].asString() == "TIME")
							time_key = std::stoi(id);
						if (temp["columnKeys"][id].asString() == "TOTAL_PUMP_TIME")
							t_time_key = std::stoi(id);
						if (temp["columnKeys"][id].asString() == "SLURRY_RATE")
							s_r_key = std::stoi(id);
						if (temp["columnKeys"][id].asString() == "FLUID_VOLUME")
							f_v_key = std::stoi(id);
						if (temp["columnKeys"][id].asString() == "FLUID_RATE")
							f_r_key = std::stoi(id);
					}
					std::vector<double> slurry_vol = GetValuesByKey(temp["data"], s_v_key);
					std::vector<double> time = GetValuesByKey(temp["data"], time_key);
					std::vector<double> t_time = GetValuesByKey(temp["data"], t_time_key);
					std::vector<double> slurry_rate = GetValuesByKey(temp["data"], s_r_key);
					std::vector<double> fluid_vol = GetValuesByKey(temp["data"], f_v_key);
					std::vector<double> fluid_rate = GetValuesByKey(temp["data"], f_r_key);
					//пересчитаем саму величину
					time = CreateNewValues(item.GetVaryType(), time, point.at(name));

					//а теперь все остальное от нее зависящее
					for (size_t i = 0; i < time.size(); ++i) {
						slurry_rate[i] = slurry_vol[i] / time[i];
						t_time[i] = std::accumulate(time.begin(), time.begin() + i + 1, 0.);
						fluid_rate[i] = fluid_vol[i] / time[i];
					}

					for (int i = 0; i < temp["data"].size(); ++i) {
						temp["data"][i][time_key] = std::to_string(time[i]);
						temp["data"][i][s_r_key] = std::to_string(slurry_rate[i]);
						temp["data"][i][t_time_key] = std::to_string(t_time[i]);
						temp["data"][i][f_r_key] = std::to_string(fluid_rate[i]);
					}
				}

				if (item.GetExternalRelation() == ValueName::SLURRY_VOLUME) {
					//сначала получим все необходимые значения для пересчета
					int s_v_key, t_s_v_key, time_key, t_time_key, s_r_key, f_v_key, t_f_v_key,
						f_m_key, p_v_key, f_t_key;
					for (const auto& id : temp["columnKeys"].getMemberNames()) {
						if (temp["columnKeys"][id].asString() == "SLURRY_VOLUME")
							s_v_key = std::stoi(id);
						if (temp["columnKeys"][id].asString() == "TOTAL_SLURRY_VOLUME")
							t_s_v_key = std::stoi(id);
						if (temp["columnKeys"][id].asString() == "TIME")
							time_key = std::stoi(id);
						if (temp["columnKeys"][id].asString() == "TOTAL_PUMP_TIME")
							t_time_key = std::stoi(id);
						if (temp["columnKeys"][id].asString() == "SLURRY_RATE")
							s_r_key = std::stoi(id);
						if (temp["columnKeys"][id].asString() == "FLUID_VOLUME")
							f_v_key = std::stoi(id);
						if (temp["columnKeys"][id].asString() == "TOTAL_FLUID_VOLUME")
							t_f_v_key = std::stoi(id);
						if (temp["columnKeys"][id].asString() == "FLUID_MASS")
							f_m_key = std::stoi(id);
						if (temp["columnKeys"][id].asString() == "PROPANT_VOLUME")
							p_v_key = std::stoi(id);
						if (temp["columnKeys"][id].asString() == "FLUID_TYPE")
							f_t_key = std::stoi(id);
					}
					std::vector<double> slurry_vol = GetValuesByKey(temp["data"], s_v_key);
					std::vector<double> t_slurry_vol = GetValuesByKey(temp["data"], t_s_v_key);
					std::vector<double> time = GetValuesByKey(temp["data"], time_key);
					std::vector<double> t_time = GetValuesByKey(temp["data"], t_time_key);
					std::vector<double> slurry_rate = GetValuesByKey(temp["data"], s_r_key);
					std::vector<double> fluid_vol = GetValuesByKey(temp["data"], f_v_key);
					std::vector<double> t_fluid_vol = GetValuesByKey(temp["data"], t_f_v_key);
					std::vector<double> fluid_mass = GetValuesByKey(temp["data"], f_m_key);
					std::vector<double> propant_vol = GetValuesByKey(temp["data"], p_v_key);
					std::vector<std::string> fluid_type(fluid_vol.size());
					for (int i = 0; i < temp["data"].size(); ++i) {
						std::string new_val = temp["data"][i][f_t_key].asString();
						fluid_type[i] = move(new_val);
					}
					int density_key, key_key;
					for (const auto& id : file["FluidBase"]["columnKeys"].getMemberNames()) {
						if (file["FluidBase"]["columnKeys"][id].asString() == "SPECIFIC_GRAVITY")
							density_key = std::stoi(id);
						if (file["FluidBase"]["columnKeys"][id].asString() == "KEY")
							key_key = std::stoi(id);
					}
					std::vector<double> f_densities(fluid_type.size());
					for (size_t i = 0; i < f_densities.size(); ++i) {
						for (int j = 0; j < file["FluidBase"]["data"].size(); ++j) {
							if (fluid_type[i] == file["FluidBase"]["data"][j][key_key].asString()) {
								f_densities[i] = std::stod(file["FluidBase"]["data"][j][density_key].asString()) * 1000.;
							}
						}
					}
					//пересчитаем саму величину
					time = CreateNewValues(item.GetVaryType(), time, point.at(name));

					//а теперь все остальное от нее зависящее
					for (size_t i = 0; i < time.size(); ++i) {
						slurry_vol[i] = slurry_rate[i] * time[i];
						t_slurry_vol[i] = std::accumulate(t_slurry_vol.begin(), t_slurry_vol.begin() + i + 1, 0.);
						t_time[i] = std::accumulate(time.begin(), time.begin() + i, 0.);
						fluid_vol[i] = slurry_vol[i] - propant_vol[i];
						t_fluid_vol[i] = std::accumulate(fluid_vol.begin(), fluid_vol.begin() + i + 1, 0.);
						fluid_mass[i] = fluid_vol[i] * f_densities[i];
					}

					for (int i = 0; i < temp["data"].size(); ++i) {
						temp["data"][i][time_key] = std::to_string(time[i]);
						temp["data"][i][s_v_key] = std::to_string(slurry_vol[i]);
						temp["data"][i][t_s_v_key] = std::to_string(t_slurry_vol[i]);
						temp["data"][i][t_time_key] = std::to_string(t_time[i]);
						temp["data"][i][f_v_key] = std::to_string(fluid_vol[i]);
						temp["data"][i][t_f_v_key] = std::to_string(t_fluid_vol[i]);
						temp["data"][i][f_m_key] = std::to_string(fluid_mass[i]);
					}
				}

				break;
			}

			case ValueName::PROPANT_CONCENTRATION:
			{
				Json::Value& temp = file["Design"]["Stages"][0]["FracturePumpingSchedule"];

				int f_v_key, s_v_key, p_v_key, p_c_s_key, p_c_e_key, p_m_key, p_t_key,
					time_key, f_m_key, f_r_key, p_r_key, t_f_v_key, t_p_m_key, f_t_key;
				for (const auto& id : temp["columnKeys"].getMemberNames()) {
					if (temp["columnKeys"][id].asString() == "FLUID_VOLUME")
						f_v_key = std::stoi(id);
					if (temp["columnKeys"][id].asString() == "SLURRY_VOLUME")
						s_v_key = std::stoi(id);
					if (temp["columnKeys"][id].asString() == "PROPANT_VOLUME")
						p_v_key = std::stoi(id);
					if (temp["columnKeys"][id].asString() == "PROPANT_CONCENTRATION_START")
						p_c_s_key = std::stoi(id);
					if (temp["columnKeys"][id].asString() == "PROPANT_CONCENTRATION_END")
						p_c_e_key = std::stoi(id);
					if (temp["columnKeys"][id].asString() == "PROPANT_MASS")
						p_m_key = std::stoi(id);
					if (temp["columnKeys"][id].asString() == "PROPANT_TYPE")
						p_t_key = std::stoi(id);
					if (temp["columnKeys"][id].asString() == "TIME")
						time_key = std::stoi(id);
					if (temp["columnKeys"][id].asString() == "FLUID_MASS")
						f_m_key = std::stoi(id);
					if (temp["columnKeys"][id].asString() == "FLUID_RATE")
						f_r_key = std::stoi(id);
					if (temp["columnKeys"][id].asString() == "PROPANT_RATE")
						p_r_key = std::stoi(id);
					if (temp["columnKeys"][id].asString() == "TOTAL_FLUID_VOLUME")
						t_f_v_key = std::stoi(id);
					if (temp["columnKeys"][id].asString() == "TOTAL_PROPANT_MASS")
						t_p_m_key = std::stoi(id);
					if (temp["columnKeys"][id].asString() == "FLUID_TYPE")
						f_t_key = std::stoi(id);
				}

				std::vector<double> fluid_vol = GetValuesByKey(temp["data"], f_v_key);
				std::vector<double> slurry_vol = GetValuesByKey(temp["data"], s_v_key);
				std::vector<double> propant_vol = GetValuesByKey(temp["data"], p_v_key);
				std::vector<double> prop_con_s = GetValuesByKey(temp["data"], p_c_s_key);
				std::vector<double> prop_con_e = GetValuesByKey(temp["data"], p_c_e_key);
				std::vector<double> prop_mass = GetValuesByKey(temp["data"], p_m_key);
				std::vector<double> time = GetValuesByKey(temp["data"], time_key);
				std::vector<double> fluid_mass = GetValuesByKey(temp["data"], f_v_key);
				std::vector<double> fluid_rate = GetValuesByKey(temp["data"], f_r_key);
				std::vector<double> prop_rate = GetValuesByKey(temp["data"], p_r_key);
				std::vector<double> t_fluid_vol = GetValuesByKey(temp["data"], t_f_v_key);
				std::vector<double> t_prop_mass = GetValuesByKey(temp["data"], t_p_m_key);

				std::vector<std::string> fluid_type(fluid_vol.size()), prop_type(fluid_vol.size());
				std::vector<int> empty_indexes;
				for (int i = 0; i < temp["data"].size(); ++i) {
					std::string new_val_1 = temp["data"][i][f_t_key].asString();
					fluid_type[i] = move(new_val_1);
					std::string new_val_2 = temp["data"][i][p_t_key].asString();
					if (new_val_2 == "") empty_indexes.push_back(i);
					prop_type[i] = move(new_val_2);
				}

				int f_density_key, f_key_key, p_density_key, p_key_key;
				for (const auto& id : file["FluidBase"]["columnKeys"].getMemberNames()) {
					if (file["FluidBase"]["columnKeys"][id].asString() == "SPECIFIC_GRAVITY")
						f_density_key = std::stoi(id);
					if (file["FluidBase"]["columnKeys"][id].asString() == "KEY")
						f_key_key = std::stoi(id);
				}
				for (const auto& id : file["ProppantBase"]["columnKeys"].getMemberNames()) {
					if (file["ProppantBase"]["columnKeys"][id].asString() == "SPECIFIC_GRAVITY")
						p_density_key = std::stoi(id);
					if (file["ProppantBase"]["columnKeys"][id].asString() == "KEY")
						p_key_key = std::stoi(id);
				}
				std::vector<double> f_densities(fluid_type.size()), p_densities(prop_type.size());
				for (size_t i = 0; i < f_densities.size(); ++i) {
					for (int j = 0; j < file["FluidBase"]["data"].size(); ++j) {
						if (fluid_type[i] == file["FluidBase"]["data"][j][f_key_key].asString()) {
							f_densities[i] = std::stod(file["FluidBase"]["data"][j][f_density_key].asString()) * 1000.;
						}
					}
				}
				for (size_t i = 0; i < p_densities.size(); ++i) {
					for (int j = 0; j < file["ProppantBase"]["data"].size(); ++j) {
						if (prop_type[i] == file["ProppantBase"]["data"][j][p_key_key].asString()) {
							p_densities[i] = std::stod(file["ProppantBase"]["data"][j][p_density_key].asString()) * 1000.;
						}
					}
				}

				p_densities = ClearFields(p_densities, empty_indexes);

				if (item.GetVaryType() == VaryType::min_max_values) {
					double diff_mins = *std::min_element(prop_con_e.begin(), prop_con_e.end()) -
						*std::min_element(prop_con_s.begin(), prop_con_s.end());
					double diff_maxs = *std::max_element(prop_con_e.begin(), prop_con_e.end()) -
						*std::max_element(prop_con_s.begin(), prop_con_s.end());
					std::vector<double> wanted_values = { point.at(name)[0] + diff_mins, point.at(name)[1] + diff_maxs };
					prop_con_s = CreateNewValues(item.GetVaryType(), prop_con_s, point.at(name));
					prop_con_e = CreateNewValues(item.GetVaryType(), prop_con_e, move(wanted_values));
				}
				else {
					prop_con_s = CreateNewValues(item.GetVaryType(), prop_con_s, point.at(name));
					prop_con_e = CreateNewValues(item.GetVaryType(), prop_con_e, point.at(name));
				}

				prop_con_s = ClearFields(prop_con_s, empty_indexes);
				prop_con_e = ClearFields(prop_con_e, empty_indexes);

				for (size_t i = 0; i < propant_vol.size(); ++i) {
					propant_vol[i] = slurry_vol[i] / 2. * (prop_con_s[i] / (p_densities[i] + prop_con_s[i]) +
						prop_con_e[i] / (p_densities[i] + prop_con_e[i]));
					fluid_vol[i] = slurry_vol[i] - propant_vol[i];
					t_fluid_vol[i] = std::accumulate(fluid_vol.begin(), fluid_vol.begin() + i + 1, 0.);
					prop_mass[i] = propant_vol[i] * p_densities[i];
					t_prop_mass[i] = std::accumulate(prop_mass.begin(), prop_mass.begin() + i + 1, 0.);
					fluid_mass[i] = fluid_vol[i] * f_densities[i];
					fluid_rate[i] = fluid_vol[i] / time[i];
					prop_rate[i] = propant_vol[i] / time[i];
				}

				for (int i = 0; i < temp["data"].size(); ++i) {
					temp["data"][i][f_v_key] = std::to_string(fluid_vol[i]);
					temp["data"][i][p_v_key] = std::to_string(propant_vol[i]);
					temp["data"][i][p_c_s_key] = std::to_string(prop_con_s[i]);
					temp["data"][i][p_c_e_key] = std::to_string(prop_con_e[i]);
					temp["data"][i][p_m_key] = std::to_string(prop_mass[i]);
					temp["data"][i][f_m_key] = std::to_string(fluid_mass[i]);
					temp["data"][i][f_r_key] = std::to_string(fluid_rate[i]);
					temp["data"][i][p_r_key] = std::to_string(prop_rate[i]);
					temp["data"][i][t_f_v_key] = std::to_string(t_fluid_vol[i]);
					temp["data"][i][t_p_m_key] = std::to_string(t_prop_mass[i]);
				}

				break;
			}
				
			case ValueName::PROPANT_MASS:
			{
				Json::Value& temp = file["Design"]["Stages"][0]["FracturePumpingSchedule"];

				int f_v_key, s_v_key, p_v_key, p_c_s_key, p_c_e_key, p_m_key, p_t_key,
					time_key, f_m_key, f_r_key, p_r_key, t_f_v_key, t_p_m_key, f_t_key;
				for (const auto& id : temp["columnKeys"].getMemberNames()) {
					if (temp["columnKeys"][id].asString() == "FLUID_VOLUME")
						f_v_key = std::stoi(id);
					if (temp["columnKeys"][id].asString() == "SLURRY_VOLUME")
						s_v_key = std::stoi(id);
					if (temp["columnKeys"][id].asString() == "PROPANT_VOLUME")
						p_v_key = std::stoi(id);
					if (temp["columnKeys"][id].asString() == "PROPANT_CONCENTRATION_START")
						p_c_s_key = std::stoi(id);
					if (temp["columnKeys"][id].asString() == "PROPANT_CONCENTRATION_END")
						p_c_e_key = std::stoi(id);
					if (temp["columnKeys"][id].asString() == "PROPANT_MASS")
						p_m_key = std::stoi(id);
					if (temp["columnKeys"][id].asString() == "PROPANT_TYPE")
						p_t_key = std::stoi(id);
					if (temp["columnKeys"][id].asString() == "TIME")
						time_key = std::stoi(id);
					if (temp["columnKeys"][id].asString() == "FLUID_MASS")
						f_m_key = std::stoi(id);
					if (temp["columnKeys"][id].asString() == "FLUID_RATE")
						f_r_key = std::stoi(id);
					if (temp["columnKeys"][id].asString() == "PROPANT_RATE")
						p_r_key = std::stoi(id);
					if (temp["columnKeys"][id].asString() == "TOTAL_FLUID_VOLUME")
						t_f_v_key = std::stoi(id);
					if (temp["columnKeys"][id].asString() == "TOTAL_PROPANT_MASS")
						t_p_m_key = std::stoi(id);
					if (temp["columnKeys"][id].asString() == "FLUID_TYPE")
						f_t_key = std::stoi(id);
				}

				std::vector<double> fluid_vol = GetValuesByKey(temp["data"], f_v_key);
				std::vector<double> slurry_vol = GetValuesByKey(temp["data"], s_v_key);
				std::vector<double> propant_vol = GetValuesByKey(temp["data"], p_v_key);
				std::vector<double> prop_con_s = GetValuesByKey(temp["data"], p_c_s_key);
				std::vector<double> prop_con_e = GetValuesByKey(temp["data"], p_c_e_key);
				std::vector<double> prop_mass = GetValuesByKey(temp["data"], p_m_key);
				std::vector<double> time = GetValuesByKey(temp["data"], time_key);
				std::vector<double> fluid_mass = GetValuesByKey(temp["data"], f_v_key);
				std::vector<double> fluid_rate = GetValuesByKey(temp["data"], f_r_key);
				std::vector<double> prop_rate = GetValuesByKey(temp["data"], p_r_key);
				std::vector<double> t_fluid_vol = GetValuesByKey(temp["data"], t_f_v_key);
				std::vector<double> t_prop_mass = GetValuesByKey(temp["data"], t_p_m_key);

				std::vector<std::string> fluid_type(fluid_vol.size()), prop_type(fluid_vol.size());
				std::vector<int> empty_indexes;
				for (int i = 0; i < temp["data"].size(); ++i) {
					std::string new_val_1 = temp["data"][i][f_t_key].asString();
					//std::cout << new_val_1 << '\n';
					fluid_type[i] = move(new_val_1);
					std::string new_val_2 = temp["data"][i][p_t_key].asString();
					//std::cout << new_val_2 << '\n';
					if (new_val_2 == "") empty_indexes.push_back(i);
					prop_type[i] = move(new_val_2);
				}

				//for (const auto& item : empty_indexes) {
				//	std::cout << item << ' ';
				//}
				//std::cout << '\n';

				int f_density_key, f_key_key, p_density_key, p_key_key;
				for (const auto& id : file["FluidBase"]["columnKeys"].getMemberNames()) {
					if (file["FluidBase"]["columnKeys"][id].asString() == "SPECIFIC_GRAVITY")
						f_density_key = std::stoi(id);
					if (file["FluidBase"]["columnKeys"][id].asString() == "KEY")
						f_key_key = std::stoi(id);
				}
				for (const auto& id : file["ProppantBase"]["columnKeys"].getMemberNames()) {
					if (file["ProppantBase"]["columnKeys"][id].asString() == "SPECIFIC_GRAVITY")
						p_density_key = std::stoi(id);
					if (file["ProppantBase"]["columnKeys"][id].asString() == "KEY")
						p_key_key = std::stoi(id);
				}
				std::vector<double> f_densities(fluid_type.size()), p_densities(prop_type.size());
				for (size_t i = 0; i < f_densities.size(); ++i) {
					for (int j = 0; j < file["FluidBase"]["data"].size(); ++j) {
						if (fluid_type[i] == file["FluidBase"]["data"][j][f_key_key].asString()) {
							f_densities[i] = std::stod(file["FluidBase"]["data"][j][f_density_key].asString()) * 1000.;
						}
					}
				}

				//f_densities = ClearFieals(f_densities, empty_indexes);
				//for (size_t i = 0; i < f_densities.size(); ++i) {
				//	std::cout << f_densities[i] << ' ';
				//}
				//std::cout << std::endl;

				for (size_t i = 0; i < p_densities.size(); ++i) {
					for (int j = 0; j < file["ProppantBase"]["data"].size(); ++j) {
						if (prop_type[i] == file["ProppantBase"]["data"][j][p_key_key].asString()) {
							p_densities[i] = std::stod(file["ProppantBase"]["data"][j][p_density_key].asString()) * 1000.;
						}
					}
				}

				p_densities = ClearFields(p_densities, empty_indexes);
				//for (size_t i = 0; i < p_densities.size(); ++i) {
				//	std::cout << p_densities[i] << ' ';
				//}
				//std::cout << std::endl;

				prop_mass = CreateNewValues(item.GetVaryType(), prop_mass, point.at(name));
				//for (size_t i = 0; i < prop_mass.size(); ++i) {
				//	std::cout << prop_mass[i] << ' ';
				//}
				//std::cout << std::endl;
				//for (const auto& i : empty_indexes) {
				//	for (size_t j = 0; j < prop_mass.size(); ++j) {
				//		if (i == j) prop_mass[j] = 0.;
				//	}
				//}
				prop_mass = ClearFields(prop_mass, empty_indexes);
				//for (size_t i = 0; i < prop_mass.size(); ++i) {
				//	std::cout << prop_mass[i] << ' ';
				//}
				//std::cout << std::endl;
				for (size_t i = 0; i < prop_con_s.size(); ++i) {
					double prop_con_s_old = prop_con_s[i];
					double prop_con_e_old = prop_con_e[i];
					prop_con_s[i] = prop_mass[i] / slurry_vol[i] - 
						(prop_con_e_old - prop_con_s_old) / 2.;
					prop_con_e[i] = prop_mass[i] / slurry_vol[i] +
						(prop_con_e_old - prop_con_s_old) / 2.;
					propant_vol[i] = slurry_vol[i] / 2. * (prop_con_s[i] / (p_densities[i] + prop_con_s[i]) +
						prop_con_e[i] / (p_densities[i] + prop_con_e[i]));
					//fluid_vol[i] = slurry_vol[i] - propant_vol[i];
					//fluid_mass[i] = fluid_vol[i] * f_densities[i];
					//fluid_rate[i] = fluid_vol[i] / time[i];
					//prop_rate[i] = propant_vol[i] / time[i];
					//t_fluid_vol[i] = std::accumulate(fluid_vol.begin(), fluid_vol.begin() + i, 0.);
					//t_prop_mass[i] = std::accumulate(prop_mass.begin(), prop_mass.begin() + i, 0.);
				}

				prop_con_s = ClearFields(prop_con_s, empty_indexes);
				prop_con_e = ClearFields(prop_con_e, empty_indexes);
				propant_vol = ClearFields(propant_vol, empty_indexes);

				for (size_t i = 0; i < fluid_vol.size(); ++i) {
					fluid_vol[i] = slurry_vol[i] - propant_vol[i];
					fluid_mass[i] = fluid_vol[i] * f_densities[i];
					fluid_rate[i] = fluid_vol[i] / time[i];
					prop_rate[i] = propant_vol[i] / time[i];
					t_fluid_vol[i] = std::accumulate(fluid_vol.begin(), fluid_vol.begin() + i + 1, 0.);
					t_prop_mass[i] = std::accumulate(prop_mass.begin(), prop_mass.begin() + i + 1, 0.);
				}

				//std::cout << "Before\n";
				//std::cout << "Fluid volume: \n";
				//for (int i = 0; i < temp["data"].size(); ++i) {
				//	std::cout << temp["data"][i][f_v_key] << ' ';
				//
				//}
				//std::cout << std::endl;
				//
				//std::cout << "Proppant volume: \n";
				//for (int i = 0; i < temp["data"].size(); ++i) {
				//	std::cout << temp["data"][i][p_v_key] << ' ';
				//}
				//std::cout << std::endl;
				//
				//std::cout << "Proppant conc start: \n";
				//for (int i = 0; i < temp["data"].size(); ++i) {
				//	std::cout << temp["data"][i][p_c_s_key] << ' ';
				//}
				//std::cout << std::endl;
				//
				//std::cout << "Proppant conc end: \n";
				//for (int i = 0; i < temp["data"].size(); ++i) {
				//	std::cout << temp["data"][i][p_c_e_key] << ' ';
				//}
				//std::cout << std::endl;
				//
				//std::cout << "Proppant mass: \n";
				//for (int i = 0; i < temp["data"].size(); ++i) {
				//	std::cout << temp["data"][i][p_m_key] << ' ';
				//}
				//std::cout << std::endl;
				//
				//std::cout << "Fluid mass: \n";
				//for (int i = 0; i < temp["data"].size(); ++i) {
				//	std::cout << temp["data"][i][f_m_key] << ' ';
				//}
				//std::cout << std::endl;
				//
				//std::cout << "Fluid rate: \n";
				//for (int i = 0; i < temp["data"].size(); ++i) {
				//	std::cout << temp["data"][i][f_r_key] << ' ';
				//}
				//std::cout << std::endl;
				//
				//std::cout << "Propppant rate: \n";
				//for (int i = 0; i < temp["data"].size(); ++i) {
				//	std::cout << temp["data"][i][p_r_key] << ' ';
				//}
				//std::cout << std::endl;
				//
				//std::cout << "Total fluid volume: \n";
				//for (int i = 0; i < temp["data"].size(); ++i) {
				//	std::cout << temp["data"][i][t_f_v_key] << ' ';
				//}
				//std::cout << std::endl;
				//
				//std::cout << "Total proppant volume: \n";
				//for (int i = 0; i < temp["data"].size(); ++i) {
				//	std::cout << temp["data"][i][t_p_m_key] << ' ';
				//}
				//std::cout << std::endl;

				for (int i = 0; i < temp["data"].size(); ++i) {
					temp["data"][i][f_v_key] = std::to_string(fluid_vol[i]);
					temp["data"][i][p_v_key] = std::to_string(propant_vol[i]);
					temp["data"][i][p_c_s_key] = std::to_string(prop_con_s[i]);
					temp["data"][i][p_c_e_key] = std::to_string(prop_con_e[i]);
					temp["data"][i][p_m_key] = std::to_string(prop_mass[i]);
					temp["data"][i][f_m_key] = std::to_string(fluid_mass[i]);
					temp["data"][i][f_r_key] = std::to_string(fluid_rate[i]);
					temp["data"][i][p_r_key] = std::to_string(prop_rate[i]);
					temp["data"][i][t_f_v_key] = std::to_string(t_fluid_vol[i]);
					temp["data"][i][t_p_m_key] = std::to_string(t_prop_mass[i]);
				}

				//std::cout << "After\n";
				//std::cout << "Fluid volume: \n";
				//for (int i = 0; i < temp["data"].size(); ++i) {
				//	std::cout << temp["data"][i][f_v_key] << ' ';
				//
				//}
				//std::cout << std::endl;
				//
				//std::cout << "Proppant volume: \n";
				//for (int i = 0; i < temp["data"].size(); ++i) {
				//	std::cout << temp["data"][i][p_v_key] << ' ';
				//}
				//std::cout << std::endl;
				//
				//std::cout << "Proppant conc start: \n";
				//for (int i = 0; i < temp["data"].size(); ++i) {
				//	std::cout << temp["data"][i][p_c_s_key] << ' ';
				//}
				//std::cout << std::endl;
				//
				//std::cout << "Proppant conc end: \n";
				//for (int i = 0; i < temp["data"].size(); ++i) {
				//	std::cout << temp["data"][i][p_c_e_key] << ' ';
				//}
				//std::cout << std::endl;
				//
				//std::cout << "Proppant mass: \n";
				//for (int i = 0; i < temp["data"].size(); ++i) {
				//	std::cout << temp["data"][i][p_m_key] << ' ';
				//}
				//std::cout << std::endl;
				//
				//std::cout << "Fluid mass: \n";
				//for (int i = 0; i < temp["data"].size(); ++i) {
				//	std::cout << temp["data"][i][f_m_key] << ' ';
				//}
				//std::cout << std::endl;
				//
				//std::cout << "Fluid rate: \n";
				//for (int i = 0; i < temp["data"].size(); ++i) {
				//	std::cout << temp["data"][i][f_r_key] << ' ';
				//}
				//std::cout << std::endl;
				//
				//std::cout << "Propppant rate: \n";
				//for (int i = 0; i < temp["data"].size(); ++i) {
				//	std::cout << temp["data"][i][p_r_key] << ' ';
				//}
				//std::cout << std::endl;
				//
				//std::cout << "Total fluid volume: \n";
				//for (int i = 0; i < temp["data"].size(); ++i) {
				//	std::cout << temp["data"][i][t_f_v_key] << ' ';
				//}
				//std::cout << std::endl;
				//
				//std::cout << "Total proppant volume: \n";
				//for (int i = 0; i < temp["data"].size(); ++i) {
				//	std::cout << temp["data"][i][t_p_m_key] << ' ';
				//}
				//std::cout << std::endl;

				break;
			}
			
		}
	}
	return file;
	//std::cout << file << std::endl;
}