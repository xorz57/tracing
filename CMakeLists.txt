cmake_minimum_required(VERSION 3.30)

project(tracing LANGUAGES CXX)

add_executable(factorial factorial.cpp)
target_compile_features(factorial PRIVATE cxx_std_11)

add_executable(fibonacci fibonacci.cpp)
target_compile_features(fibonacci PRIVATE cxx_std_11)
