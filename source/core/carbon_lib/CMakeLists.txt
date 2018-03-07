project(carbon_lib)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")

include_directories(${CARBON_C_API})
include_directories(${CARBON_CXX_API})
include_directories(${CARBON_INCLUDE})

include_directories("${COMMON}/include")
include_directories("${USP}/include")
include_directories(${SHARED_UTIL_INC_FOLDER})

set(HEADERS)

set(HEADERS_PRIVATE)

set(SOURCES "${SRC_DIR}/stdafx.cpp")

include_directories(${SRC_DIR}) # for "stdafx.h"

if(WIN32)
    if (MSVC)

        set_source_files_properties("${SRC_DIR}/stdafx.cpp"
            PROPERTIES
            COMPILE_FLAGS "/Ycstdafx.h"
        )

        # For precompiled header.
        # Set 
        # "Precompiled Header" to "Use (/Yu)"
        # "Precompiled Header File" to "stdafx.h"
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Yustdafx.h /FIstdafx.h")
    endif()

    list(APPEND HEADERS_PRIVATE
        "${SRC_DIR}/stdafx.h"
        "${SRC_DIR}/windows/targetver.h"
    )
    list(APPEND SOURCES
        "${SRC_DIR}/windows/dll.cpp"
    )
endif()

source_group("Header Files (public)" FILES ${HEADERS})
source_group("Header Files (private)" FILES ${HEADERS_PRIVATE})
source_group("Source Files" FILES ${SOURCES})

add_library(carbon SHARED 
    ${HEADERS} ${HEADERS_PRIVATE} ${SOURCES}
    )

# Linker ignores the __declspec(dllexports) coming from the static lib,
# for now have to use a manually generated exports def file.
# More details:
# 1) https://social.msdn.microsoft.com/Forums/vstudio/en-US/d06a8436-7d06-46f5-9ae8-4df26ef1a0da/help-me-keeping-exports-coming-from-a-static-library?forum=vclanguage
# 2) https://stackoverflow.com/questions/31446363/proper-way-to-link-static-libraries-with-dll
target_sources(carbon PRIVATE "${SRC_DIR}/exports.def")

target_link_libraries(carbon PUBLIC c_api)

SET_TARGET_PROPERTIES(carbon PROPERTIES
    DEFINE_SYMBOL  "SPX_CONFIG_EXPORTAPIS" 
    PROJECT_LABEL "carbon_lib"
)

