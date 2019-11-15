#include "../include/FracModel.h"

void Pseudo3D::Run(Json::Value data) {
	std::ofstream file_id;
	file_id.open("fracture_solver_init.json");
	Json::StyledWriter styledWriter;
	file_id << styledWriter.write(data);
	file_id.close();
	//std::system("C:\\Users\\shabalina.aa\\Documents\\Диплом\\main\\main.exe fracture_solver_init.json");
	std::system("exe_files\\Pseudo3D\\main.exe fracture_solver_init.json");
}

void Planar3D::Run(Json::Value data) {
	std::ofstream file_id;
	file_id.open("fracture_solver_init.json");
	Json::StyledWriter styledWriter;
	file_id << styledWriter.write(data);
	file_id.close();
	std::system("exe_files\\Planar3D\\Planar3D.exe fracture_solver_init.json");
}