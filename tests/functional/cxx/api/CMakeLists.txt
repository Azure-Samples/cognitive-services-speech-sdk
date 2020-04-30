project(cxx_api_tests)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")

include_directories("${CARBON_EXTERNAL}/Catch2")
include_directories(${CARBON_C_API})
include_directories(${CARBON_CXX_API})
include_directories(${CARBON_INCLUDE})
include_directories("${COMMON}/include")
include_directories(${TEST_INCLUDE})
include_directories(${SRC_DIR})
include_directories(${USP})
include_directories(${USP}/include)
include_directories("${SR}")
include_directories(${AUDIO})
include_directories("${CARBON_EXTERNAL}/json")
include_directories(${CONVERSATION_TRANSLATION})
include_directories(${CONVERSATION_TRANSLATION}/include)

set(HEADERS_PRIVATE
    "${TEST_INCLUDE}/test_PAL.h"
    "${SRC_DIR}/conversation_translator_utils.h"
    "${SRC_DIR}/dump_log_files_at_end.h"
    )

set(SOURCES
    "${SRC_DIR}/main.cpp"
    "${SRC_DIR}/audio_config_tests.cpp"
    "${SRC_DIR}/speech_recognizer_tests.cpp"
    "${SRC_DIR}/conversation_transcriber_tests.cpp"
    "${SRC_DIR}/conversation_translator_tests.cpp"
    "${SRC_DIR}/speech_synthesizer_tests.cpp"
    "${SRC_DIR}/speech_recognizer_dynamic_grammar_tests.cpp"
    "${SRC_DIR}/intent_recognizer_tests.cpp"
    "${SRC_DIR}/speech_event_signal_tests.cpp"
    "${SRC_DIR}/recognizer_utils.cpp"
    "${SRC_DIR}/synthesizer_utils.cpp"
    "${SRC_DIR}/ring_buffer_tests.cpp"
    "${SRC_DIR}/translation_recognizer_tests.cpp"
    "${SRC_DIR}/dialog_service_connector_tests.cpp"
    "${SRC_DIR}/language_config_tests.cpp"
    "${SRC_DIR}/conversation_tests.cpp"
    "${SRC_DIR}/keyword_recognizer_tests.cpp"
    )

if(EXTENSION_AUDIOCOMPRESSION_ENABLED)
  list(APPEND SOURCES
    "${SRC_DIR}/wire_compression_tests.cpp"
  )
endif()

if(WIN32 AND (NOT SPEECHSDK_OS STREQUAL "WindowsUwp"))
    include_directories("${SRC_DIR}/Windows")
    list(APPEND SOURCES
        "${SRC_DIR}/windows/test_PAL_windows.cpp"
    )
else()
    #add for other platforms here
    include_directories("${SRC_DIR}/Default")
    list(APPEND SOURCES
        "${SRC_DIR}/Default/test_PAL_default.cpp"
    )
endif()

if(BUILD_UNIDEC)
    list(APPEND SOURCES "${SRC_DIR}/speech_recognizer_unidec_tests.cpp")
endif()

source_group("Source Files" FILES ${SOURCES})
source_group("Header Files" FILES ${TEST_HEADERS} ${HEADERS_PRIVATE})

add_executable(${PROJECT_NAME} ${SOURCES} ${TEST_HEADERS} ${HEADERS_PRIVATE})
target_link_libraries(${PROJECT_NAME} ${SPEECHSDK_CORE_LIBRARY} audio common ${TEST_LIBS})

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
if(${use_openssl})
   target_link_libraries(${PROJECT_NAME} z dl)
endif()
endif()

if (NOT WIN32)
    find_package (Threads)
    target_link_libraries (${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
endif()

add_dependencies(${PROJECT_NAME} mock)

set_target_properties (${PROJECT_NAME} PROPERTIES
    FOLDER tests/api
)

if (EXTENSION_CODEC_ENABLED AND STATIC_CODEC_EXTENSION)
  target_link_libraries(${PROJECT_NAME} codec)
endif()

if(EXTENSION_KWS_ENABLED AND STATIC_KWS_EXTENSION)
  target_link_libraries(${PROJECT_NAME} kws_factory)
endif()
