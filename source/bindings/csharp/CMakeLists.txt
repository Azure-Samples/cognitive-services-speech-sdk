project(Microsoft.CognitiveServices.Speech.csharp)

set(CSPROJFILENAME "Microsoft.CognitiveServices.Speech.csharp")
set(SRC_DIR "${PROJECT_SOURCE_DIR}")
set(CSHARP_BINDINGS_SRC_DIR "${SRC_DIR}/interop")
set(CSHARP_LIB ${PROJECT_NAME} CACHE INTERNAL "")
FILE(GLOB ALL_CSHARP_SOURCES "${SRC_DIR}/*.cs" "${CSHARP_BINDINGS_SRC_DIR}/*.cs")
source_group("Source Files" FILES ${ALL_CSHARP_SOURCES})
set(USECSPROJ TRUE)

set(INTEROP_CS_FILES "${SRC_DIR}/interop/*.cs")
set(BASE_CS_FILES "${SRC_DIR}/*.cs")
file(TO_NATIVE_PATH ${INTEROP_CS_FILES} NATIVE_INTEROP_CS_FILES)
file(TO_NATIVE_PATH ${BASE_CS_FILES} NATIVE_BASE_CS_FILES)

if (WIN32)
  if (USECSPROJ)
    # Copy the template cs file and compile it.
    set(NUGET_PACKAGE_BASE "$(USERPROFILE)/.nuget")
    if((CMAKE_SYSTEM_NAME STREQUAL "Windows") AND (SPEECHSDK_CRT STREQUAL "OS_UCRT"))
      set(OS_BUILD "OS_BUILD")
    endif()
    configure_file(${SRC_DIR}/${CSPROJFILENAME}.csproj.template ${CMAKE_CURRENT_BINARY_DIR}/${CSPROJFILENAME}.csproj)
    set(CSHARP_PROJECT_PATH ${CMAKE_CURRENT_BINARY_DIR}/${CSPROJFILENAME})
    set(CSHARP_LIB_CUSTOM "${PROJECT_NAME}_CUSTOM" CACHE INTERNAL "")
    add_custom_target(${CSHARP_LIB_CUSTOM} ALL
      COMMAND ${CMAKE_MAKE_PROGRAM} /t:Restore "${CSHARP_PROJECT_PATH}.csproj"
      COMMAND ${CMAKE_MAKE_PROGRAM} /p:Configuration=$<CONFIGURATION> "${CSHARP_PROJECT_PATH}.csproj"
      COMMAND ${CMAKE_MAKE_PROGRAM} /p:Configuration=$<CONFIGURATION>UnixOS "${CSHARP_PROJECT_PATH}.csproj"
      COMMAND ${CMAKE_MAKE_PROGRAM} /p:Configuration=$<CONFIGURATION>IOS "${CSHARP_PROJECT_PATH}.csproj"
      COMMAND del /S /Q obj
      COMMENT "Building external C# project")
    add_dependencies(${CSHARP_LIB_CUSTOM} ${SPEECHSDK_CORE_LIBRARY})
    enable_language(CSharp)
    add_library(${CSHARP_LIB} SHARED IMPORTED GLOBAL)
    add_dependencies(${CSHARP_LIB} ${CSHARP_LIB_CUSTOM})
    if(SPECTRE_MITIGATION)
      set_property(TARGET ${CSHARP_LIB} PROPERTY VS_GLOBAL_SpectreMitigation "Spectre")
    endif()
    # Ensure we build with debug information (TODO global setting does not apply?)
    set_target_properties(${CSHARP_LIB} PROPERTIES LINK_FLAGS "/DEBUG /OPT:REF /OPT:ICF")
    set_target_properties(${CSHARP_LIB} PROPERTIES IMPORTED_LOCATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)/net46/${PROJECT_NAME}.dll")
  else()
    enable_language(CSharp)
    add_library(${CSHARP_LIB} ${ALL_CSHARP_SOURCES})
    set(CMAKE_CSharp_FLAGS "/langversion:7")
    target_compile_options(${CSHARP_LIB} PRIVATE /warnaserror /warn:4)
    add_dependencies(${CSHARP_LIB} ${SPEECHSDK_CORE_LIBRARY})
    link_libraries(${CSHARP_LIB} ${SPEECHSDK_CORE_LIBRARY})
    set_target_properties(${CSHARP_LIB} PROPERTIES LINK_FLAGS "/DEBUG /OPT:REF /OPT:ICF")
    set_target_properties(${CSHARP_LIB} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    set_target_properties(${CSHARP_LIB} PROPERTIES
      FOLDER bindings/csharp
    )
    if(SPECTRE_MITIGATION)
      set_property(TARGET ${CSHARP_LIB} PROPERTY VS_GLOBAL_SpectreMitigation "Spectre")
    endif()
    set_property(TARGET ${CSHARP_LIB} PROPERTY VS_DOTNET_TARGET_FRAMEWORK_VERSION "v4.6.1")
    set_target_properties(${CSHARP_LIB} PROPERTIES
      VS_DOTNET_REFERENCES "System;System.Net.Http;System.Runtime.Serialization;System.Runtime.Serialization.Json;System.Xml"
    )
  endif()
elseif(APPLE)
  # Copy the template cs file and compile it.
  set(NUGET_PACKAGE_BASE ${CMAKE_BINARY_DIR})
  configure_file(${SRC_DIR}/${CSPROJFILENAME}.csproj.template ${CMAKE_CURRENT_BINARY_DIR}/${CSPROJFILENAME}.csproj)
  set(CSHARP_PROJECT_PATH ${CMAKE_CURRENT_BINARY_DIR}/${CSPROJFILENAME})

  find_program(DOTNET dotnet)
  if(DOTNET STREQUAL "DOTNET-NOTFOUND")
      message(WARNING "Cannot find dotnet command line tool")
  else()
    add_custom_target(nuget-restore
        COMMAND ${DOTNET} restore --packages ${CMAKE_BINARY_DIR}/packages ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.csproj
    )
    SET(NUGET_RESTORE
      nuget-restore)
    add_custom_target(${CSHARP_LIB} ALL
        COMMAND ${DOTNET} build -c "$<CONFIGURATION>MacOS" -p:UseSharedCompilation=false "${CSHARP_PROJECT_PATH}.csproj"
        COMMENT "Building external C# project"
        )
    add_dependencies(${CSHARP_LIB} ${NUGET_RESTORE})
  endif()
endif()

