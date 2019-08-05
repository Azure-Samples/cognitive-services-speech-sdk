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
    "${SRC_DIR}/audio_decoder.h"
    "${SRC_DIR}/opus_decoder.h"
    "${SRC_DIR}/gstreamer_modules.h"
    )

set(SOURCES
    "${SRC_DIR}/create_module_object.cpp"
    "${SRC_DIR}/codec_adapter.cpp"
    "${SRC_DIR}/stdafx.cpp"
    "${SRC_DIR}/base_gstreamer.cpp"
    "${SRC_DIR}/audio_decoder.cpp"
    "${SRC_DIR}/opus_decoder.cpp"
    )

set(STATICLIB_SOURCES
    "${SRC_DIR}/gstreamer_modules.cpp"
    )

if (NOT DEFINED APPLE)
    set(SOURCES ${SOURCES} ${STATICLIB_SOURCES})
endif()

source_group("Header Files (public)" FILES ${HEADERS})
source_group("Header Files (private)" FILES ${HEADERS_PRIVATE})
source_group("Source Files" FILES ${SOURCES})

if(SPEECHSDK_OS STREQUAL "Linux")
    set(GST_LIBS ${GSTL_LIBRARIES})
elseif(APPLE)
    # !!! GStreamerWrapper.framework MUST NOT BE SHIPPED with the distribution binary. We do not have license to do that. !!!
    FIND_LIBRARY(GSTREAMER GStreamer)
    if (NOT GSTREAMER)
        message(FATAL_ERROR "GSTREAMER not found")
    endif()
    FIND_LIBRARY(GSTREAMER_WRAPPER GStreamerWrapper)
    if (NOT GSTREAMER_WRAPPER)
        message(FATAL_ERROR "GSTREAMER_WRAPPER not found")
    endif()
    get_filename_component(GSTREAMER_WRAPPER_DIR ${GSTREAMER_WRAPPER} DIRECTORY)
    message(STATUS "GST wrapper " ${GSTREAMER_WRAPPER} " at " ${GSTREAMER_WRAPPER_DIR})
    set(GST_LIBS ${GSTREAMER} iconv)
    set(GST_INCLUDE_DIRS ${GSTREAMER}/Headers)
    message(STATUS "GST libs are " ${GST_LIBS})
elseif(ANDROID)
    # !!! libgstreamer_android.so MUST NOT BE SHIPPED with the distribution binary. We do not have license to do that. !!!
    set(GST_LIBS "${GSTREAMER_PKG}/libgstreamer_android.so")
endif()

if(SPEECHSDK_OS MATCHES "^IOS")
    add_library(codec STATIC ${HEADERS} ${HEADERS_PRIVATE} ${SOURCES})
else()
    add_library(codec SHARED ${HEADERS} ${HEADERS_PRIVATE} ${SOURCES})
    set_target_properties (codec PROPERTIES
        DEFINE_SYMBOL "SPX_CONFIG_EXPORTAPIS"
        OUTPUT_NAME "Microsoft.CognitiveServices.Speech.extension.codec"
        FOLDER core
        )
endif()

if(WIN32 AND SPECTRE_MITIGATION)
    set_property(TARGET codec PROPERTY VS_GLOBAL_SpectreMitigation "Spectre")
endif()

target_include_directories(codec PRIVATE ${GST_INCLUDE_DIRS})
if(SPEECHSDK_OS MATCHES "^IOS")
    target_link_libraries(codec
        PUBLIC "-weak_framework GStreamerWrapper -F${GSTREAMER_WRAPPER_DIR}"
        )
else()
    target_link_libraries(codec PRIVATE ${GST_LIBS} common)
endif()

