cmake_minimum_required(VERSION 3.16)

project(Finance_simulator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#where to find header files
include_directories(include)

#source files
set(SOURCES
    src/mainFINSIM.cpp
    src/Cash.cpp
    src/Stock.cpp)

#create executable
add_executable(Finance_simulator 
${SOURCES})
   
