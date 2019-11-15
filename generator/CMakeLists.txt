cmake_minimum_required(VERSION 3.11)

project(generator)

set(CMAKE_CXX_STANDARD 17)

FILE(GLOB HEADERS include/*.h*)
FILE(GLOB SOURCES src/*.c*)
			
add_library(generator STATIC ${HEADERS} ${SOURCES})

link_directories(../lib/${CMAKE_BUILD_TYPE})
			
target_link_libraries(generator jsoncpp)

set_target_properties(generator
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "../lib/"
    LIBRARY_OUTPUT_DIRECTORY "../lib/"
    RUNTIME_OUTPUT_DIRECTORY "../bin/"
)
