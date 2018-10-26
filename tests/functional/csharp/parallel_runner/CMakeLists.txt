project(Microsoft.CognitiveServices.Speech.Tests.ParallelRunner)

if(WIN32)
enable_language(CSharp)
set(SRC_DIR "${PROJECT_SOURCE_DIR}")
file(GLOB PARALLEL_RUNNER_SOURCES "${SRC_DIR}/*.cs" "${SRC_DIR}/**/*.cs")
source_group("Source Files" FILES ${PARALLEL_RUNNER_SOURCES})

add_executable(${PROJECT_NAME} ${PARALLEL_RUNNER_SOURCES})

target_compile_options(${PROJECT_NAME} PRIVATE /langversion:6 /warnaserror /warn:4)
target_link_libraries(${PROJECT_NAME} ${CSHARP_LIB})

add_dependencies(${PROJECT_NAME} ${CSHARP_LIB})

set(PARALLEL_RUNNER_REFERENCES
  System
  System.Net.Http)

set_target_properties(${PROJECT_NAME} PROPERTIES
  FOLDER tests
  VS_DOTNET_TARGET_FRAMEWORK_VERSION "v4.6.1"
  VS_DOTNET_REFERENCES "${PARALLEL_RUNNER_REFERENCES}"
  VS_DOTNET_REFERENCE_Speech "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)/net461/${CSHARP_LIB}.dll"
)

endif()
