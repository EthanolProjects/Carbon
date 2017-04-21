cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 14)

Project(Carbon)
add_definitions(-DCARBON_EXPORTS)
IF(WIN32)
    add_definitions(-DCARBON_TARGET_WINDOWS)
ELSEIF(CMAKE_SYSTEM_NAME MATCHES "Linux")
    add_definitions(-DCARBON_TARGET_LINUX)
ELSEIF(Apple)
    add_definitions(-DCARBON_TARGET_APPLE)
ENDIF()

IF(UNIX)
    link_libraries(dl pthread)
ENDIF()

# Build Library
set(INC_CARBON_BASE Carbon)
set(SRC_CARBON_BASE Details)
include(Details/CMakeLists.txt)
include_directories(${INC_CARBON_ALL})
add_library(Carbon SHARED ${SRC_CARBON_ALL})

# Build Tests
FIND_PACKAGE(OpenMP REQUIRED)
if(OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()
add_definitions(-DUNITTESTINC="../Build/UNIX/UnitTestUNIX.h")
add_executable(CarbonTest Tests/Tests.cpp)
target_link_libraries(CarbonTest Carbon gtest)

