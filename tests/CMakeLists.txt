project(tests)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")
set(TEST_INCLUDE "${PROJECT_SOURCE_DIR}/functional/cxx/include")
set(SCRIPTS_DIR "${SRC_DIR}/scripts/")
set(INPUT_DIR "${SRC_DIR}/input/")

file(GLOB SCRIPTS ${SCRIPTS_DIR}/*)
foreach(script_file ${SCRIPTS})
    file(COPY ${script_file}
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endforeach()

file(GLOB TEST_INPUT ${INPUT_DIR}/**/*)

list(LENGTH TEST_INPUT NUM_TEST_INPUTS)
if(NUM_TEST_INPUTS EQUAL 0)
    execute_process(COMMAND git submodule update --init)
endif()

file(GLOB TEST_INPUT ${INPUT_DIR}/**/*)
foreach(input_file ${TEST_INPUT})
    file(COPY ${input_file}
        DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/input/")
endforeach()

set(TEST_HEADERS "${TEST_INCLUDE}/test_utils.h")

set(TEST_LIBS)

if(NOT ANDROID)
  add_subdirectory("${SRC_DIR}/functional/cxx/core/audio")
  add_subdirectory("${SRC_DIR}/functional/cxx/core/usp")
  add_subdirectory("${SRC_DIR}/functional/cxx/api")
  add_subdirectory("${SRC_DIR}/functional/cxx/uspclientconsole")
  add_subdirectory("${SRC_DIR}/functional/csharp/end2end")
  add_subdirectory("${SRC_DIR}/functional/csharp/console")
endif()

add_subdirectory(${JAVA_TESTS})
