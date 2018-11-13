project(cxx_api_tests)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")

include_directories ("${CARBON_EXTERNAL}/Catch2")

include_directories(${CARBON_C_API})
include_directories(${CARBON_CXX_API})
include_directories(${CARBON_INCLUDE})
include_directories("${COMMON}/include")
include_directories(${TEST_INCLUDE})
include_directories(${SRC_DIR})

set(SOURCES
    "${SRC_DIR}/main.cpp"
    "${SRC_DIR}/audio_config_tests.cpp"
    "${SRC_DIR}/speech_recognizer_tests.cpp"
    "${SRC_DIR}/intent_recognizer_tests.cpp"
    "${SRC_DIR}/speech_event_signal_tests.cpp"
    "${SRC_DIR}/recognizer_utils.cpp"
    )

source_group("Source Files" FILES ${SOURCES})
source_group("Header Files" FILES ${TEST_HEADERS})

add_executable(${PROJECT_NAME} ${SOURCES} ${TEST_HEADERS})
target_link_libraries(${PROJECT_NAME} common ${SPEECHSDK_CORE_LIBRARY} ${TEST_LIBS})

if (NOT WIN32)
    find_package (Threads)
    target_link_libraries (${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
endif()

add_dependencies(${PROJECT_NAME} mock)

set_target_properties (${PROJECT_NAME} PROPERTIES
    FOLDER tests/api
)
