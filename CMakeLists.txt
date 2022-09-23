cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_FLAGS "${CMAKAE_CXX_FLAGS} -Wall -Wextra")

project(Mini_Json_Parser CXX)

add_executable(a.out main.cc)
target_compile_features(a.out PUBLIC cxx_std_20)