project(conversation_translation)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")
set(INC_DIR "${PROJECT_SOURCE_DIR}/include")

set(HEADERS
    "${INC_DIR}/ISpxConversationInterfaces.h"
    "${INC_DIR}/create_module_object.h"
    )

set(HEADERS_PRIVATE
    "${SRC_DIR}/common.h"
    "${SRC_DIR}/conversation_impl.h"
    "${SRC_DIR}/conversation_messages.h"
    "${SRC_DIR}/conversation_manager.h"
    "${SRC_DIR}/conversation_connection.h"
    "${SRC_DIR}/conversation_translator.h"
    "${SRC_DIR}/conversation_utils.h"
    "${SRC_DIR}/conversation_events.h"
    "${SRC_DIR}/conversation_translator_logging.h"
    "${SRC_DIR}/conversation_translator_connection.h"
    )

set(SOURCES
    "${SRC_DIR}/stdafx.cpp"
    "${SRC_DIR}/conversation_impl.cpp"
    "${SRC_DIR}/create_module_object.cpp"
    "${SRC_DIR}/conversation_manager.cpp"
    "${SRC_DIR}/conversation_messages.cpp"
    "${SRC_DIR}/conversation_connection.cpp"
    "${SRC_DIR}/conversation_translator.cpp"
    "${SRC_DIR}/conversation_translator_connection.cpp"
    )

source_group("Header Files (public)" FILES ${HEADERS})
source_group("Header Files (private)" FILES ${HEADERS_PRIVATE})
source_group("Source Files" FILES ${SOURCES})

add_library(${PROJECT_NAME} STATIC ${HEADERS} ${HEADERS_PRIVATE} ${SOURCES})

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CARBON_C_API}
        ${CARBON_CXX_API}
        ${CARBON_INCLUDE}
        "${CARBON_EXTERNAL}/json"
        "${COMMON}/include"
        ${COMMON_GENERATED_INCLUDE}
    PUBLIC
        ${INC_DIR}
)



if(WIN32 AND SPECTRE_MITIGATION)
    set_property(TARGET ${PROJECT_NAME} PROPERTY VS_GLOBAL_SpectreMitigation "Spectre")
endif()

if (UNIX)
    add_definitions(-Wno-error)
endif()

set_target_properties (${PROJECT_NAME} PROPERTIES
    FOLDER core
)

if(SPEECHSDK_OS STREQUAL "Linux")
    set(http_LIBS ${AZURE_SHARED_LIB} -lz -ldl)
else()
    set(http_LIBS ${AZURE_SHARED_LIB})
endif()

if(WIN32)
    list(APPEND http_LIBS rpcrt4.lib)
endif()

target_link_libraries(${PROJECT_NAME}
                      PRIVATE common
                      PRIVATE ${http_LIBS})

if (MSVC)
   set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "/Yucommon.h")
   set_source_files_properties(stdafx.cpp PROPERTIES COMPILE_FLAGS "/Yccommon.h")
endif(MSVC)

