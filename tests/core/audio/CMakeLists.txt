project(audio_tests)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")

include_directories ("${AUDIO}/include")
include_directories ("${CARBON_EXTERNAL}/Catch2")

include_directories(${CARBON_C_API})
include_directories(${CARBON_CXX_API})
include_directories(${CARBON_INCLUDE})

set(SOURCES
    "${SRC_DIR}/main.cpp"
    "${SRC_DIR}/mic_test_case.cpp"
    )

source_group("Source Files" FILES ${SOURCES})

add_executable(audio_tests ${SOURCES})
target_link_libraries(audio_tests audio)

if (NOT WIN32)
    find_package (Threads)
    target_link_libraries (audio_tests ${CMAKE_THREAD_LIBS_INIT})
endif()

set_target_properties (audio_tests PROPERTIES
    FOLDER tests/audio
)