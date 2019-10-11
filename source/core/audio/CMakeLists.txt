project(audio)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")
set(INC_DIR "${PROJECT_SOURCE_DIR}/include")

include_directories(${INC_DIR})
include_directories(${CARBON_C_API})
include_directories(${CARBON_CXX_API})
include_directories(${CARBON_INCLUDE})
include_directories("${CARBON_EXTERNAL}/json")
include_directories(${TTS})

include_directories("${COMMON}/include")

set(HEADERS "${INC_DIR}/audio_sys.h")

set(HEADERS_PRIVATE
    "${SRC_DIR}/stdafx.h"
    "${SRC_DIR}/audio_pump.h"
    "${SRC_DIR}/audio_config.h"
    "${SRC_DIR}/audio_data_stream.h"
    "${SRC_DIR}/pull_audio_input_stream.h"
    "${SRC_DIR}/push_audio_input_stream.h"
    "${SRC_DIR}/pull_audio_output_stream.h"
    "${SRC_DIR}/push_audio_output_stream.h"
    "${SRC_DIR}/single_to_many_stream_reader_adapter.h"
    "${SRC_DIR}/single_to_many_stream_reader.h"
    "${SRC_DIR}/audio_processor_write_to_audio_source_buffer.h"
    "${SRC_DIR}/audio_source_buffer_data.h"
    "${SRC_DIR}/audio_source_buffer_properties.h"
    "${SRC_DIR}/blocking_read_write_buffer.h"
    "${SRC_DIR}/blocking_read_write_ring_buffer.h"
    "${SRC_DIR}/create_module_object.h"
    "${SRC_DIR}/microphone_pump_base.h"
    "${SRC_DIR}/interactive_microphone.h"
    "${SRC_DIR}/read_write_ring_buffer.h"
    "${SRC_DIR}/wav_file_reader.h"
    "${SRC_DIR}/wav_file_pump.h"
    "${SRC_DIR}/wav_file_writer.h"
    "${SRC_DIR}/default_speaker.h"
    "${SRC_DIR}/null_audio_output.h"
    )

set(SOURCES
    "${SRC_DIR}/audio_config.cpp"
    "${SRC_DIR}/audio_pump.cpp"
    "${SRC_DIR}/audio_sys_common.cpp"
    "${SRC_DIR}/audio_data_stream.cpp"
    "${SRC_DIR}/pull_audio_input_stream.cpp"
    "${SRC_DIR}/push_audio_input_stream.cpp"
    "${SRC_DIR}/pull_audio_output_stream.cpp"
    "${SRC_DIR}/push_audio_output_stream.cpp"
    "${SRC_DIR}/audio_processor_write_to_audio_source_buffer.cpp"
    "${SRC_DIR}/single_to_many_stream_reader_adapter.cpp"
    "${SRC_DIR}/single_to_many_stream_reader.cpp"
    "${SRC_DIR}/audio_source_buffer_data.cpp"
    "${SRC_DIR}/audio_source_buffer_properties.cpp"
    "${SRC_DIR}/create_module_object.cpp"
    "${SRC_DIR}/interactive_microphone.cpp"
    "${SRC_DIR}/microphone_pump_base.cpp"
    "${SRC_DIR}/pull_audio_input_stream.cpp"
    "${SRC_DIR}/push_audio_input_stream.cpp"
    "${SRC_DIR}/read_write_ring_buffer.cpp"
    "${SRC_DIR}/wav_file_pump.cpp"
    "${SRC_DIR}/wav_file_reader.cpp"
    "${SRC_DIR}/wav_file_writer.cpp"
    "${SRC_DIR}/default_speaker.cpp"
    "${SRC_DIR}/null_audio_output.cpp"
    )

set(AUDIO_LIBS common)

include_directories(${SRC_DIR}) # for "stdafx.h"

if(WIN32)
    list(APPEND HEADERS_PRIVATE
            "${SRC_DIR}/windows/iunknown.h"
            "${SRC_DIR}/windows/com_init_and_uninit.h"
            "${SRC_DIR}/windows/audio_sys_win_base.h"
            "${SRC_DIR}/windows/microphone_pump.h"
        )
    list(APPEND SOURCES
            "${SRC_DIR}/windows/microphone_pump.cpp"
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
    list(APPEND HEADERS_PRIVATE
        "${SRC_DIR}/osx/microphone_pump.h"
        )
    list(APPEND SOURCES
        "${SRC_DIR}/osx/audio_sys.cpp"
    )
    FIND_LIBRARY(AUDIO_TOOLBOX AudioToolbox)
    if (NOT AUDIO_TOOLBOX)
        message(FATAL_ERROR "AudioToolbox not found")
    endif()
    FIND_LIBRARY(CORE_FOUNDATION CoreFoundation)
    if (NOT CORE_FOUNDATION)
        message(FATAL_ERROR "CoreFoundation not found")
    endif()
    FIND_LIBRARY(CORE_AUDIO CoreAudio)
    if (NOT CORE_AUDIO)
        message(FATAL_ERROR "CoreAudio not found")
    endif()
    list(APPEND AUDIO_LIBS ${AUDIO_TOOLBOX} ${CORE_FOUNDATION} ${CORE_AUDIO})
else()
    if (NOT (CMAKE_SYSTEM_NAME STREQUAL "Android"))
        list(APPEND HEADERS_PRIVATE
             "${SRC_DIR}/linux/microphone_pump.h"
             )
        list(APPEND SOURCES
            "${SRC_DIR}/linux/microphone_pump.cpp"
            "${SRC_DIR}/linux/audio_sys.c"
            )
        list(APPEND AUDIO_LIBS asound)
    else()
        list(APPEND HEADERS_PRIVATE
             "${SRC_DIR}/android/microphone_pump.h"
             )
        list(APPEND SOURCES
            "${SRC_DIR}/android/microphone_pump.cpp"
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

if(CMAKE_SYSTEM_NAME STREQUAL "WindowsStore")
    # Need C++17 for WinRT API access via cppwinrt.
    set_target_properties(audio PROPERTIES CXX_STANDARD 17)
    target_compile_definitions(audio PUBLIC _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)
endif()

if(WIN32 AND SPECTRE_MITIGATION)
    set_property(TARGET audio PROPERTY VS_GLOBAL_SpectreMitigation "Spectre")
endif()

set_target_properties (audio PROPERTIES
    FOLDER core
)

target_link_libraries(audio PRIVATE ${AUDIO_LIBS})

