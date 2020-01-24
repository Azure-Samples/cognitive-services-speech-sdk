project(tts)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")
set(INC_DIR "${PROJECT_SOURCE_DIR}/include")

include_directories(${CARBON_C_API})
include_directories(${CARBON_CXX_API})
include_directories(${CARBON_INCLUDE})
include_directories("${CARBON_EXTERNAL}/json")

include_directories(${INC_DIR})
include_directories(${TTS_REST})
include_directories(${TTS_USP})
include_directories(${SR})
include_directories("${USP}/include")
include_directories("${COMMON}/include")
include_directories("${COMMON_GENERATED_INCLUDE}")

set(HEADERS
    )

set(HEADERS_PRIVATE
    "${SRC_DIR}/stdafx.h"
    "${SRC_DIR}/speech_synthesis_api_factory.h"
    "${SRC_DIR}/create_module_object.h"
    "${SRC_DIR}/synthesizer.h"
    "${SRC_DIR}/synthesis_result.h"
    "${SRC_DIR}/synthesis_event_args.h"
    "${SRC_DIR}/word_boundary_event_args.h"
    "${SRC_DIR}/synthesis_helper.h"
    )

set(SOURCES
    "${SRC_DIR}/stdafx.cpp"
    "${SRC_DIR}/speech_synthesis_api_factory.cpp"
    "${SRC_DIR}/create_module_object.cpp"
    "${SRC_DIR}/synthesizer.cpp"
    "${SRC_DIR}/synthesis_result.cpp"
    "${SRC_DIR}/synthesis_event_args.cpp"
    "${SRC_DIR}/word_boundary_event_args.cpp"
    )

source_group("Header Files (public)" FILES ${HEADERS})
source_group("Header Files (private)" FILES ${HEADERS_PRIVATE})
source_group("Source Files" FILES ${SOURCES})

add_library(tts STATIC ${HEADERS} ${HEADERS_PRIVATE} ${SOURCES})

if(WIN32 AND SPECTRE_MITIGATION)
    set_property(TARGET tts PROPERTY VS_GLOBAL_SpectreMitigation "Spectre")
endif()

set_target_properties (tts PROPERTIES
    FOLDER core
)

target_link_libraries(tts PRIVATE common)
