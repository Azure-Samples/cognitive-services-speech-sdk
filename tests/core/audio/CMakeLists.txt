project(audio_tests)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")

include_directories ("${AUDIO}")
include_directories ("${CARBON_EXTERNAL}/Catch2")

include_directories(${CARBON_C_API})
include_directories(${CARBON_CXX_API})
include_directories(${CARBON_INCLUDE})
include_directories("${COMMON}/include")

set(SOURCES
    "${SRC_DIR}/main.cpp"
    "${SRC_DIR}/mic_test_case.cpp"
    )

source_group("Source Files" FILES ${SOURCES})

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} audio)

if (NOT WIN32)
    find_package (Threads)
    target_link_libraries (${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
endif()

set_target_properties (${PROJECT_NAME} PROPERTIES
    FOLDER tests/audio
)