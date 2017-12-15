project(common)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")
set(INC_DIR "${PROJECT_SOURCE_DIR}/include")

include_directories(${INC_DIR})
include_directories(${CARBON_C_API})
include_directories(${CARBON_INCLUDE})

set(HEADERS
    "${SRC_DIR}/stdafx.h"
    "${INC_DIR}/guid_utils.h"
    "${INC_DIR}/handle_table.h"
    "${INC_DIR}/trace_message.h"
    )

set(HEADERS_PRIVATE)

set(SOURCES
    "${SRC_DIR}/stdafx.cpp"
    "${SRC_DIR}/handle_table.cpp"
    "${SRC_DIR}/trace_message.cpp"
    )

if(WIN32)
    include_directories(${SRC_DIR}) # for "stdafx.h"
    list(APPEND SOURCES
        "${SRC_DIR}/windows/guid_utils.cpp"
    )
endif()

source_group("Header Files (public)" FILES ${HEADERS})
source_group("Header Files (private)" FILES ${HEADERS_PRIVATE})
source_group("Source Files" FILES ${SOURCES})

add_library(common STATIC ${HEADERS} ${HEADERS_PRIVATE} ${SOURCES})

set_target_properties (common PROPERTIES
    FOLDER carbon_libs
)