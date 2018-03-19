project(cxx_api_tests)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")

include_directories ("${CARBON_EXTERNAL}/Catch2")

include_directories(${CARBON_C_API})
include_directories(${CARBON_CXX_API})
include_directories("${COMMON}/include")
include_directories(${TEST_INCLUDE})

set(SOURCES
    "${SRC_DIR}/main.cpp"
    "${SRC_DIR}/speech_recognizer_test_case.cpp"
    ${TEST_HEADERS}
    )

source_group("Source Files" FILES ${SOURCES})

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} common libcarbon)

if (NOT WIN32)
    find_package (Threads)
    target_link_libraries (${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
endif()

add_dependencies(${PROJECT_NAME} mock)

set_target_properties (${PROJECT_NAME} PROPERTIES
    FOLDER tests/api
)