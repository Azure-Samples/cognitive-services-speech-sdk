project(cxx_api_tests)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")

include_directories ("${CARBON_EXTERNAL}/Catch2")
include_directories(${CARBON_C_API})
include_directories(${CARBON_CXX_API})
include_directories(${CARBON_INCLUDE})
include_directories("${COMMON}/include")
include_directories(${TEST_INCLUDE})
include_directories(${SRC_DIR})
include_directories(${USP})
include_directories(${AUDIO})
include_directories ("${CARBON_EXTERNAL}/json")

set(SOURCES
    "${SRC_DIR}/main.cpp"
    "${SRC_DIR}/audio_config_tests.cpp"
    "${SRC_DIR}/speech_recognizer_tests.cpp"
    "${SRC_DIR}/conversation_transcriber_tests.cpp"
    "${SRC_DIR}/speech_synthesizer_tests.cpp"
    "${SRC_DIR}/speech_recognizer_dynamic_grammar_tests.cpp"
    "${SRC_DIR}/intent_recognizer_tests.cpp"
    "${SRC_DIR}/speech_event_signal_tests.cpp"
    "${SRC_DIR}/recognizer_utils.cpp"
    "${SRC_DIR}/synthesizer_utils.cpp"
    "${SRC_DIR}/ring_buffer_tests.cpp"
    "${SRC_DIR}/bot_connector_tests.cpp"
    )

if(BUILD_UNIDEC)
    list(APPEND SOURCES "${SRC_DIR}/speech_recognizer_unidec_tests.cpp")
endif()

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
