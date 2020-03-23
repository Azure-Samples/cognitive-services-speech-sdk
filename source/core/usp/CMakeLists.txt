project(usp)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")
set(INC_DIR "${PROJECT_SOURCE_DIR}/include")

include_directories(${INC_DIR})
include_directories(${CARBON_C_API})
include_directories(${CARBON_CXX_API})
include_directories(${CARBON_INCLUDE})
include_directories("${CARBON_EXTERNAL}/json")
include_directories ("${COMMON}/include")

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
    "${SRC_DIR}/stdafx.cpp"
    "${SRC_DIR}/usp_web_socket.cpp"
    )

source_group("Header Files (public)" FILES ${HEADERS})
source_group("Header Files (private)" FILES ${HEADERS_PRIVATE})
source_group("Source Files" FILES ${SOURCES})

add_library(usp STATIC ${HEADERS} ${HEADERS_PRIVATE} ${SOURCES})

if(WIN32 AND SPECTRE_MITIGATION)
    set_property(TARGET usp PROPERTY VS_GLOBAL_SpectreMitigation "Spectre")
endif()

if(SPEECHSDK_OS STREQUAL "Linux")
    set(USP_LIBS ${AZURE_SHARED_LIB} -lz -ldl)
else()
    set(USP_LIBS ${AZURE_SHARED_LIB})
endif()

if(WIN32)
    list(APPEND USP_LIBS rpcrt4.lib)
endif()

set_target_properties (usp PROPERTIES
    FOLDER core
)

target_link_libraries(usp ${USP_LIBS})

#list(APPEND CARBON_LIBS usp)
