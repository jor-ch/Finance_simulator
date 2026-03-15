cmake_minimum_required(VERSION 3.16)
project(Finance_simulator)
set(CMAKE_CXX_STANDARD 17)
set(SOURCES
    src/mainFINSIM.cpp)

set(HEADERS
)
add_executable(Finance_simulator 
${SOURCES}
${HEADERS})
   
