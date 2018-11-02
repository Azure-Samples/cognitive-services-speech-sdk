project(common)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")
set(INC_DIR "${PROJECT_SOURCE_DIR}/include")

include_directories(${INC_DIR})
include_directories(${CARBON_C_API})
include_directories(${CARBON_CXX_API})
include_directories(${CARBON_INCLUDE})

set(HEADERS
    "${SRC_DIR}/stdafx.h"
    "${INC_DIR}/exception.h"
    "${INC_DIR}/create_object_helpers.h"
    "${INC_DIR}/debug_utils.h"
    "${INC_DIR}/delegate_audio_pump_impl.h"
    "${INC_DIR}/delegate_audio_file_impl.h"
    "${INC_DIR}/factory_helpers.h"
    "${INC_DIR}/guid_utils.h"
    "${INC_DIR}/handle_table.h"
    "${INC_DIR}/mock_controller.h"
    "${INC_DIR}/property_bag_impl.h"
    "${INC_DIR}/resource_manager.h"
    "${INC_DIR}/service_helpers.h"
    "${INC_DIR}/service_provider_impl.h"
    "${INC_DIR}/shared_ptr_helpers.h"
    "${INC_DIR}/singleton.h"
    "${INC_DIR}/site_helpers.h"
    "${INC_DIR}/trace_message.h"
    "${INC_DIR}/platform.h"
    "${INC_DIR}/string_utils.h"
    "${INC_DIR}/file_utils.h"
    "${CARBON_INCLUDE}/ispxinterfaces.h"
    "${INC_DIR}/property_id_2_name_map.h"
    "${INC_DIR}/speech_config.h"
    "${INC_DIR}/try_catch_helpers.h"
    "${INC_DIR}/data_buffer.h"
    )

set(HEADERS_PRIVATE
    "${INC_DIR}/azure_c_shared_utility_urlencode_wrapper.h"
    "${INC_DIR}/azure_c_shared_utility_buffer_wrapper.h"
    "${INC_DIR}/azure_c_shared_utility_httpapi_wrapper.h"
    "${INC_DIR}/azure_c_shared_utility_httpapiex_wrapper.h"
    "${INC_DIR}/azure_c_shared_utility_uniqueid_wrapper.h"
    "${INC_DIR}/azure_c_shared_utility_strings_wrapper.h"
    "${INC_DIR}/azure_c_shared_utility_xlogging_wrapper.h"
    )

set(SOURCES
    "${SRC_DIR}/exception.cpp"
    "${SRC_DIR}/handle_table.cpp"
    "${SRC_DIR}/mock_controller.cpp"
    "${SRC_DIR}/trace_message.cpp"
    "${SRC_DIR}/string_utils.cpp"
    "${SRC_DIR}/file_utils.cpp"
    "${SRC_DIR}/guid_utils.cpp"
    "${SRC_DIR}/platform.cpp"
    "${SRC_DIR}/site_helpers.cpp"
    "${SRC_DIR}/property_id_2_name_map.cpp"
    "${SRC_DIR}/speech_config.cpp"
    )

if(CMAKE_SYSTEM_NAME STREQUAL "WindowsStore")
    list(APPEND SOURCES
            "${SRC_DIR}/debug_utils_winrt.cpp"
    )
else()
   list(APPEND SOURCES
           "${SRC_DIR}/debug_utils.cpp"
    )
endif()

configure_file(
    spx_build_information.h.in
    ${COMMON_GENERATED_INCLUDE}/spx_build_information.h
    ESCAPE_QUOTES)

# make this the last source file to avoid link warning LNK4221 
list(APPEND SOURCES "${SRC_DIR}/stdafx.cpp")

source_group("Header Files (public)" FILES ${HEADERS})
source_group("Header Files (private)" FILES ${HEADERS_PRIVATE})
source_group("Source Files" FILES ${SOURCES})

add_library(common STATIC ${HEADERS} ${HEADERS_PRIVATE} ${SOURCES})

if(WIN32 AND SPECTRE_MITIGATION)
    set_property(TARGET common PROPERTY VS_GLOBAL_SpectreMitigation "Spectre")
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "WindowsStore")
    # Need C++17 for WinRT API access via cppwinrt.
    set_target_properties(common PROPERTIES CXX_STANDARD 17)
    target_compile_definitions(common PUBLIC _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)
endif()

set_target_properties(common PROPERTIES
    FOLDER core
)
target_link_libraries(common ${AZURE_SHARED_LIB})

if(WIN32)
    target_link_libraries(common Dbghelp.lib)
endif()

# Adding logcat logging
if(ANDROID)
    target_link_libraries(common log)
endif()
