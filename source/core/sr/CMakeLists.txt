project(sr)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")
set(INC_DIR "${PROJECT_SOURCE_DIR}/include")

set(HEADERS
    "${INC_DIR}/speech_api_factory.h"
)

set(HEADERS_PRIVATE
    "${SRC_DIR}/stdafx.h"
    "${SRC_DIR}/activity_event_args.h"
    "${SRC_DIR}/activity_session.h"
    "${SRC_DIR}/audio_stream_session.h"
    "${SRC_DIR}/dialog_service_connector.h"
    "${SRC_DIR}/create_module_object.h"
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
    "${SRC_DIR}/connection_event_args.h"
    "${SRC_DIR}/connection_message.h"
    "${SRC_DIR}/connection_message_event_args.h"
    "${SRC_DIR}/session_event_args.h"
    "${SRC_DIR}/usp_reco_engine_adapter.h"
    "${SRC_DIR}/translation_recognizer.h"
    "${SRC_DIR}/audio_buffer.h"
    "${SRC_DIR}/conversation_transcriber.h"
    "${SRC_DIR}/conversation.h"
    "${SRC_DIR}/user.h"
    "${SRC_DIR}/participant.h"
    "${SRC_DIR}/stored_grammar.h"
    "${SRC_DIR}/class_language_model.h"
    "${SRC_DIR}/speech_config.h"
    "${SRC_DIR}/speech_translation_config.h"
    "${SRC_DIR}/language_list_utils.h"
    "${SRC_DIR}/auto_detect_source_lang_config.h"
    "${SRC_DIR}/interaction_id_provider.h"
    "${SRC_DIR}/source_lang_config.h"
    "${SRC_DIR}/participants_mgr.h"
    "${SRC_DIR}/audio_source_notify_me_track_delegate_impl.h"
    "${SRC_DIR}/audio_source_notify_me_tracker.h"
    "${SRC_DIR}/session_audio_source_helper.h"
    "${SRC_DIR}/audio_session_shim.h"
    "${SRC_DIR}/audio_file_logger.h"
    "${SRC_DIR}/speech_audio_processor.h"
    "${SRC_DIR}/output_reco_adapter.h"
    )

set(SOURCES
    "${SRC_DIR}/activity_event_args.cpp"
    "${SRC_DIR}/activity_session.cpp"
    "${SRC_DIR}/audio_stream_session.cpp"
    "${SRC_DIR}/dialog_service_connector.cpp"
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
    "${SRC_DIR}/connection_event_args.cpp"
    "${SRC_DIR}/connection_message.cpp"
    "${SRC_DIR}/connection_message_event_args.cpp"
    "${SRC_DIR}/session_event_args.cpp"
    "${SRC_DIR}/usp_reco_engine_adapter.cpp"
    "${SRC_DIR}/translation_recognizer.cpp"
    "${SRC_DIR}/audio_buffer.cpp"
    "${SRC_DIR}/stdafx.cpp"
    "${SRC_DIR}/conversation_transcriber.cpp"
    "${SRC_DIR}/conversation.cpp"
    "${SRC_DIR}/user.cpp"
    "${SRC_DIR}/participant.cpp"
    "${SRC_DIR}/stored_grammar.cpp"
    "${SRC_DIR}/class_language_model.cpp"
    "${SRC_DIR}/speech_config.cpp"
    "${SRC_DIR}/speech_translation_config.cpp"
    "${SRC_DIR}/language_list_utils.cpp"
    "${SRC_DIR}/auto_detect_source_lang_config.cpp"
    "${SRC_DIR}/source_lang_config.cpp"
    "${SRC_DIR}/participants_mgr.cpp"
    "${SRC_DIR}/audio_session_shim.cpp"
    "${SRC_DIR}/audio_file_logger.cpp"
    "${SRC_DIR}/speech_audio_processor.cpp"
    "${SRC_DIR}/output_reco_adapter.cpp"
    )

source_group("Header Files (public)" FILES ${HEADERS})
source_group("Header Files (private)" FILES ${HEADERS_PRIVATE})
source_group("Source Files" FILES ${SOURCES})

add_library(sr STATIC ${HEADERS} ${HEADERS_PRIVATE} ${SOURCES})

target_include_directories(sr
    PRIVATE
        ## TODO
        ${CARBON_C_API}
        ${CARBON_CXX_API}
        ${CARBON_INCLUDE}
        "${CARBON_EXTERNAL}/json"
        ${COMMON_GENERATED_INCLUDE}
    PUBLIC
        ${INC_DIR}
)

if(WIN32 AND SPECTRE_MITIGATION)
    set_property(TARGET sr PROPERTY VS_GLOBAL_SpectreMitigation "Spectre")
endif()

set_target_properties (sr PROPERTIES
    FOLDER core
)

target_link_libraries(sr PRIVATE usp common pal)

if (MSVC)
   set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "/Yustdafx.h")
   set_source_files_properties(stdafx.cpp PROPERTIES COMPILE_FLAGS "/Ycstdafx.h")
endif(MSVC)