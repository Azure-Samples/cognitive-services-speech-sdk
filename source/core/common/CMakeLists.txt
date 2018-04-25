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
    "${INC_DIR}/http_helpers.h"
    "${INC_DIR}/mock_controller.h"
    "${INC_DIR}/named_properties_constants.h"
    "${INC_DIR}/named_properties_impl.h"
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
    )

set(HEADERS_PRIVATE   
    "${INC_DIR}/azure_c_shared_utility_buffer_wrapper.h"
    "${INC_DIR}/azure_c_shared_utility_httpapi_wrapper.h"
    "${INC_DIR}/azure_c_shared_utility_httpapiex_wrapper.h"
    "${INC_DIR}/azure_c_shared_utility_uniqueid_wrapper.h"
    "${INC_DIR}/azure_c_shared_utility_strings_wrapper.h"
    "${INC_DIR}/azure_c_shared_utility_xlogging_wrapper.h"
    )

set(SOURCES
    "${SRC_DIR}/exception.cpp"
    "${SRC_DIR}/debug_utils.cpp"
    "${SRC_DIR}/handle_table.cpp"
    "${SRC_DIR}/http_helpers.cpp"
    "${SRC_DIR}/mock_controller.cpp"
    "${SRC_DIR}/trace_message.cpp"
    "${SRC_DIR}/string_utils.cpp"
    "${SRC_DIR}/file_utils.cpp"
    "${SRC_DIR}/guid_utils.cpp"
    "${SRC_DIR}/platform.cpp"
    "${SRC_DIR}/site_helpers.cpp"
    "${SRC_DIR}/stdafx.cpp"
    )

source_group("Header Files (public)" FILES ${HEADERS})
source_group("Header Files (private)" FILES ${HEADERS_PRIVATE})
source_group("Source Files" FILES ${SOURCES})

add_library(common STATIC ${HEADERS} ${HEADERS_PRIVATE} ${SOURCES})

set_target_properties (common PROPERTIES
    FOLDER carbon_libs
)
target_link_libraries(common ${AZURE_SHARED_LIB})

if(WIN32)
    target_link_libraries(common Dbghelp.lib)
endif()

# Adding logcat logging
if(ANDROID)
    target_link_libraries(common log)
endif()
