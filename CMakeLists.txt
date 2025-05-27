cmake_minimum_required(VERSION 3.30)

project(tracing LANGUAGES CXX)

add_library(tracing INTERFACE)

target_include_directories(tracing INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/tracing>
  $<INSTALL_INTERFACE:include/tracing>  # <prefix>/include/tracing
)

add_subdirectory(examples)
