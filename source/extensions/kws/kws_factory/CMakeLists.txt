project(kws_factory)

include_directories("${CARBON_EXTERNAL}/json")

set(HEADERS)

set(HEADERS_PRIVATE
    stdafx.h
    create_module_object.h
    )

set(SOURCES
    create_module_object.cpp
    stdafx.cpp
    )

source_group("Header Files (public)" FILES ${HEADERS})
source_group("Header Files (private)" FILES ${HEADERS_PRIVATE})
source_group("Source Files" FILES ${SOURCES})

if(WIN32)
  set(SPEECHSDK_ORIGINAL_FILENAME "Microsoft.CognitiveServices.Speech.extension.kws.dll")
  configure_file(
    ${CARBON_ROOT}/ci/version.rc.in
    ${CMAKE_CURRENT_BINARY_DIR}/windows/version.rc)

  list(APPEND SOURCES "${CMAKE_CURRENT_BINARY_DIR}/windows/version.rc")
endif()

if(SPEECHSDK_OS MATCHES "^IOS")
    add_library(kws_factory STATIC ${HEADERS} ${HEADERS_PRIVATE} ${SOURCES})
else()
    add_library(kws_factory SHARED ${HEADERS} ${HEADERS_PRIVATE} ${SOURCES})
    set_target_properties(kws_factory PROPERTIES
        DEFINE_SYMBOL "SPX_CONFIG_EXPORTAPIS"
        OUTPUT_NAME "Microsoft.CognitiveServices.Speech.extension.kws"
        FOLDER "extensions/kws"
        )
endif()

# Set USE_NON_CORE_ROOT_SITE based on if we are building extensions or unit tests
add_compile_definitions(USE_NON_CORE_ROOT_SITE)

target_include_directories(kws_factory
    PRIVATE
        ${SDKKWS_HEADERS}
        ${CARBON_C_API}
        ${CARBON_CXX_API}
        ${CARBON_INCLUDE}
        "${COMMON}/include"
    )

if(SPEECHSDK_OS MATCHES "^IOS")
    target_link_libraries(kws_factory
     PRIVATE common
     PRIVATE kws_wrapper
        )
else()
    target_link_libraries(kws_factory
    PRIVATE common
    PRIVATE kws_wrapper
        )
endif()

if(STATIC_KWS_EXTENSION)
    target_link_libraries(kws_factory PRIVATE logging)
else()
    target_link_libraries(kws_factory PRIVATE ${SPEECHSDK_CORE_LIBRARY})
endif()
