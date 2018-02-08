project(carbon_csharp_console)

if(WIN32)
enable_language(CSharp)
set(SRC_DIR "${PROJECT_SOURCE_DIR}")
set(SOURCES "${SRC_DIR}/carbon_csharp_console.cs")
source_group("Source Files" FILES ${SOURCES})

add_executable(${PROJECT_NAME} carbon_csharp_console.cs)
target_link_libraries(${PROJECT_NAME} carbon_csharp_lib)

set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DOTNET_TARGET_FRAMEWORK_VERSION "v4.6.1")

set_target_properties (${PROJECT_NAME} PROPERTIES
    FOLDER tools
)
endif()