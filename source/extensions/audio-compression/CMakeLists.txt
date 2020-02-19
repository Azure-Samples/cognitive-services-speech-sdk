project(speechsdk_compression)

set(SDKCOMPRESSION_SUPPORTED_TARGETS
  "Android-arm32"
  "Android-arm64"
  "Android-x64"
  "Android-x86"
  "IOS-all"
  "IOSSIMULATOR-x64"
  "Linux-arm64"
  "Linux-x86"
  "Linux-x64"
  "Windows-x86"
  "Windows-x64"
  "WindowsUwp-arm32"
  "WindowsUwp-arm64"
  "WindowsUwp-x64"
  "WindowsUwp-x86"
  )

if(NOT SPEECHSDK_TARGET_PLATFORM IN_LIST SDKCOMPRESSION_SUPPORTED_TARGETS)
  message(FATAL_ERROR "Unsupported target for KWS '${SPEECHSDK_TARGET_PLATFORM}', must be one of these: ${SDKCOMPRESSION_SUPPORTED_TARGETS}")
endif()

set(SDKCOMPRESSION_HEADERS "${PROJECT_SOURCE_DIR}/include")

add_subdirectory(silk)

