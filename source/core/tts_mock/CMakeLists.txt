project(tts_mock)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")
set(INC_DIR "${PROJECT_SOURCE_DIR}/include")
set(HEADERS)

set(HEADERS_PRIVATE
    "${SRC_DIR}/stdafx.h"
    "${SRC_DIR}/create_module_object.h"
    "${SRC_DIR}/mock_tts_engine_adapter.h"
    )

set(SOURCES
    "${SRC_DIR}/stdafx.cpp"
    "${SRC_DIR}/create_module_object.cpp"
    "${SRC_DIR}/mock_tts_engine_adapter.cpp"
    )

set_source_groups()

add_library(tts_mock SHARED ${HEADERS} ${HEADERS_PRIVATE} ${SOURCES})

target_include_directories(tts_mock
    PRIVATE
        ${SRC_DIR}
        ${CARBON_C_API}
        ${CARBON_CXX_API}
        "${CARBON_EXTERNAL}/json"
        ${COMMON_GENERATED_INCLUDE}
    PUBLIC
        ${INC_DIR}
)

add_compile_definitions(USE_NON_CORE_ROOT_SITE)

if(WIN32 AND SPECTRE_MITIGATION)
    set_property(TARGET tts_mock PROPERTY VS_GLOBAL_SpectreMitigation "Spectre")
endif()

set_target_properties (tts_mock PROPERTIES
    OUTPUT_NAME "carbon-tts-mock"
    FOLDER core/tts
)

target_link_libraries(tts_mock PRIVATE interfaces common audio ${SPEECHSDK_CORE_LIBRARY})
