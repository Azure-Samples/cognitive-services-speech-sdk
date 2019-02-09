project(kws_factory)

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


add_library(kws_factory SHARED ${HEADERS} ${HEADERS_PRIVATE} ${SOURCES})

target_include_directories(kws_factory
    PRIVATE
        ${SDKKWS_HEADERS}
        ${CARBON_C_API}
        ${CARBON_CXX_API}
        ${CARBON_INCLUDE}
        "${COMMON}/include"
    )

set_target_properties(kws_factory PROPERTIES
    DEFINE_SYMBOL "SPX_CONFIG_EXPORTAPIS"
    OUTPUT_NAME "Microsoft.CognitiveServices.Speech.extension.kws"
    FOLDER "extensions/kws"
    )

target_link_libraries(kws_factory PRIVATE
    common
    kws_wrapper
    # TODO seems not needed:
    ${SPEECHSDK_CORE_LIBRARY}
    $<TARGET_OBJECTS:c_api>
    )
