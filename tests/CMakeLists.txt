project(tests)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")
set(TEST_INCLUDE "${PROJECT_SOURCE_DIR}/functional/cxx/include")
set(INPUT_DIR "${SRC_DIR}/input/")

file(GLOB TEST_INPUT ${INPUT_DIR}/**/*)

list(LENGTH TEST_INPUT NUM_TEST_INPUTS)
if(NUM_TEST_INPUTS EQUAL 0)
    execute_process(COMMAND git submodule update --init)
endif()

set(TEST_HEADERS "${TEST_INCLUDE}/test_utils.h")

set(TEST_LIBS)

if(NOT ANDROID)
  # Used to make the Catch2 harness properly emit traces to test run logs
  add_definitions(-DCATCH_CONFIG_EXPERIMENTAL_REDIRECT)

  add_subdirectory("${SRC_DIR}/unit-tests")
  add_subdirectory("${SRC_DIR}/functional/cxx/core")
  add_subdirectory("${SRC_DIR}/functional/cxx/api")
  add_subdirectory("${SRC_DIR}/functional/cxx/uspclientconsole")
if (BUILD_CSHARP)
  add_subdirectory("${SRC_DIR}/functional/csharp/end2end")
  add_subdirectory("${SRC_DIR}/functional/csharp/console")
  add_subdirectory("${SRC_DIR}/functional/csharp/parallel_runner")
endif()
endif()

if (BUILD_JAVA)
add_subdirectory(${JAVA_TESTS})
endif()
