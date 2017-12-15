project(audio)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")
set(INC_DIR "${PROJECT_SOURCE_DIR}/include")

include_directories(${INC_DIR})
include_directories(${CARBON_C_API})
include_directories(${CARBON_CXX_API})
include_directories(${CARBON_INCLUDE})

include_directories("${COMMON}/include")

set(HEADERS
    "${INC_DIR}/microphone.h"
    "${INC_DIR}/wav_file_reader.h"
    )

set(HEADERS_PRIVATE "${SRC_DIR}/stdafx.h")

set(SOURCES

    "${SRC_DIR}/stdafx.cpp"
    "${SRC_DIR}/wav_file_reader.cpp"
    )

set(AUDIO_LIBS common)

if(WIN32)
    include_directories(${SRC_DIR}) # for "stdafx.h"
    list(APPEND HEADERS_PRIVATE
        "${SRC_DIR}/windows/audio_sample_handler.h"
        "${SRC_DIR}/windows/iunknown.h"
        "${SRC_DIR}/windows/sample_handler.h"
    )
    list(APPEND SOURCES
        "${SRC_DIR}/windows/audio_sample_handler.cpp"
        "${SRC_DIR}/windows/microphone.cpp"
        "${SRC_DIR}/windows/sample_handler.cpp"
    )
    list(APPEND AUDIO_LIBS
        mf.lib
        mfplat.lib
        mfreadwrite.lib
        mfuuid.lib
    )
endif()

source_group("Header Files (public)" FILES ${HEADERS})
source_group("Header Files (private)" FILES ${HEADERS_PRIVATE})
source_group("Source Files" FILES ${SOURCES})

add_library(audio STATIC ${HEADERS} ${HEADERS_PRIVATE} ${SOURCES})

set_target_properties (audio PROPERTIES
    FOLDER carbon_libs
)

target_link_libraries(audio PRIVATE ${AUDIO_LIBS})