# guide from https://rix0r.nl/blog/2015/08/13/cmake-guide/

cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

project(ActiveBSP VERSION 0.1 LANGUAGES CXX C)

if (CMAKE_BUILD_TYPE STREQUAL Release)
    add_definitions(-g)
endif()

set (CMAKE_CXX_COMPILE_FLAGS -Wall)

set(ACTIVEBSP_LIB activebsp)

set(ABSP_ENABLE_TESTS ON CACHE BOOL "Enable tests")

if (ABSP_ENABLE_TESTS STREQUAL ON)
    enable_testing ()
endif()

include(GNUInstallDirs)

set(Boost_REALPATH ON)
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

find_package(Boost 1.58.0 REQUIRED COMPONENTS system serialization)
include_directories(${Boost_INCLUDE_DIR})
add_library(boost INTERFACE IMPORTED)

find_package(MPI REQUIRED)
include_directories(SYSTEM ${MPI_INCLUDE_PATH})

message("Found MPI PATH ${MPI_C_LIBRARIES}")

set (CMAKE_CXX_STANDARD 11)

set_property(TARGET boost PROPERTY
    INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR} ${MPI_INCLUDE_PATH})


set(USE_PROVIDED_BSPLIB NewBSPonMPI CACHE STRING
    "Name of a provided BSPLIB implementation to use")

if(USE_PROVIDED_BSPLIB STREQUAL NewBSPonMPI)
    set(BSPLIB_WRAPPER_PATH NewActiveBSPonMPIWrapper CACHE PATH
        "Path to a BSPlib wrapper for ActiveBSP")

    #Currently bsprun does not work, probably requires to compile with bspcc
    set(ABSP_RUNCMD mpirun CACHE STRING
        "Command wrapper to run parallel programs with")

    set(ABSP_RUNARGS "" CACHE STRING
        "Args to always use with the run command")
endif()

option(ENABLE_LOG_MEASURE "print performance measures" OFF)
option(ENABLE_LOG_TRACE "print trace" OFF)

if(ENABLE_LOG_MEASURE)
  message("enabling logging of measure")
  add_definitions(-DENABLE_LOG_MEASURE=1)
endif()

if(ENABLE_LOG_TRACE)
    message("enabling trace")
    add_definitions(-DENABLE_LOG_TRACE=1)
endif()

add_subdirectory(common)

add_subdirectory(${BSPLIB_WRAPPER_PATH} bsplib_wrapper)

add_subdirectory(ActiveBSP)

add_subdirectory(thirdparty/googletest)

set(ABSPRUN_PATH ${CMAKE_CURRENT_BINARY_DIR}/absp_run.sh CACHE PATH "Command to use for running absp programs")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/absp_run.sh.in ${ABSPRUN_PATH} @ONLY)


if (ABSP_ENABLE_TESTS)
    add_subdirectory(tests)
endif()

add_subdirectory(examples)

