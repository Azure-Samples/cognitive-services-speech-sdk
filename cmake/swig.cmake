# Download and unpack swig at configure time
configure_file(${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt.swig swig-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/swig-download )
if(result)
  message(FATAL_ERROR "CMake step for swig failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/swig-download )
if(result)
  message(FATAL_ERROR "Build step for swig failed: ${result}")
endif()

set(SWIG_EXECUTABLE ${CMAKE_BINARY_DIR}/swig/swig.exe)
