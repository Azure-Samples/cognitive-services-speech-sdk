project(common)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")
set(INC_DIR "${PROJECT_SOURCE_DIR}/include")


set(HEADERS
    "${INC_DIR}/spxcore_common.h"
    "${INC_DIR}/spx_namespace.h"
    "${INC_DIR}/json.h"
    "${INC_DIR}/asyncop.h"
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
    "${INC_DIR}/object_init_impl.h"
    "${INC_DIR}/service_helpers.h"
    "${INC_DIR}/service_provider_impl.h"
    "${INC_DIR}/shared_ptr_helpers.h"
    "${INC_DIR}/log_helpers.h"
    "${INC_DIR}/singleton.h"
    "${INC_DIR}/platform.h"
    "${INC_DIR}/string_utils.h"
    "${INC_DIR}/file_utils.h"
    "${INC_DIR}/property_id_2_name_map.h"
    "${INC_DIR}/audio_format_id_2_name_map.h"
    "${INC_DIR}/try_catch_helpers.h"
    "${INC_DIR}/data_buffer.h"
    "${INC_DIR}/ring_buffer.h"
    "${INC_DIR}/audio_chunk.h"
    "${INC_DIR}/time_utils.h"
    "${INC_DIR}/http_exception.h"
    "${INC_DIR}/http_request.h"
    "${INC_DIR}/http_response.h"
    "${INC_DIR}/memory_utils.h"
    "${INC_DIR}/http_utils.h"
    "${INC_DIR}/event.h"
    "${INC_DIR}/extension_resource_manager.h"
    "${INC_DIR}/dynamic_module.h"
    "${INC_DIR}/module_factory.h"
    "${INC_DIR}/site_helpers.h"
    "${INC_DIR}/audio_source_buffer_data_delegate_impl.h"
    "${INC_DIR}/audio_source_buffer_data_writer_delegate_impl.h"
    "${INC_DIR}/audio_source_control_delegate_impl.h"
    "${INC_DIR}/audio_source_init_delegate_impl.h"
    "${INC_DIR}/audio_source_simple_impl.h"
    "${INC_DIR}/audio_pump_delegate_helper.h"
    "${INC_DIR}/audio_source_control_adapts_audio_pump_impl.h"
    "${INC_DIR}/audio_source_control_simple_impl.h"
    "${INC_DIR}/audio_source_init_not_impl.h"
    "${INC_DIR}/audio_source_microphone_pump_impl.h"
    "${INC_DIR}/audio_source_file_pump_impl.h"
    "${INC_DIR}/audio_source_stream_pump_impl.h"
    "${INC_DIR}/audio_source_buffer_service_provider_helper.h"
    "${INC_DIR}/proxy_server_info.h"
    "${INC_DIR}/http_headers.h"
    "${INC_DIR}/http_endpoint_info.h"
    "${INC_DIR}/exception_helpers.h"
    "${INC_DIR}/web_socket.h"
    "${INC_DIR}/i_web_socket.h"
    "${INC_DIR}/i_telemetry.h"
    "${INC_DIR}/no_op_telemetry.h"
    "${INC_DIR}/function_helpers.h"
    "${INC_DIR}/platform_init.h"
    "${INC_DIR}/buffer_helpers.h"
    "${INC_DIR}/thread_service.h"
    "${INC_DIR}/synthesis_helper.h"
    )

if(SPEECHSDK_OS STREQUAL "OSX")
  list(APPEND HEADERS
    "${INC_DIR}/thread_stack_pcs.h"
  )
endif()

set(HEADERS_PRIVATE
    "${INC_DIR}/azure_c_shared_utility_urlencode_wrapper.h"
    "${INC_DIR}/azure_c_shared_utility_buffer_wrapper.h"
    "${INC_DIR}/azure_c_shared_utility_httpapi_wrapper.h"
    "${INC_DIR}/azure_c_shared_utility_httpapiex_wrapper.h"
    "${INC_DIR}/azure_c_shared_utility_strings_wrapper.h"
    "${INC_DIR}/azure_c_shared_utility_xlogging_wrapper.h"
    "${INC_DIR}/azure_c_shared_utility_macro_utils.h"
    "${INC_DIR}/azure_c_shared_utility_includes.h"
    "${SRC_DIR}/stdafx.h"
    )

set(SOURCES
    "${SRC_DIR}/exception.cpp"
    "${SRC_DIR}/handle_table.cpp"
    "${SRC_DIR}/mock_controller.cpp"
    "${SRC_DIR}/string_utils.cpp"
    "${SRC_DIR}/file_utils.cpp"
    "${SRC_DIR}/guid_utils.cpp"
    "${SRC_DIR}/platform.cpp"
    "${SRC_DIR}/time_utils.cpp"
    "${SRC_DIR}/ring_buffer.cpp"
    "${SRC_DIR}/http_exception.cpp"
    "${SRC_DIR}/http_request.cpp"
    "${SRC_DIR}/http_response.cpp"
    "${SRC_DIR}/http_utils.cpp"
    "${SRC_DIR}/memory_utils.cpp"
    "${SRC_DIR}/extension_resource_manager.cpp"
    "${SRC_DIR}/extension_site_helpers.cpp"
    "${SRC_DIR}/dynamic_module.cpp"
    "${SRC_DIR}/module_factory.cpp"
    "${SRC_DIR}/http_endpoint_info.cpp"
    "${SRC_DIR}/web_socket.cpp"
    "${SRC_DIR}/platform_init.cpp"
    "${SRC_DIR}/thread_service.cpp"
    )

if(SPEECHSDK_OS STREQUAL "OSX")
  list(APPEND SOURCES
    "${SRC_DIR}/thread_stack_pcs.cpp"
    "${SRC_DIR}/backtrace.cpp"
  )
endif()

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

set_source_groups()

add_library(common STATIC ${HEADERS} ${HEADERS_PRIVATE} ${SOURCES})


target_include_directories(common
    PRIVATE
        ## TODO: These should come from target_link_libraries
        ${CARBON_C_API}
        ${CARBON_CXX_API}
        "${CARBON_EXTERNAL}/json"
    PUBLIC
        ${INC_DIR}
)

if(WIN32 AND SPECTRE_MITIGATION)
    set_property(TARGET common PROPERTY VS_GLOBAL_SpectreMitigation "Spectre")
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "WindowsStore")
    # Need C++17 for WinRT API access via cppwinrt.
    set_target_properties(common PROPERTIES CXX_STANDARD 17)
    target_compile_definitions(common PUBLIC _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)
endif()

set_target_properties(common PROPERTIES
    FOLDER core/shared
)

target_link_libraries(common
    PUBLIC interfaces
    PRIVATE pal
)

if(WIN32)
    target_link_libraries(common PRIVATE Dbghelp.lib)
endif()

# Adding logcat logging
if(ANDROID)
    target_link_libraries(common PRIVATE log)
endif()
