cmake_minimum_required(VERSION 3.6)
project(eeMiniproject)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp gate.cpp gate.h gateList.cpp gateList.h gateType.cpp gateType.h cell.cpp cell.h cellList.cpp cellList.h)
add_executable(eeMiniproject ${SOURCE_FILES})