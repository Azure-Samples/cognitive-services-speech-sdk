project(logging)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")
set(INC_DIR "${PROJECT_SOURCE_DIR}/include")

set(HEADERS
    "${SRC_DIR}/stdafx.h"
    "${INC_DIR}/file_logger.h"
    "${INC_DIR}/trace_message.h"
    )

set(SOURCES
    "${SRC_DIR}/file_logger.cpp"
    "${SRC_DIR}/trace_message.cpp"
    )

# make this the last source file to avoid link warning LNK4221
list(APPEND SOURCES "${SRC_DIR}/stdafx.cpp")

source_group("Header Files (public)" FILES ${HEADERS})
source_group("Source Files" FILES ${SOURCES})

add_library(logging STATIC ${HEADERS} ${SOURCES})

target_include_directories(logging
    PRIVATE
        ${CARBON_C_API}
        ${CARBON_CXX_API}
        ${CARBON_INCLUDE}
        "${CARBON_EXTERNAL}/json"
    PUBLIC
        ${INC_DIR}
)

target_link_libraries(logging PRIVATE pal common)

if(WIN32 AND SPECTRE_MITIGATION)
    set_property(TARGET logging PROPERTY VS_GLOBAL_SpectreMitigation "Spectre")
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "WindowsStore")
    # Need C++17 for WinRT API access via cppwinrt.
    set_target_properties(logging PROPERTIES CXX_STANDARD 17)
    target_compile_definitions(logging PUBLIC _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)
endif()

set_target_properties(logging PROPERTIES
    FOLDER core
)
