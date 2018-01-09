project(common)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")
set(INC_DIR "${PROJECT_SOURCE_DIR}/include")

include_directories(${INC_DIR})
include_directories(${CARBON_C_API})
include_directories(${CARBON_INCLUDE})
include_directories (${AZURE_SHARED_UTIL_INCLUDE})
#include_directories(${SRC_DIR}) # for "stdafx.h"

set(HEADERS
    "${SRC_DIR}/stdafx.h"
    "${INC_DIR}/guid_utils.h"
    "${INC_DIR}/handle_table.h"
    "${INC_DIR}/trace_message.h"
    "${INC_DIR}/platform.h"
    "${INC_DIR}/string_utils.h"
    "${INC_DIR}/file_utils.h"
    )

set(HEADERS_PRIVATE)

set(SOURCES
    "${SRC_DIR}/stdafx.cpp"
    "${SRC_DIR}/handle_table.cpp"
    "${SRC_DIR}/trace_message.cpp"
    "${SRC_DIR}/string_utils.cpp"
    "${SRC_DIR}/file_utils.cpp"
    "${SRC_DIR}/guid_utils.cpp"
    )

source_group("Header Files (public)" FILES ${HEADERS})
source_group("Header Files (private)" FILES ${HEADERS_PRIVATE})
source_group("Source Files" FILES ${SOURCES})

add_library(common STATIC ${HEADERS} ${HEADERS_PRIVATE} ${SOURCES})

set_target_properties (common PROPERTIES
    FOLDER carbon_libs
)
target_link_libraries(common ${AZURE_SHARED_LIB})