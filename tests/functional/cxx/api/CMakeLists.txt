project(cxx_api_tests)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")

set(HEADERS_PRIVATE
    "${TEST_INCLUDE}/test_PAL.h"
    "${SRC_DIR}/conversation_translator_utils.h"
    "${SRC_DIR}/dump_log_files_at_end.h"
    "${SRC_DIR}/translator_languages.h"
    "${SRC_DIR}/authentication_token_provider.h"
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
    "${SRC_DIR}/translation_recognizer_tests.cpp"
    "${SRC_DIR}/dialog_service_connector_tests.cpp"
    "${SRC_DIR}/language_config_tests.cpp"
    "${SRC_DIR}/conversation_tests.cpp"
    "${SRC_DIR}/dump_log_files_at_end.cpp"
    "${SRC_DIR}/keyword_recognizer_tests.cpp"
    "${SRC_DIR}/speaker_recognition_tests.cpp"
    "${SRC_DIR}/translator_languages.cpp"
    "${SRC_DIR}/pronunciation_assessment_tests.cpp"
    "${SRC_DIR}/authentication_token_provider.cpp"
    )

if(EXTENSION_AUDIOCOMPRESSION_ENABLED)
  list(APPEND SOURCES
    "${SRC_DIR}/wire_compression_tests.cpp"
  )
endif()

if(WIN32 AND (NOT SPEECHSDK_OS STREQUAL "WindowsUwp"))
    list(APPEND SOURCES
        "${SRC_DIR}/windows/test_PAL_windows.cpp"
    )
else()
    #add for other platforms here
    list(APPEND SOURCES
        "${SRC_DIR}/Default/test_PAL_default.cpp"
    )
endif()

if(BUILD_UNIDEC)
    list(APPEND SOURCES "${SRC_DIR}/speech_recognizer_unidec_tests.cpp")
endif()

if(BUILD_RNNT)
    list(APPEND SOURCES "${SRC_DIR}/speech_recognizer_rnnt_tests.cpp")
endif()

if(BUILD_HYBRID_TTS)
    list(APPEND SOURCES "${SRC_DIR}/speech_synthesizer_hybrid_tests.cpp")
endif()

list(APPEND HEADERS_PRIVATE ${TEST_HEADERS})

set_source_groups()

add_executable(${PROJECT_NAME} ${SOURCES} ${TEST_HEADERS} ${HEADERS_PRIVATE})

set(PRIVATE_AUDIO_HEADERS "${CARBON_CORE}/audio")
set(PRIVATE_CONVERSATION_TRANSLATOR_HEADERS "${CARBON_CORE}/conversation_translation")

target_include_directories(${PROJECT_NAME}
    PRIVATE
        "${CARBON_EXTERNAL}/Catch2"
        "${CARBON_EXTERNAL}/json"
        ${TEST_INCLUDE}
        ${CARBON_C_API}
        ${CARBON_CXX_API}
        ${PRIVATE_AUDIO_HEADERS}
        ${PRIVATE_CONVERSATION_TRANSLATOR_HEADERS}
)

if(WIN32 AND (NOT SPEECHSDK_OS STREQUAL "WindowsUwp"))
    target_include_directories(${PROJECT_NAME} PRIVATE "${SRC_DIR}/Windows")
else()
    target_include_directories(${PROJECT_NAME} PRIVATE "${SRC_DIR}/Default")
endif()

target_link_libraries(${PROJECT_NAME} interfaces ${SPEECHSDK_CORE_LIBRARY} audio common ${TEST_LIBS})

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

if(EXTENSION_RNNT_ENABLED AND STATIC_EMBEDDEDSR_EXTENSION)
  target_link_libraries(${PROJECT_NAME} embedded_sr)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "WindowsStore")
if(use_openssl)
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/ignore:4099 /INCREMENTAL:NO")
endif()
endif()