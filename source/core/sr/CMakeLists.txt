project(sr)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")
set(INC_DIR "${PROJECT_SOURCE_DIR}/include")

include_directories(${CARBON_C_API})
include_directories(${CARBON_CXX_API})
include_directories(${CARBON_INCLUDE})

include_directories(${INC_DIR})
include_directories("${AUDIO}/include")
include_directories("${COMMON}/include")
include_directories("${USP}/include")

set(HEADERS
    "${INC_DIR}/recognizer.h"
    "${INC_DIR}/recognizer_factory.h"
    )

set(HEADERS_PRIVATE
    "${SRC_DIR}/stdafx.h"
    "${SRC_DIR}/audio_pump.h"
    "${SRC_DIR}/audio_stream_session.h"
    "${SRC_DIR}/recognition_event_args.h"
    "${SRC_DIR}/recognition_result.h"
    "${SRC_DIR}/session.h"
    "${SRC_DIR}/session_event_args.h"
    "${SRC_DIR}/usp_reco_engine_adapter.h"
    )

set(SOURCES
    "${SRC_DIR}/stdafx.cpp"
    "${SRC_DIR}/audio_pump.cpp"
    "${SRC_DIR}/audio_stream_session.cpp"
    "${SRC_DIR}/recognition_event_args.cpp"
    "${SRC_DIR}/recognition_result.cpp"
    "${SRC_DIR}/recognizer.cpp"
    "${SRC_DIR}/recognizer_factory.cpp"
    "${SRC_DIR}/session.cpp"
    "${SRC_DIR}/session_event_args.cpp"
    "${SRC_DIR}/usp_reco_engine_adapter.cpp"
    )

source_group("Header Files (public)" FILES ${HEADERS})
source_group("Header Files (private)" FILES ${HEADERS_PRIVATE})
source_group("Source Files" FILES ${SOURCES})

add_library(sr STATIC ${HEADERS} ${HEADERS_PRIVATE} ${SOURCES})

set_target_properties (sr PROPERTIES
    FOLDER carbon_libs
)