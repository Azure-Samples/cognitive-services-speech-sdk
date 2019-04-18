project(codec)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")
set(INC_DIR "${PROJECT_SOURCE_DIR}/include")

include_directories(${INC_DIR})
include_directories(${CARBON_C_API})
include_directories(${CARBON_CXX_API})
include_directories(${CARBON_INCLUDE})
include_directories("${CARBON_EXTERNAL}/json")

include_directories("${COMMON}/include")
if(SPEECHSDK_OS STREQUAL "Linux")
    find_package(PkgConfig)
    if (PKG_CONFIG_FOUND)
        pkg_check_modules(GSTL "gstreamer-1.0")
        if (GSTL_FOUND)
            add_definitions(${GSTL_CFLAGS} ${GSTL_CFLAGS_OTHER})
        endif()
    endif()
elseif(ANDROID)
    include_directories("${GSTREAMER_PKG}/include/gstreamer-1.0")
    include_directories("${GSTREAMER_PKG}/include/glib-2.0")
    include_directories("${GSTREAMER_PKG}/lib/glib-2.0/include")
endif()

set(HEADERS)

set(HEADERS_PRIVATE
    "${SRC_DIR}/stdafx.h"
    "${SRC_DIR}/create_module_object.h"
    "${SRC_DIR}/codec_adapter.h"
    "${SRC_DIR}/base_gstreamer.h"
    "${SRC_DIR}/mpeg_decoder.h"
    "${SRC_DIR}/opus_decoder.h"
    )

set(SOURCES
    "${SRC_DIR}/create_module_object.cpp"
    "${SRC_DIR}/codec_adapter.cpp"
    "${SRC_DIR}/stdafx.cpp"
    "${SRC_DIR}/base_gstreamer.cpp"
    "${SRC_DIR}/mpeg_decoder.cpp"
    "${SRC_DIR}/opus_decoder.cpp"
    )


source_group("Header Files (public)" FILES ${HEADERS})
source_group("Header Files (private)" FILES ${HEADERS_PRIVATE})
source_group("Source Files" FILES ${SOURCES})

if(SPEECHSDK_OS STREQUAL "Linux")
    set(GST_LIBS ${GSTL_LIBRARIES})
elseif(ANDROID)
# !!! libgstreamer_android.so MUST NOT BE SHIPPED with the distribution binary. We do not have license to do that. !!!
    set(GST_LIBS "${GSTREAMER_PKG}/libgstreamer_android.so")
endif()

add_library(codec SHARED ${HEADERS} ${HEADERS_PRIVATE} ${SOURCES})

if(WIN32 AND SPECTRE_MITIGATION)
    set_property(TARGET codec PROPERTY VS_GLOBAL_SpectreMitigation "Spectre")
endif()

set_target_properties (codec PROPERTIES
    DEFINE_SYMBOL "SPX_CONFIG_EXPORTAPIS"
    OUTPUT_NAME "Microsoft.CognitiveServices.Speech.extension.codec"
    FOLDER core
)

target_include_directories(codec PRIVATE ${GSTL_INCLUDE_DIRS})
target_link_libraries(codec PRIVATE ${GST_LIBS} common)

