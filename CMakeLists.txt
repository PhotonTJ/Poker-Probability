cmake_minimum_required(VERSION 3.15)
project(poker_probability CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
# The engine is a static library that also gets linked into the pybind11
# shared module, so it must be built as position-independent code either way.
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(POKER_BUILD_TESTS "Build the doctest unit test binary" ON)
option(POKER_BUILD_PYTHON_BINDINGS "Build the pybind11 Python extension module" OFF)

add_subdirectory(engine)
