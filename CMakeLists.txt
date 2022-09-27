cmake_minimum_required(VERSION 3.14)

project(SimpleJSONParser)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

option(SIMPLE_JSON_PARSER_BUILD_TESTS "Build tests" ON)

if(SIMPLE_JSON_PARSER_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()