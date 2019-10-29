project(pal)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")
set(INC_DIR "${PROJECT_SOURCE_DIR}/include")

include_directories(${INC_DIR})

if(SPEECHSDK_OS STREQUAL "Android")
    set(PAL_SRC "${SRC_DIR}/android")
else()
    set(PAL_SRC "${SRC_DIR}/base")
endif()

set(SOURCES
    "${PAL_SRC}/environment.cpp"
    "${PAL_SRC}/guid.cpp"
)

set(HEADERS
    "${INC_DIR}/guid.h"
)

set(HEADERS_PRIVATE
    "${PAL_SRC}/environment.h"
)

source_group("Header Files (public)" FILES ${HEADERS})
source_group("Header Files (private)" FILES ${HEADERS_PRIVATE})
source_group("Source Files" FILES ${SOURCES})

add_library(pal STATIC ${HEADERS} ${HEADERS_PRIVATE} ${SOURCES})

if(WIN32 AND SPECTRE_MITIGATION)
    set_property(TARGET pal PROPERTY VS_GLOBAL_SpectreMitigation "Spectre")
endif()

set_target_properties (pal PROPERTIES
    FOLDER core
)

if (SPEECHSDK_OS STREQUAL "Android")
    target_link_libraries(pal ${AZURE_SHARED_LIB})
else()
    target_link_libraries(pal ${AZURE_SHARED_LIB})
endif()