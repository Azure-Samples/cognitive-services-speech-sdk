project(mock)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")

set(HEADERS)

set(HEADERS_PRIVATE
    "${SRC_DIR}/stdafx.h"
    "${SRC_DIR}/create_module_object.h"
    "${SRC_DIR}/mock_audio_file_impl.h"
    "${SRC_DIR}/mock_audio_reader.h"
    "${SRC_DIR}/mock_audio_reader_impl.h"
    "${SRC_DIR}/mock_interactive_microphone.h"
    "${SRC_DIR}/mock_kws_engine_adapter.h"
    "${SRC_DIR}/mock_lu_engine_adapter.h"
    "${SRC_DIR}/mock_reco_engine_adapter.h"
    "${SRC_DIR}/mock_wav_file_reader.h"
    "${SRC_DIR}/mock_wav_file_pump.h"
    )

set(SOURCES
    "${SRC_DIR}/create_module_object.cpp"
    "${SRC_DIR}/mock_audio_reader_impl.cpp"
    "${SRC_DIR}/mock_interactive_microphone.cpp"
    "${SRC_DIR}/mock_kws_engine_adapter.cpp"
    "${SRC_DIR}/mock_lu_engine_adapter.cpp"
    "${SRC_DIR}/mock_reco_engine_adapter.cpp"
    "${SRC_DIR}/mock_wav_file_pump.cpp"
    "${SRC_DIR}/stdafx.cpp"
    )

    set_source_groups()

add_library(mock SHARED ${HEADERS} ${HEADERS_PRIVATE} ${SOURCES})

target_include_directories(mock
    PRIVATE
        ${CARBON_C_API}
        ${CARBON_CXX_API}
        "${CARBON_EXTERNAL}/json"
)

if(WIN32 AND SPECTRE_MITIGATION)
    set_property(TARGET mock PROPERTY VS_GLOBAL_SpectreMitigation "Spectre")
endif()

set_target_properties (mock PROPERTIES
    DEFINE_SYMBOL "SPX_CONFIG_EXPORTAPIS"
    OUTPUT_NAME "carbon-mock"
    FOLDER core
)

target_link_libraries(mock PRIVATE interfaces common ${SPEECHSDK_CORE_LIBRARY})
