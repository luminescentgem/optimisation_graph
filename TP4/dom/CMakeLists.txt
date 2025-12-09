# cmake -DCPLEX_ROOT_DIR=/opt/ibm/ILOG/CPLEX_Studio2211/ -DCMAKE_MODULE_PATH=.  CMakeLists.txt

cmake_minimum_required(VERSION 3.1...3.23)
project(main)

set(CMAKE_CXX_STANDARD 20)

IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE "RELEASE")
ENDIF()

find_package(CPLEX)
include_directories(SYSTEM ${CPLEX_INCLUDE_DIRS})

macro (add_executable _name)
    _add_executable(${ARGV})
    if (TARGET ${_name})
        set_property(TARGET ${_name} PROPERTY CXX_STANDARD 20)
        target_link_libraries( ${_name} PRIVATE ${CPLEX_LIBRARIES})
        target_link_libraries( ${_name} PRIVATE dl)
    endif()
endmacro()

add_executable (main main.cpp)

