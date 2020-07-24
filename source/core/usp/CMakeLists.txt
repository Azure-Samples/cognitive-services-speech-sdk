project(usp)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")
set(INC_DIR "${PROJECT_SOURCE_DIR}/include")

set(HEADERS
    "${INC_DIR}/usp.h"
    "${INC_DIR}/usperror.h"
    "${INC_DIR}/uspmessages.h"
    )

set(HEADERS_PRIVATE
    "${SRC_DIR}/uspcommon.h"
    "${SRC_DIR}/uspinternal.h"
    "${SRC_DIR}/usp_metrics.h"
    "${SRC_DIR}/stdafx.h"
    "${SRC_DIR}/usp_web_socket.h"
    )

set(SOURCES
    "${SRC_DIR}/usp_metrics.cpp"
    "${SRC_DIR}/usp.cpp"
    "${SRC_DIR}/uspimpl.cpp"
    "${SRC_DIR}/usp_web_socket.cpp"
    )

source_group("Header Files (public)" FILES ${HEADERS})
source_group("Header Files (private)" FILES ${HEADERS_PRIVATE})
source_group("Source Files" FILES ${SOURCES})

add_library(usp STATIC ${HEADERS} ${HEADERS_PRIVATE} ${SOURCES})

target_include_directories(usp
    PRIVATE
        ${SRC_DIR}
        ## TODO: These four should also come from target_link_libraries
        ${CARBON_C_API}
        ${CARBON_CXX_API}
        ${CARBON_INCLUDE}
        ${CARBON_EXTERNAL}/json
    PUBLIC
        ${INC_DIR}
)

if(WIN32 AND SPECTRE_MITIGATION)
    set_property(TARGET usp PROPERTY VS_GLOBAL_SpectreMitigation "Spectre")
endif()

set_target_properties (usp PROPERTIES
    FOLDER core
)

target_link_libraries(usp common)

#list(APPEND CARBON_LIBS usp)
