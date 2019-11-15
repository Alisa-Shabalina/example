cmake_minimum_required(VERSION 3.11)

project(fracture_model)

set(CMAKE_CXX_STANDARD 17)

FILE(GLOB HEADERS include/*.h*)
FILE(GLOB SOURCES src/*.c*)
			
add_library(fracture_model STATIC ${HEADERS} ${SOURCES})

link_directories(../lib/${CMAKE_BUILD_TYPE})
			
target_link_libraries(fracture_model jsoncpp)

set_target_properties(fracture_model
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "../lib/"
    LIBRARY_OUTPUT_DIRECTORY "../lib/"
    RUNTIME_OUTPUT_DIRECTORY "../bin/"
)
