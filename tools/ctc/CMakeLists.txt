cmake_minimum_required(VERSION 3.8)

project (ctc)

include_directories("${CMAKE_SOURCE_DIR}/../../core/include")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../../bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../../bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../../bin")

add_definitions(-DSPX_CONFIG_TRACE_INFO_WARN_ERR_VERBOSE )
add_executable(ctc ctc.cpp)


