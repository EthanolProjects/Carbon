cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 14)

Project(Carbon)
add_definitions(-DCARBON_EXPORTS)
IF(UNIX)
    link_libraries(dl pthread)
ENDIF()

include(CTest)

if (CMAKE_VERSION VERSION_LESS 3.2)
    set(UPDATE_DISCONNECTED_IF_AVAILABLE "")
else()
    set(UPDATE_DISCONNECTED_IF_AVAILABLE "UPDATE_DISCONNECTED 1")
endif()

include(DownloadProject.cmake)
download_project(PROJ                googletest
                 GIT_REPOSITORY      https://github.com/google/googletest.git
                 GIT_TAG             master
                 ${UPDATE_DISCONNECTED_IF_AVAILABLE}
)
# Prevent GoogleTest from overriding our compiler/linker options
# when building with Visual Studio
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
# When using CMake 2.8.11 or later, header path dependencies
# are automatically added to the gtest and gmock targets.
# For earlier CMake versions, we have to explicitly add the
# required directories to the header search path ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
    include_directories("${gtest_SOURCE_DIR}/include" "${gmock_SOURCE_DIR}/include")
endif()


# Build Library
set(INC_CARBON_BASE Include)
set(SRC_CARBON_BASE Libraries)
include(Libraries/CMakeLists.txt)
include_directories(${INC_CARBON_ALL})
add_library(Carbon SHARED ${SRC_CARBON_ALL})

# Build Tests
FIND_PACKAGE(OpenMP)
if(OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()
add_definitions(-DUNITTESTINC="../Build/UNIX/UnitTestUNIX.h")
add_executable(CarbonTest Tests/Tests.cpp)
target_link_libraries(CarbonTest Carbon gtest_main)
add_test(NAME CarbonTests COMMAND CarbonTest)
