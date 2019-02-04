project(kws_wrapper)

# Using a function to limit the scope of the variables
function(create_kwsapi_ddk_target)
  set(_root "${CARBON_ROOT}/external/KWS")

  if(SPEECHSDK_TARGET_PLATFORM STREQUAL "Android-arm32")
    set(_target "Android_ARM32")
    set(_link_lib log)
  elseif(SPEECHSDK_TARGET_PLATFORM STREQUAL "Linux-x86")
    set(_target "Linux_x86")
    set(_link_lib)
  elseif(SPEECHSDK_TARGET_PLATFORM STREQUAL "Linux-x64")
    set(_target "Linux_x86_64")
    set(_link_lib)
  elseif(SPEECHSDK_TARGET_PLATFORM STREQUAL "Windows-x64")
    set(_target "Windows_Win64")
    set(_link_lib)
  elseif(SPEECHSDK_TARGET_PLATFORM STREQUAL "Windows-x86")
    set(_target "Windows_Win32")
    set(_link_lib)
  endif()

  set(_link_debug "${_root}/${_target}_Debug/targets/DDK")
  set(_link_release "${_root}/${_target}_Release/targets/DDK")
  if(SPEECHSDK_OS STREQUAL "Windows")
      # Windows KWS artifacts are "multi-config"
      set(_link_debug "${_link_debug}/Debug")
      set(_link_release "${_link_release}/Release")
  endif()

  set(_libname "${CMAKE_SHARED_LIBRARY_PREFIX}kwsapi_ddk${CMAKE_STATIC_LIBRARY_SUFFIX}")

  add_library(kwsapi_ddk STATIC IMPORTED)
  set_target_properties(kwsapi_ddk PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${_root}/${_target}_$<IF:$<CONFIG:Debug>,Debug,Release>/targets/DDK"
    IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG CXX
    IMPORTED_LOCATION_DEBUG "${_link_debug}/${_libname}"
    IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE CXX
    IMPORTED_LOCATION_RELEASE "${_link_release}/${_libname}"
    INTERFACE_LINK_LIBRARIES "${_link_lib}"
    IMPORTED_CONFIGURATIONS "DEBUG;RELEASE"
    MAP_IMPORTED_CONFIG_MINSIZEREL Release
    MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release)
endfunction()
create_kwsapi_ddk_target()

set(HEADERS)

set(HEADERS_PRIVATE
    stdafx.h
    )

set(SOURCES
    kws_engine_adapter.cpp
    stdafx.cpp
    )

source_group("Header Files (public)" FILES ${HEADERS})
source_group("Header Files (private)" FILES ${HEADERS_PRIVATE})
source_group("Source Files" FILES ${SOURCES})

add_library(kws_wrapper STATIC ${HEADERS} ${HEADERS_PRIVATE} ${SOURCES})

target_include_directories(kws_wrapper
    PRIVATE
        ${CARBON_C_API}
        ${CARBON_CXX_API}
        ${CARBON_INCLUDE}
        ${COMMON}/include
    PUBLIC
        ${SDKKWS_HEADERS}
    )

set_target_properties(kws_wrapper PROPERTIES
    DEFINE_SYMBOL "SPX_CONFIG_EXPORTAPIS"
    FOLDER "extensions/kws"
    )

target_link_libraries(kws_wrapper PRIVATE
    ${CARBON_COMMON_LIB}
    ${CARBON_CORE_LIB}
    kwsapi_ddk
    )
