project(usp)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")
set(INC_DIR "${PROJECT_SOURCE_DIR}/include")

include_directories(${INC_DIR})
include_directories ("${CARBON_EXTERNAL}/parson")
include_directories ("${COMMON}/include")

add_library(parson STATIC "${CARBON_ROOT}/external/parson/parson.c")

if(MSVC)
  set_target_properties (parson PROPERTIES
      COMPILE_FLAGS "/wd4232"
  )
endif()

set_target_properties (parson PROPERTIES
    FOLDER external_libs
)

set(HEADERS
    "${INC_DIR}/usp.h"
    "${INC_DIR}/usperror.h"
    "${INC_DIR}/uspmessages.h"
    "${INC_DIR}/iobuffer.h"
    )

set(HEADERS_PRIVATE
    "${SRC_DIR}/uspcommon.h"
    "${SRC_DIR}/uspinternal.h"
    "${SRC_DIR}/dnscache.h"
    "${SRC_DIR}/metricids.h"
    "${SRC_DIR}/metrics.h"
    "${SRC_DIR}/propbag.h"
    "${SRC_DIR}/tokenstore.h"
    "${SRC_DIR}/transport.h"
    )

set(SOURCES
    "${SRC_DIR}/dnscache.c"
    "${SRC_DIR}/iobuffer.c"
    "${SRC_DIR}/metrics.c"
    "${SRC_DIR}/propbag.c"
    "${SRC_DIR}/responses.cpp"
    "${SRC_DIR}/transport.c"
    "${SRC_DIR}/usp.cpp"
    "${SRC_DIR}/uspimpl.cpp"
    )

source_group("Header Files (public)" FILES ${HEADERS})
source_group("Header Files (private)" FILES ${HEADERS_PRIVATE})
source_group("Source Files" FILES ${SOURCES})

add_library(usp STATIC ${HEADERS} ${HEADERS_PRIVATE} ${SOURCES})

set(USP_LIBS ${AZURE_SHARED_LIB} parson)

if(WIN32)
    list(APPEND USP_LIBS rpcrt4.lib)
endif()

set_target_properties (usp PROPERTIES
    FOLDER carbon_libs
)

target_link_libraries(usp ${USP_LIBS})

#list(APPEND CARBON_LIBS usp)