project(c_api)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")

include_directories(${CARBON_C_API})
include_directories(${CARBON_CXX_API})
include_directories(${CARBON_INCLUDE})
include_directories("${COMMON}/include")
include_directories("${SR}/include")
include_directories("${CARBON_EXTERNAL}/json")

set(HEADERS)

set(HEADERS_PRIVATE
    "${SRC_DIR}/stdafx.h"
    "${SRC_DIR}/event_helpers.h"
    "${SRC_DIR}/handle_helpers.h"
    )

set(SOURCES
    "${SRC_DIR}/event_helpers.cpp"
    "${SRC_DIR}/speechapi_c_audio_config.cpp"
    "${SRC_DIR}/speechapi_c_audio_stream.cpp"
    "${SRC_DIR}/speechapi_c_audio_stream_format.cpp"
    "${SRC_DIR}/speechapi_c_conversation_transcription_result.cpp"
    "${SRC_DIR}/speechapi_c_conversation_transcriber.cpp"
    "${SRC_DIR}/speechapi_c_error.cpp"
    "${SRC_DIR}/speechapi_c_intent_recognizer.cpp"
    "${SRC_DIR}/speechapi_c_intent_result.cpp"
    "${SRC_DIR}/speechapi_c_intent_trigger.cpp"
    "${SRC_DIR}/speechapi_c_grammar.cpp"
    "${SRC_DIR}/speechapi_c_language_understanding_model.cpp"
    "${SRC_DIR}/speechapi_c_keyword_recognition_model.cpp"
    "${SRC_DIR}/speechapi_c_translation_recognizer.cpp"
    "${SRC_DIR}/speechapi_c_translation_result.cpp"
    "${SRC_DIR}/speechapi_c_recognizer.cpp"
    "${SRC_DIR}/speechapi_c_factory.cpp"
    "${SRC_DIR}/speechapi_c_result.cpp"
    "${SRC_DIR}/speechapi_c_session.cpp"
    "${SRC_DIR}/speechapi_c_speech_config.cpp"
    "${SRC_DIR}/speechapi_c_speech_translation_config.cpp"
    "${SRC_DIR}/speechapi_c_property_bag.cpp"
    "${SRC_DIR}/speechapi_c_dialog_service_config.cpp"
    "${SRC_DIR}/speechapi_c_dialog_service_connector.cpp"
    "${SRC_DIR}/speechapi_c_connection.cpp"
    "${SRC_DIR}/speechapi_c_synthesizer.cpp"
    "${SRC_DIR}/speechapi_c_user.cpp"
    "${SRC_DIR}/speechapi_c_participant.cpp"
    "${SRC_DIR}/stdafx.cpp"
    )

source_group("Header Files (public)" FILES ${HEADERS})
source_group("Header Files (private)" FILES ${HEADERS_PRIVATE})
source_group("Source Files" FILES ${SOURCES})

source_group("Speech C API headers" FILES ${SPEECH_C_API_HEADERS})

add_library(c_api OBJECT
    ${SPEECH_C_API_HEADERS}
    ${HEADERS} ${HEADERS_PRIVATE} ${SOURCES}
    )

if(WIN32 AND SPECTRE_MITIGATION)
    set_property(TARGET c_api PROPERTY VS_GLOBAL_SpectreMitigation "Spectre")
endif()

set_target_properties (c_api PROPERTIES
    FOLDER core
    COMPILE_DEFINITIONS "SPX_CONFIG_EXPORTAPIS"
)
