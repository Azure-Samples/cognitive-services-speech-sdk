project(speechsdk_kws)

set(SDKKWS_SUPPORTED_TARGETS "Android-arm32" "Linux-arm64" "Linux-x86" "Linux-x64" "Windows-x86" "Windows-x64")
if(NOT SPEECHSDK_TARGET_PLATFORM IN_LIST SDKKWS_SUPPORTED_TARGETS)
  message(FATAL_ERROR "Unsupported target for KWS '${SPEECHSDK_TARGET_PLATFORM}', must be one of these: ${SDKKWS_SUPPORTED_TARGETS}")
endif()

set(SDKKWS_HEADERS "${PROJECT_SOURCE_DIR}/include")

add_subdirectory(kws_wrapper)
add_subdirectory(kws_factory)
