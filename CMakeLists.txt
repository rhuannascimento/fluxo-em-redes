cmake_minimum_required(VERSION 3.14)
project(graph_native)
set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(
  pybind11
  GIT_REPOSITORY https://github.com/pybind/pybind11
  GIT_TAG        v2.11.1
)
FetchContent_MakeAvailable(pybind11)

pybind11_add_module(graph_native project/src/bindings.cpp project/src/graph.cpp project/src/io.cpp)
target_include_directories(graph_native PRIVATE project/include)
set_target_properties(graph_native PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}")
