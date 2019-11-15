cmake_minimum_required(VERSION 3.11)

project(GRP_paameters_optimization)

set(CMAKE_CXX_STANDARD 17)
# force static runtime libraries for msvc builds
if(MSVC)
    set(variables
            CMAKE_CXX_FLAGS_DEBUG
            CMAKE_CXX_FLAGS_RELEASE
            CMAKE_CXX_FLAGS_RELWITHDEBINFO
            CMAKE_CXX_FLAGS_MINSIZEREL
            )
    foreach(variable ${variables})
        if(${variable} MATCHES "/MD")
            string(REGEX REPLACE "/MD" "/MT" ${variable} "${${variable}}")
        endif()
    endforeach()
endif()

add_subdirectory(jsoncpp)
add_subdirectory(stoch_optimization)
add_subdirectory(fracture_model)
add_subdirectory(gd_solver)
add_subdirectory(generator)
find_package(RapidJSON)

include_directories(jsoncpp)
include_directories(stoch_optimization)
include_directories(fracture_model)
include_directories(gd_solver)
include_directories(generator)
include_directories(RapidJSON)

set(SOURCE_EXE main.cpp)

add_executable(main ${SOURCE_EXE})

target_link_libraries(main jsoncpp)
target_link_libraries(main stoch_optimization)
target_link_libraries(main gd_solver)
target_link_libraries(main fracture_model)
target_link_libraries(main generator)
