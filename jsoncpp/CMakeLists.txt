cmake_minimum_required(VERSION 3.10)

include_directories (include)

add_library (jsoncpp STATIC 
              src/lib_json/json_reader.cpp 
              src/lib_json/json_value.cpp
              src/lib_json/json_writer.cpp )

set_target_properties(jsoncpp
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "../lib/"
    LIBRARY_OUTPUT_DIRECTORY "../lib/"
    RUNTIME_OUTPUT_DIRECTORY "../bin/"
)
