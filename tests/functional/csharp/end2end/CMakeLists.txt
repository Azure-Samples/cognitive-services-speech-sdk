project(Microsoft.CognitiveServices.Speech.Tests.EndToEnd)

if(WIN32)
enable_language(CSharp)
set(SRC_DIR "${PROJECT_SOURCE_DIR}")
FILE(GLOB END2END_TEST_SOURCES "${SRC_DIR}/*.cs" "${SRC_DIR}/**/*.cs")
source_group("Source Files" FILES ${END2END_TEST_SOURCES})

add_library(${PROJECT_NAME} SHARED ${END2END_TEST_SOURCES})

find_program(NUGET nuget)
if(NUGET STREQUAL "NUGET-NOTFOUND")
  message(WARNING "Cannot find nuget command line tool, please install it or make sure to run nuget restore on solution.")
else()
  add_custom_target(nuget-restore
    COMMAND ${NUGET} restore ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.csproj -PackagesDirectory ${CMAKE_BINARY_DIR}/packages
  )
  SET(NUGET_RESTORE
    nuget-restore)
endif()

configure_file(${SRC_DIR}/packages.config
  ${CMAKE_CURRENT_BINARY_DIR}/packages.config COPYONLY)

target_compile_options(${PROJECT_NAME} PRIVATE "/langversion:6")
target_link_libraries(${PROJECT_NAME} ${CSHARP_LIB})

add_dependencies(${PROJECT_NAME} ${CSHARP_LIB} ${NUGET_RESTORE})

set(END2END_TEST_REFERENCES
  System
  System.Net.Http)

set(END2END_TEST_PROJECT_TYPES
  "{3AC096D0-A1C2-E12C-1390-A8335801FDAB}"
  "{FAE04EC0-301F-11D3-BF4B-00C04F79EFBC}")

set_target_properties(${PROJECT_NAME} PROPERTIES
  FOLDER tests
  VS_GLOBAL_PROJECT_TYPES "${END2END_TEST_PROJECT_TYPES}"
  VS_DOTNET_TARGET_FRAMEWORK_VERSION "v4.6.1"
  VS_DOTNET_REFERENCES "${END2END_TEST_REFERENCES}"
  VS_DOTNET_REFERENCE_TestFramework "${CMAKE_BINARY_DIR}/packages/MSTest.TestFramework.1.3.0/lib/net45/Microsoft.VisualStudio.TestPlatform.TestFramework.dll"
  VS_DOTNET_REFERENCE_TestFrameworkExtensions "${CMAKE_BINARY_DIR}/packages/MSTest.TestFramework.1.3.0/lib/net45/Microsoft.VisualStudio.TestPlatform.TestFramework.Extensions.dll")

endif()
