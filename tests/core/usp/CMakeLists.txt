project(usp_tests)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")

include_directories ("${USP}/include")
include_directories ("${CARBON_EXTERNAL}/Catch2")
include_directories("${COMMON}/include")
include_directories(${TEST_INCLUDE})

set(SOURCES
    "${SRC_DIR}/main.cpp"
    "${SRC_DIR}/usp_test_case.cpp"
    )

source_group("Source Files" FILES ${SOURCES})
source_group("Header Files" FILES ${TEST_HEADERS})

add_executable(${PROJECT_NAME} ${SOURCES} ${TEST_HEADERS})
target_link_libraries(${PROJECT_NAME} usp common ${TEST_LIBS})

if (NOT WIN32)
    find_package (Threads)
    target_link_libraries (${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
endif()

set_target_properties (${PROJECT_NAME} PROPERTIES
    FOLDER tests/usp
)