cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 14)

Project(Carbon)
set(INC_CARBON_BASE Carbon)
set(SRC_CARBON_BASE details)
include(details/CMakeLists.txt)
add_library(Carbon SHARED ${SRC_CARBON_ALL})

