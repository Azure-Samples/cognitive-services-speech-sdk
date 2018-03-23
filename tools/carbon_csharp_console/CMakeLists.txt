project(carbon_csharp_console)

if(WIN32)
enable_language(CSharp)
set(SRC_DIR "${PROJECT_SOURCE_DIR}")
set(SOURCES "${SRC_DIR}/carbon_csharp_console.cs")
source_group("Source Files" FILES ${SOURCES})

add_executable(${PROJECT_NAME} carbon_csharp_console.cs)
target_compile_options(${PROJECT_NAME} PRIVATE "/langversion:6")
target_link_libraries(${PROJECT_NAME} ${CSHARP_LIB})
add_dependencies(${PROJECT_NAME} ${CSHARP_LIB})

set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DOTNET_TARGET_FRAMEWORK_VERSION "v4.6.1")

set_target_properties(${PROJECT_NAME} PROPERTIES
    FOLDER tools
)
set_target_properties(${CSHARP_LIB} PROPERTIES
   VS_DOTNET_TARGET_FRAMEWORK_VERSION "v4.6.1"
)
endif()