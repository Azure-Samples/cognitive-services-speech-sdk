project(usp)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")
set(INC_DIR "${PROJECT_SOURCE_DIR}/include")

add_subdirectory ("${CARBON_EXTERNAL}/azure-c-shared-utility" azure_c_shared_utility EXCLUDE_FROM_ALL)

set_target_properties (aziotsharedutil PROPERTIES
    FOLDER external_libs
    )

include_directories(${INC_DIR})
include_directories (${SHARED_UTIL_INC_FOLDER})
include_directories ("${CARBON_EXTERNAL}/parson")

set(HEADERS
    "${INC_DIR}/usp.h"
    "${INC_DIR}/usperror.h"
    "${INC_DIR}/uspmessages.h"
    )

set(HEADERS_PRIVATE
    "${SRC_DIR}/uspcommon.h"
    "${SRC_DIR}/uspinternal.h"
    "${SRC_DIR}/dnscache.h"
    "${SRC_DIR}/iobuffer.h"
    "${SRC_DIR}/metricids.h"
    "${SRC_DIR}/metrics.h"
    "${SRC_DIR}/propbag.h"
    "${SRC_DIR}/tokenstore.h"
    "${SRC_DIR}/transport.h"
    )

set(SOURCES
    "${CARBON_ROOT}/external/parson/parson.c"
    "${SRC_DIR}/dnscache.c"
    "${SRC_DIR}/iobuffer.c"
    "${SRC_DIR}/metrics.c"
    "${SRC_DIR}/propbag.c"
    "${SRC_DIR}/responses.c"
    "${SRC_DIR}/transport.c"
    "${SRC_DIR}/uspapi.c"
    "${SRC_DIR}/uspimpl.c"
    )

source_group("Header Files (public)" FILES ${HEADERS})
source_group("Header Files (private)" FILES ${HEADERS_PRIVATE})
source_group("Source Files" FILES ${SOURCES})

add_library(usp STATIC ${HEADERS} ${HEADERS_PRIVATE} ${SOURCES})

set(USP_LIBS aziotsharedutil)

if(WIN32)
    list(APPEND USP_LIBS rpcrt4.lib)
endif()

if(LINUX)
    # todo: double check this!
    list(APPEND USP_LIBS uuid)
endif()

set_target_properties (usp PROPERTIES
    FOLDER carbon_libs
)

target_link_libraries(usp ${USP_LIBS})

#list(APPEND CARBON_LIBS usp)