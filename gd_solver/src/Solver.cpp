#include "../include/Solver.h"

namespace fs = std::filesystem;

SolverResults Solver::Run() {
	std::system("exe_files\\frtrwell\\frtrwell.exe result.json");

	std::ifstream file_in;
	std::ofstream file_out;
	std::string line;
	file_in.open("fractures_pebi.txt");
	file_out.open("fractures_for_pebi.txt");

	while (!file_in.eof())
	{
		std::getline(file_in, line); // Check getline() doc, you can retrieve a line before/after a given string etc.
		//std::cout << line << '\n';
		std::string new_line = line;
		if (line.find("DESIGN_stage_1") != std::string::npos) {
			new_line = "DESIGN_stage_1 "
				+ std::to_string(2347.4599999999996) + ' '
				+ std::to_string(0.0) + ' '
				+ std::to_string(2064.1);
		} 
		file_out << new_line << std::endl;
	}
	file_in.close();
	file_out.close();
	std::system("exe_files\\pebi\\Console_pebi-2310-0e2e3e6c.exe test_new.data");

	fs::path dir = fs::current_path();
	std::vector<std::string> files_in_dir;
	for (const auto& entry : fs::directory_iterator(dir)) {
		std::string file_name = entry.path().string();
		file_name.erase(begin(file_name), begin(file_name) + dir.string().size() + 1);
		//std::cout << file_name << std::endl;
		if (file_name.find("res0") != std::string::npos) {
			files_in_dir.push_back(file_name);
		}
	}
	
	SolverResults s_res;
	
	std::string folder = files_in_dir[files_in_dir.size() - 1];
	s_res.ReadParamsFromTxt(dir.string() + "\\" + files_in_dir[files_in_dir.size() - 1] + "\\DESIGN\\DESIGN.txt");
	fs::remove_all(files_in_dir[files_in_dir.size() - 1]);

	return s_res;
}