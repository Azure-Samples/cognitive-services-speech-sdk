project(audio)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")
set(INC_DIR "${PROJECT_SOURCE_DIR}/include")

include_directories(${INC_DIR})
include_directories(${CARBON_C_API})
include_directories(${CARBON_CXX_API})
include_directories(${CARBON_INCLUDE})

include_directories("${COMMON}/include")

set(HEADERS "${INC_DIR}/audio_sys.h")

set(HEADERS_PRIVATE
    "${SRC_DIR}/stdafx.h"
    "${SRC_DIR}/audio_pump.h"
    "${SRC_DIR}/audio_config.h"
    "${SRC_DIR}/pull_audio_input_stream.h"
    "${SRC_DIR}/push_audio_input_stream.h"
    "${SRC_DIR}/create_module_object.h"
    "${SRC_DIR}/microphone.h"
    "${SRC_DIR}/microphone_pump_base.h"
    "${SRC_DIR}/interactive_microphone.h"
    "${SRC_DIR}/wav_file_reader.h"
    "${SRC_DIR}/wav_file_pump.h"
    )

set(SOURCES
    "${SRC_DIR}/audio_pump.cpp"
    "${SRC_DIR}/audio_config.cpp"
    "${SRC_DIR}/pull_audio_input_stream.cpp"
    "${SRC_DIR}/push_audio_input_stream.cpp"
    "${SRC_DIR}/create_module_object.cpp"
    "${SRC_DIR}/interactive_microphone.cpp"
    "${SRC_DIR}/wav_file_reader.cpp"
    "${SRC_DIR}/wav_file_pump.cpp"
    "${SRC_DIR}/microphone_pump_base.cpp"
    )

set(AUDIO_LIBS common)

include_directories(${SRC_DIR}) # for "stdafx.h"

if(WIN32)
    list(APPEND HEADERS_PRIVATE
            "${SRC_DIR}/windows/iunknown.h"
            "${SRC_DIR}/windows/com_init_and_uninit.h"
            "${SRC_DIR}/windows/audio_sys_win_base.h"
        )
    list(APPEND SOURCES
            "${SRC_DIR}/windows/microphone.cpp"
            "${SRC_DIR}/windows/com_init_and_uninit.cpp"
            "${SRC_DIR}/windows/audio_sys_win_base.cpp"
        )
    if(${CMAKE_SYSTEM_NAME} STREQUAL "WindowsStore")
        list(APPEND SOURCES
            "${SRC_DIR}/windows/audio_sys_winrt.cpp"
        )
        list(APPEND AUDIO_LIBS mmdevapi)
    else()
        list(APPEND SOURCES
            "${SRC_DIR}/windows/audio_sys_win32.cpp"
        )
    endif()

elseif(APPLE)
    list(APPEND SOURCES
        "${SRC_DIR}/osx/microphone.cpp"
        "${SRC_DIR}/osx/audio_sys.c"
    )
    FIND_LIBRARY(AUDIO_TOOLBOX AudioToolbox)
    if (NOT AUDIO_TOOLBOX)
        message(FATAL_ERROR "AudioToolbox not found")
    endif()
list(APPEND AUDIO_LIBS ${AUDIO_TOOLBOX})
else()
    if (NOT (CMAKE_SYSTEM_NAME STREQUAL "Android"))
        list(APPEND SOURCES
            "${SRC_DIR}/linux/microphone.cpp"
            "${SRC_DIR}/linux/audio_sys.c"
        )
        list(APPEND AUDIO_LIBS asound)
    else()
        list(APPEND SOURCES
            "${SRC_DIR}/android/microphone.cpp"
            "${SRC_DIR}/android/audio_sys.cpp"
            "${SRC_DIR}/android/audio_common.cpp"
            "${SRC_DIR}/android/audio_recorder.cpp"
            "${SRC_DIR}/android/debug_utils.cpp"
        )
        list(APPEND AUDIO_LIBS OpenSLES android log atomic)
    endif()
endif()

list(APPEND SOURCES
    "${SRC_DIR}/stdafx.cpp"
)

source_group("Header Files (public)" FILES ${HEADERS})
source_group("Header Files (private)" FILES ${HEADERS_PRIVATE})
source_group("Source Files" FILES ${SOURCES})

add_library(audio STATIC ${HEADERS} ${HEADERS_PRIVATE} ${SOURCES})

if(WIN32 AND SPECTRE_MITIGATION)
    set_property(TARGET audio PROPERTY VS_GLOBAL_SpectreMitigation "Spectre")
endif()

set_target_properties (audio PROPERTIES
    FOLDER core
)

target_link_libraries(audio PRIVATE ${AUDIO_LIBS})
