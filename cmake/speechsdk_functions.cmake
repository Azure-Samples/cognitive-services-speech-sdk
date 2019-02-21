# On Windows, gets architecture selected by generator / architecture parameter.
# N.B. we don't care about < VS 2017
function(GET_WINDOWS_PLATFORM OUTVAR)
  if(CMAKE_HOST_WIN32)
    if(CMAKE_GENERATOR MATCHES " Win64$")
      set(_arch x64)
    elseif(CMAKE_GENERATOR MATCHES " ARM$")
      set(_arch arm32)
    elseif(CMAKE_GENERATOR STREQUAL "Visual Studio 15 2017")
      # Handle "cmake -A"
      # (note: seems case insensitive at first as an option, but there's some internal logic which is not)
      if(CMAKE_GENERATOR_PLATFORM STREQUAL "Win32")
        set(_arch x86)
      elseif(CMAKE_GENERATOR_PLATFORM STREQUAL "x64")
        set(_arch x64)
      elseif(CMAKE_GENERATOR_PLATFORM STREQUAL "ARM")
        set(_arch arm32)
      elseif(CMAKE_GENERATOR_PLATFORM STREQUAL "ARM64")
        set(_arch arm64)
      else()
        set(_arch x86)
      endif()
    else()
      set(_arch x86)
    endif()
  else()
    message(FATAL_ERROR "this should be only called on Windows hosts")
  endif()
  set(${OUTVAR} "${_arch}" PARENT_SCOPE)
endfunction()

# Get the default for the local platform. This is meant to be a guess based on
# the host platform (and generator, in case of Windows).
# Anything "unusual" people need to specify explicitly.
function(GET_LOCAL_PLATFORM OUTVAR)
  if(NOT DEFINED OUTVAR)
    message(FATAL_ERROR "missing parameter OUTVAR")
  endif()
  if(CMAKE_HOST_WIN32)
    get_windows_platform(_arch)
    set(${OUTVAR} "Windows-${_arch}" PARENT_SCOPE)
  elseif(CMAKE_HOST_APPLE)
    set(${OUTVAR} "OSX-x64" PARENT_SCOPE)
  elseif(CMAKE_HOST_UNIX)
    set(${OUTVAR} "Linux-x64" PARENT_SCOPE)
  else()
    # No default. This should error out later.
    set(${OUTVAR} "" PARENT_SCOPE)
  endif()
endfunction()

# Read version from version.txt, strip any pre-release portion, append
# -alpha.0.1 and return in the specified output variable. This is the version
# used for local builds.
function(GET_LOCAL_VERSION OUTVAR)
  if(NOT DEFINED OUTVAR)
    message(FATAL_ERROR "missing parameter OUTVAR")
  endif()
  # Read up to two lines from version.txt.
  file(STRINGS version.txt LINES LIMIT_COUNT 2)
  list(LENGTH LINES COUNT)
  if(COUNT GREATER 1)
    message(FATAL_ERROR "Found more than one line in version.txt; must only be one.")
  endif()
  set(NUM_RE "(0|[1-9][0-9]*)")
  set(NUM1_RE "([1-9][0-9]*)")
  set(MMP_RE "${NUM_RE}\\.${NUM_RE}\\.${NUM_RE}")
  if(NOT LINES MATCHES "^(${MMP_RE})(-(alpha|beta|rc)\\.${NUM1_RE})?$")
    message(FATAL_ERROR "bad version number in version.txt: ${LINES}")
  endif()
  set(${OUTVAR} "${CMAKE_MATCH_1}-alpha.0.1" PARENT_SCOPE)
endfunction()

# Parse version and validate with respect to the build type (local, dev, int,
# prod). Return the version components in variables with the specified prefix:
#
# * ${PREFIX}_MAJOR    - major version
# * ${PREFIX}_MINOR    - minor version
# * ${PREFIX}_PATCH    - patch version
# * ${PREFIX}_VTAG     - revision for file/assembly version (8 local, 12 dev, 16 int, 25..28 prod alpha/beta/rc/release)
# * ${PREFIX}_ISPRERELEASE - 1 if pre-release version, 0 otherwise.
# * ${PREFIX}_PRERELEASE - pre-release portion of the version number or empty. 1 if pre-release version, 0 otherwise.
function(PARSE_VERSION)
  cmake_parse_arguments(OPT "" "PREFIX;BUILD_TYPE;VERSION" "" ${ARGN})
  if(NOT (DEFINED OPT_BUILD_TYPE AND DEFINED OPT_VERSION AND DEFINED OPT_PREFIX))
    message(FATAL_ERROR "missing parameter for parse_version")
  endif()
  set(NUM_RE "0|[1-9][0-9]*")
  if(OPT_VERSION MATCHES
      "^(${NUM_RE})\\.(${NUM_RE})\\.(${NUM_RE})(-(alpha|beta|rc)\\.(${NUM_RE})(\\.(${NUM_RE}))?)?$")
    if(OPT_BUILD_TYPE STREQUAL "local")
      set("${OPT_PREFIX}_ISPRIVATE" 1 PARENT_SCOPE)
      if(NOT "${CMAKE_MATCH_5}.${CMAKE_MATCH_6}${CMAKE_MATCH_7}" STREQUAL "alpha.0.1")
        message(FATAL_ERROR "version number for build type '${OPT_BUILD_TYPE}' must be MAJOR.MINOR.PATCH-alpha.0.1 but is '${OPT_VERSION}'")
      endif()
    elseif(OPT_BUILD_TYPE STREQUAL "dev")
      set("${OPT_PREFIX}_ISPRIVATE" 1 PARENT_SCOPE)
      if(NOT (("${CMAKE_MATCH_5}.${CMAKE_MATCH_6}" STREQUAL "alpha.0") AND (DEFINED CMAKE_MATCH_7)))
        message(FATAL_ERROR "version number for build type '${OPT_BUILD_TYPE}' must be MAJOR.MINOR.PATCH-alpha.0.BUILDID but is '${OPT_VERSION}'")
      endif()
    elseif(OPT_BUILD_TYPE STREQUAL "int")
      set("${OPT_PREFIX}_ISPRIVATE" 1 PARENT_SCOPE)
      if(NOT (("${CMAKE_MATCH_5}.${CMAKE_MATCH_6}" STREQUAL "beta.0") AND (DEFINED CMAKE_MATCH_7)))
        message(FATAL_ERROR "version number for build type '${OPT_BUILD_TYPE}' must be MAJOR.MINOR.PATCH-beta.0.BUILDID but is '${OPT_VERSION}'")
      endif()
    elseif(OPT_BUILD_TYPE STREQUAL "prod")
      set("${OPT_PREFIX}_ISPRIVATE" 0 PARENT_SCOPE)
      if((DEFINED CMAKE_MATCH_4) AND (DEFINED CMAKE_MATCH_7 OR CMAKE_MATCH_6 EQUAL 0))
        message(FATAL_ERROR "version number for build type '${OPT_BUILD_TYPE}' must be MAJOR.MINOR.PATCH-(alpha|beta|rc).NUM' ${OPT_VERSION}'")
      endif()
    else()
      message(FATAL_ERROR "unexpected build type '${OPT_BUILD_TYPE}', must be 'local', 'dev', 'int', or 'prod'.")
    endif()
    set("${OPT_PREFIX}_MAJOR" ${CMAKE_MATCH_1} PARENT_SCOPE)
    set("${OPT_PREFIX}_MINOR" ${CMAKE_MATCH_2} PARENT_SCOPE)
    set("${OPT_PREFIX}_PATCH" ${CMAKE_MATCH_3} PARENT_SCOPE)
    if(DEFINED CMAKE_MATCH_4)
      set("${OPT_PREFIX}_ISPRERELEASE" 1 PARENT_SCOPE)
    else()
      set("${OPT_PREFIX}_ISPRERELEASE" 0 PARENT_SCOPE)
    endif()
    set("${OPT_PREFIX}_PRERELEASE" ${CMAKE_MATCH_4} PARENT_SCOPE)
  else()
    message(FATAL_ERROR "cannot parse version '${OPT_VERSION}'")
  endif()

  if(OPT_BUILD_TYPE STREQUAL "local")
    set("${OPT_PREFIX}_VTAG" 8 PARENT_SCOPE)
  elseif(OPT_BUILD_TYPE STREQUAL "dev")
    set("${OPT_PREFIX}_VTAG" 12 PARENT_SCOPE)
  elseif(OPT_BUILD_TYPE STREQUAL "int")
    set("${OPT_PREFIX}_VTAG" 16 PARENT_SCOPE)
  elseif(OPT_BUILD_TYPE STREQUAL "prod")
    if(OPT_VERSION MATCHES "-alpha")
      set("${OPT_PREFIX}_VTAG" 25 PARENT_SCOPE)
    elseif(OPT_VERSION MATCHES "-beta")
      set("${OPT_PREFIX}_VTAG" 26 PARENT_SCOPE)
    elseif(OPT_VERSION MATCHES "-rc")
      set("${OPT_PREFIX}_VTAG" 27 PARENT_SCOPE)
    else()
      set("${OPT_PREFIX}_VTAG" 28 PARENT_SCOPE)
    endif()
  else()
    message(FATAL_ERROR "cannot determine revision number")
  endif()
endfunction()

# Parse version and validate with respect to the build type (local, dev, int,
# prod). Return the platform components in variables with the specified prefix:
#
# * ${PREFIX}_OS - OS to build for
# * ${PREFIX}_ARCH - processor architecture
# TODO Should we try to infer ARCH if not given?
function(PARSE_PLATFORM)
  cmake_parse_arguments(OPT "" "PREFIX;TARGET_PLATFORM" "" ${ARGN})
  if(NOT (DEFINED OPT_PREFIX AND DEFINED OPT_TARGET_PLATFORM))
    message(FATAL_ERROR "missing parameter for parse_platform")
  endif()
  set(PLATFORMS
    Windows-x86
    Windows-x64
    WindowsUwp-x86
    WindowsUwp-x64
    WindowsUwp-arm32
    WindowsUwp-arm64
    Linux-x86
    Linux-x64
    Linux-arm32
    OSX-x64
    Android-x86
    Android-x64
    Android-arm32
    Android-arm64
    IOSSIMULATOR-x64
    IOSSIMULATOR-x86
    IOS-all
    IOS-arm64)

  if(NOT OPT_TARGET_PLATFORM IN_LIST PLATFORMS)
    message(FATAL_ERROR "unsupported platform '${OPT_TARGET_PLATFORM}', pick one of these: ${PLATFORMS}")
  endif()

  if(OPT_TARGET_PLATFORM MATCHES "^(.*)-(.*)$")
    set(_os "${CMAKE_MATCH_1}")
    set(_arch "${CMAKE_MATCH_2}")
  else()
    # Should not happen based on earlier check.
    message(FATAL_ERROR "internal error")
  endif()

  # Check that the host can build the requested platform.
  if(CMAKE_HOST_WIN32)
    # Windows can build Android or Windows.
    get_windows_platform(_generator_arch)
    if(_os MATCHES "^Windows")
      if(NOT _arch STREQUAL _generator_arch)
        message(FATAL_ERROR "Can't build '${OPT_TARGET_PLATFORM}' with ${CMAKE_GENERATOR} ${CMAKE_GENERATOR_PLATFORM}, pick the right generator.")
      endif()
    elseif(NOT _os STREQUAL "Android")
      message(FATAL_ERROR "Can't build '${OPT_TARGET_PLATFORM}' on Windows")
    endif()
  elseif(CMAKE_HOST_APPLE)
    # Apple can build IOS or OSX.
    if(NOT _os MATCHES "^(OSX|IOS|IOSSIMULATOR)$")
      message(FATAL_ERROR "Can't build '${OPT_TARGET_PLATFORM}' on Apple")
    endif()
  elseif(CMAKE_HOST_UNIX)
    # Unix (Linux) can build itself
    # TODO Android should work too, but we haven't tested (Android in VSTS is built from Windows host).
    if(NOT _os STREQUAL "Linux")
      message(FATAL_ERROR "Can't build '${OPT_TARGET_PLATFORM}' on Linux")
    endif()
    # TODO need to check no cross-compiling
  endif()

  # Set outputs
  set("${OPT_PREFIX}_ARCH" "${_arch}" PARENT_SCOPE)
  set("${OPT_PREFIX}_OS" "${_os}" PARENT_SCOPE)

endfunction()
