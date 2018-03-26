project(c_api)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")

include_directories(${CARBON_C_API})
include_directories(${CARBON_CXX_API})
include_directories(${CARBON_INCLUDE})
include_directories("${COMMON}/include")
include_directories("${SR}/include")

set(HEADERS)

set(HEADERS_PRIVATE
    "${SRC_DIR}/stdafx.h"
    "${SRC_DIR}/event_helpers.h"
    )

set(SOURCES
    "${SRC_DIR}/event_helpers.cpp"
    "${SRC_DIR}/speechapi_c_intent_recognizer.cpp"
    "${SRC_DIR}/speechapi_c_intent_result.cpp"
    "${SRC_DIR}/speechapi_c_intent_trigger.cpp"
    "${SRC_DIR}/speechapi_c_luis_model.cpp"
    "${SRC_DIR}/speechapi_c_recognizer.cpp"
    "${SRC_DIR}/speechapi_c_recognizer_factory.cpp"
    "${SRC_DIR}/speechapi_c_result.cpp"
    "${SRC_DIR}/speechapi_c_session.cpp"
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

set_target_properties (c_api PROPERTIES
    FOLDER carbon_libs
    COMPILE_DEFINITIONS "SPX_CONFIG_EXPORTAPIS"
)
