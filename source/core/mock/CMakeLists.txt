project(mock)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")

include_directories(${CARBON_C_API})
include_directories(${CARBON_CXX_API})
include_directories(${CARBON_INCLUDE})

include_directories("${COMMON}/include")

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

source_group("Header Files (public)" FILES ${HEADERS})
source_group("Header Files (private)" FILES ${HEADERS_PRIVATE})
source_group("Source Files" FILES ${SOURCES})

add_library(mock SHARED ${HEADERS} ${HEADERS_PRIVATE} ${SOURCES})

set_target_properties (mock PROPERTIES
    DEFINE_SYMBOL  "SPX_CONFIG_EXPORTAPIS" 
    OUTPUT_NAME "carbon-mock"
    FOLDER carbon_libs
)

target_link_libraries(mock PRIVATE common libcarbon)
