cmake_minimum_required(VERSION 3.11)

project(stoch_optimization)

set(CMAKE_CXX_STANDARD 14)

FILE(GLOB HEADERS include/*.h*)
FILE(GLOB SOURCES src/*.c*)
			
add_library(stoch_optimization STATIC ${HEADERS} ${SOURCES})

link_directories(../lib/${CMAKE_BUILD_TYPE})

set_target_properties(stoch_optimization
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "../lib/"
    LIBRARY_OUTPUT_DIRECTORY "../lib/"
    RUNTIME_OUTPUT_DIRECTORY "../bin/"
)
