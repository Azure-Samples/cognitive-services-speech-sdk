project(sr)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")
set(INC_DIR "${PROJECT_SOURCE_DIR}/include")

include_directories(${CARBON_C_API})
include_directories(${CARBON_CXX_API})
include_directories(${CARBON_INCLUDE})
include_directories("${CARBON_EXTERNAL}/json")

include_directories(${INC_DIR})
include_directories("${COMMON}/include")
include_directories("${USP}/include")
include_directories("${COMMON_GENERATED_INCLUDE}")

set(HEADERS
    )

set(HEADERS_PRIVATE
    "${SRC_DIR}/stdafx.h"
    "${SRC_DIR}/activity.h"
    "${SRC_DIR}/activity_event_args.h"
    "${SRC_DIR}/activity_session.h"
    "${SRC_DIR}/audio_stream_session.h"
    "${SRC_DIR}/dialog_service_connector.h"
    "${SRC_DIR}/connection_event_args.h"
    "${SRC_DIR}/create_module_object.h"
    "${SRC_DIR}/speech_api_factory.h"
    "${SRC_DIR}/intent_recognizer.h"
    "${SRC_DIR}/intent_trigger.h"
    "${SRC_DIR}/keyword_spotter_model.h"
    "${SRC_DIR}/luis_direct_lu_engine_adapter.h"
    "${SRC_DIR}/language_understanding_model.h"
    "${SRC_DIR}/phrase.h"
    "${SRC_DIR}/phrase_list_impl.h"
    "${SRC_DIR}/phrase_list_grammar.h"
    "${SRC_DIR}/recognition_event_args.h"
    "${SRC_DIR}/recognition_result.h"
    "${SRC_DIR}/recognizer.h"
    "${SRC_DIR}/connection.h"
    "${SRC_DIR}/session_event_args.h"
    "${SRC_DIR}/usp_reco_engine_adapter.h"
    "${SRC_DIR}/translation_recognizer.h"
    "${SRC_DIR}/audio_buffer.h"
    "${SRC_DIR}/thread_service.h"
    "${SRC_DIR}/conversation_transcriber.h"
    "${SRC_DIR}/user.h"
    "${SRC_DIR}/participant.h"
    )

set(SOURCES
    "${SRC_DIR}/activity.cpp"
    "${SRC_DIR}/activity_event_args.cpp"
    "${SRC_DIR}/activity_session.cpp"
    "${SRC_DIR}/audio_stream_session.cpp"
    "${SRC_DIR}/dialog_service_connector.cpp"
    "${SRC_DIR}/connection_event_args.cpp"
    "${SRC_DIR}/create_module_object.cpp"
    "${SRC_DIR}/speech_api_factory.cpp"
    "${SRC_DIR}/intent_recognizer.cpp"
    "${SRC_DIR}/intent_trigger.cpp"
    "${SRC_DIR}/keyword_spotter_model.cpp"
    "${SRC_DIR}/luis_direct_lu_engine_adapter.cpp"
    "${SRC_DIR}/language_understanding_model.cpp"
    "${SRC_DIR}/phrase.cpp"
    "${SRC_DIR}/recognition_event_args.cpp"
    "${SRC_DIR}/recognition_result.cpp"
    "${SRC_DIR}/recognizer.cpp"
    "${SRC_DIR}/connection.cpp"
    "${SRC_DIR}/session_event_args.cpp"
    "${SRC_DIR}/usp_reco_engine_adapter.cpp"
    "${SRC_DIR}/translation_recognizer.cpp"
    "${SRC_DIR}/audio_buffer.cpp"
    "${SRC_DIR}/stdafx.cpp"
    "${SRC_DIR}/thread_service.cpp"
    "${SRC_DIR}/conversation_transcriber.cpp"
    "${SRC_DIR}/user.cpp"
    "${SRC_DIR}/participant.cpp"

    )

source_group("Header Files (public)" FILES ${HEADERS})
source_group("Header Files (private)" FILES ${HEADERS_PRIVATE})
source_group("Source Files" FILES ${SOURCES})

add_library(sr STATIC ${HEADERS} ${HEADERS_PRIVATE} ${SOURCES})

if(WIN32 AND SPECTRE_MITIGATION)
    set_property(TARGET sr PROPERTY VS_GLOBAL_SpectreMitigation "Spectre")
endif()

set_target_properties (sr PROPERTIES
    FOLDER core
)

target_link_libraries(sr PRIVATE factory common)
