project(carbon_csharp_console)

if(WIN32)
enable_language(CSharp)
set(SRC_DIR "${PROJECT_SOURCE_DIR}")
FILE(GLOB CARBON_CSHARP_CONSOLE_SOURCES "${SRC_DIR}/*.cs")
source_group("Source Files" FILES ${CARBON_CSHARP_CONSOLE_SOURCES})

add_executable(${PROJECT_NAME} 
    ${CARBON_CSHARP_CONSOLE_SOURCES}
)

target_compile_options(${PROJECT_NAME} PRIVATE /langversion:6 /warnaserror /warn:4)
target_link_libraries(${PROJECT_NAME} ${CSHARP_LIB})
add_dependencies(${PROJECT_NAME} ${CSHARP_LIB})

set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DOTNET_TARGET_FRAMEWORK_VERSION "v4.6.1")

set_target_properties(${PROJECT_NAME} PROPERTIES
    FOLDER tools
)

set_target_properties(${PROJECT_NAME} PROPERTIES
   VS_DOTNET_REFERENCES "System;System.Net.Http"
   VS_DOTNET_REFERENCE_Speech "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)/net461/${CSHARP_LIB}.dll"
)

endif()
