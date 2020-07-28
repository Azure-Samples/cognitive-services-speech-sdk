project(${SPEECHSDK_CORE_FRAMEWORK})

set(SRC_DIR "${PROJECT_SOURCE_DIR}/src")
set(OBJC_PUBLIC_HEADER_DIR "${PROJECT_SOURCE_DIR}/public")
set(LIBCARBON_SRC_DIR "${PROJECT_SOURCE_DIR}/../../core/libcarbon")

include_directories(${CARBON_C_API})
include_directories(${CARBON_CXX_API})
include_directories(${OBJC_PUBLIC_HEADER_DIR})
include_directories("${COMMON}/include")
include_directories("${LOGGING}/include")
include_directories("${FACTORY}/include")

FILE(GLOB OBJC_PUBLIC_HEADERS "${OBJC_PUBLIC_HEADER_DIR}/*.h")
FILE(GLOB OBJC_PRIVATE_HEADERS "${SRC_DIR}/*.h")
FILE(GLOB OBJC_SOURCE "${SRC_DIR}/*.mm")

set(HEADERS ${OBJC_PUBLIC_HEADERS})
set(HEADERS_PRIVATE ${OBJC_PRIVATE_HEADERS})
set(SOURCES ${OBJC_SOURCE}
  "${SRC_DIR}/../../../core/libcarbon/dll.cpp"
  )

set_source_groups()

list(APPEND ALL_FRAMEWORK_HEADERS ${HEADERS} ${SPEECH_C_API_HEADERS} ${SPEECH_CXX_API_HEADERS})

# headers to be installed in the framework must be present in the add_library_call
# https://cmake.org/pipermail/cmake/2015-July/061143.html
add_library(${PROJECT_NAME} SHARED ${ALL_FRAMEWORK_HEADERS} ${HEADERS_PRIVATE} ${SOURCES} $<TARGET_OBJECTS:c_api>)

if(DEFINED APPLE)
    if (DEFINED PLATFORM)
        set(FRAMEWORK_MINIMUM_OS_VERSION_KEY "MinimumOSVersion")
        set(FRAMEWORK_MINIMUM_OS_VERSION_VALUE ${DEPLOYMENT_TARGET})
    else()
        set(FRAMEWORK_MINIMUM_OS_VERSION_KEY "LSMinimumSystemVersion")
        set(FRAMEWORK_MINIMUM_OS_VERSION_VALUE "10.12")
        set(FRAMEWORK_SUPPORTED_PLATFORMS "  <key>CFBundleSupportedPlatforms</key>
  <array>
    <string>MacOsX</string>
  </array>"
    )
    endif()
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
  FRAMEWORK TRUE
  FRAMEWORK_VERSION A
  # settings for Info.plist file
  # specify the template
  MACOSX_FRAMEWORK_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Info.plist.in
  MACOSX_FRAMEWORK_IDENTIFIER com.microsoft.cognitiveservices.SpeechSDK
  # CFBundleVersion
  MACOSX_FRAMEWORK_BUNDLE_VERSION ${SPEECHSDK_VERSION}
  # CFBundleShortVersionString
  MACOSX_FRAMEWORK_SHORT_VERSION_STRING ${SPEECHSDK_MAJOR}.${SPEECHSDK_MINOR}.${SPEECHSDK_PATCH}
  # "current version" in semantic format in Mach-O binary file
  VERSION ${SPEECHSDK_VERSION}
  # "compatibility version" in semantic format in Mach-O binary file
  SOVERSION ${SPEECHSDK_MAJOR}.0

  PUBLIC_HEADER "${ALL_FRAMEWORK_HEADERS}"
  PRIVATE_HEADER "${PRIVATE_HEADERS}"
  # settings for generating an Xcode project
  XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
  XCODE_ATTRIBUTE_PRODUCT_NAME ${SPEECHSDK_CORE_FRAMEWORK}
  XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES

  CXX_VISIBILITY_PRESET hidden
  C_VISIBILITY_PRESET hidden
  OBJC_VISIBILITY_PRESET hidden
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE interfaces
  PRIVATE sr
  PRIVATE audio
  PRIVATE usp
  PRIVATE common
  PRIVATE factory
  PRIVATE logging
  PRIVATE conversation_translation
  )

if(EXTENSION_CODEC_ENABLED)
  target_link_libraries(${PROJECT_NAME}
    PRIVATE codec
    )
endif()

if(EXTENSION_KWS_ENABLED)
  target_link_libraries(${PROJECT_NAME}
    PRIVATE kws_factory
    )
endif()

set_target_properties(${PROJECT_NAME}
    PROPERTIES BUILD_RPATH "@loader_path;@executable_path/Frameworks"
    )

get_target_property(FRAMEWORK_OUTPUT_DIR ${PROJECT_NAME} LIBRARY_OUTPUT_DIRECTORY)
add_custom_target(${PROJECT_NAME}-macos-headers ALL
    COMMAND sed -i \"\" -E "\'s@#include <(((speechapi_.*)|(spxerror)|(spxdebug))\.h)>@#include \"\\1\"@\'" ${FRAMEWORK_OUTPUT_DIR}/${PROJECT_NAME}.framework/Headers/*.h
)
add_dependencies(${PROJECT_NAME}-macos-headers ${PROJECT_NAME} )

# Install
if (CMAKE_IOS_INSTALL_COMBINED AND CMAKE_OSX_SYSROOT MATCHES "os")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        XCODE_ATTRIBUTE_ARCHS[sdk=iphonesimulator*] i386 x86_64
        XCODE_ATTRIBUTE_VALID_ARCHS[sdk=iphonesimulator*] i386 x86_64
        )
endif()

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    EXPORT speechsdk-objc-export
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    FRAMEWORK DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(EXPORT speechsdk-objc-export
  FILE
    SpeechSDKTargets.cmake
  NAMESPACE
    MicrosoftCognitiveServicesSpeech::
  DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cmake/MicrosoftCognitiveServicesSpeech
)

