cmake_minimum_required(VERSION 3.11)

project(gd_solver)

set(CMAKE_CXX_STANDARD 17)

FILE(GLOB HEADERS include/*.h*)
FILE(GLOB SOURCES src/*.c*)
			
add_library(gd_solver STATIC ${HEADERS} ${SOURCES})

link_directories(../lib/${CMAKE_BUILD_TYPE})
			
#target_link_libraries(gd_solver jsoncpp)

set_target_properties(gd_solver
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "../lib/"
    LIBRARY_OUTPUT_DIRECTORY "../lib/"
    RUNTIME_OUTPUT_DIRECTORY "../bin/"
)
