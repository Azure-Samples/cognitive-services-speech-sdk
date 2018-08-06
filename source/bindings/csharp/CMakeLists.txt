project(Microsoft.CognitiveServices.Speech.csharp)

find_package(SWIG REQUIRED)
include(UseSWIG)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")
set(CSHARP_LIB ${PROJECT_NAME} CACHE INTERNAL "")

# Build the cxx binding dll via swig
set(SOURCES "${SRC_DIR}/carbon_csharp.i")
set(CSHARP_BINDINGS ${PROJECT_NAME}.bindings)
# ${SWIG_INTERFACE} and ${SWIG_WRAPPERS} point to common interface / header files
set(SWIG_MODULE_${CSHARP_BINDINGS}_EXTRA_DEPS
    ${SWIG_INTERFACE}
    ${SWIG_WRAPPERS}
    "${SRC_DIR}/carbon_csharp.i"
    ${SPEECH_C_API_HEADERS}
    ${SPEECH_CXX_API_HEADERS})

# ${SWIG_INTERFACE} should resolve to {repo/root}/bindings/carbon.i
# warning 302: identifier redefined by ...
set(CMAKE_SWIG_FLAGS
   -w401,516,503,302
   -namespace Microsoft.CognitiveServices.Speech.Internal
   -dllimport "${CSHARP_BINDINGS}.dll")
set_source_files_properties("${SRC_DIR}/carbon_csharp.i" PROPERTIES
    CPLUSPLUS ON)

# Make sure the nested directory structure exists
set(CSHARP_SOURCE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/swig-generated CACHE INTERNAL "")
file(REMOVE_RECURSE ${CSHARP_SOURCE_DIRECTORY})
file(MAKE_DIRECTORY ${CSHARP_SOURCE_DIRECTORY})

# Create swig target
set(CMAKE_SWIG_OUTDIR ${CSHARP_SOURCE_DIRECTORY})

include_directories(${CARBON_C_API})
include_directories(${CARBON_CXX_API})
include_directories("${SRC_DIR}/..")

if(WIN32)
  swig_add_library(${CSHARP_BINDINGS} LANGUAGE csharp SOURCES ${SOURCES})
  swig_link_libraries(${CSHARP_BINDINGS} ${SPEECHSDK_CORE_LIBRARY})
else()
  # Due to .netcore loading on Linux, it currently does not support side-by-side located
  # native dependencies, so we compile everything into a single so.
  swig_add_library(${CSHARP_BINDINGS} LANGUAGE csharp SOURCES ${SOURCES} $<TARGET_OBJECTS:c_api>)
  swig_link_libraries(${CSHARP_BINDINGS}
    PRIVATE sr
    PRIVATE audio 
    PRIVATE usp
    PRIVATE common 
    PRIVATE factory)
endif()

set_target_properties(${SWIG_MODULE_${CSHARP_BINDINGS}_REAL_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set_target_properties(${SWIG_MODULE_${CSHARP_BINDINGS}_REAL_NAME} PROPERTIES
    FOLDER bindings/csharp
)

# Copy the template cs file and compile it.
configure_file(${SRC_DIR}/Microsoft.CognitiveServices.Speech.csharp.csproj.template
  ${CMAKE_CURRENT_BINARY_DIR}/Microsoft.CognitiveServices.Speech.csharp.csproj)

if(WIN32)
    set(SWIG_CS_SOURCES ${CSHARP_SOURCE_DIRECTORY}/*.cs)
    set(ALL_CSHARP_SOURCES ${SRC_DIR}/*.cs ${SWIG_CS_SOURCES})

    set(CSHARP_PROJECT_PATH ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}) 
    
    set(CSHARP_LIB_CUSTOM "${PROJECT_NAME}_CUSTOM" CACHE INTERNAL "")
    add_custom_target(${CSHARP_LIB_CUSTOM} ALL
      COMMAND ${CMAKE_MAKE_PROGRAM} /t:Restore "${CSHARP_PROJECT_PATH}.csproj"
      COMMAND ${CMAKE_MAKE_PROGRAM} /p:Configuration=$<CONFIGURATION> "${CSHARP_PROJECT_PATH}.csproj"
      COMMAND ${CMAKE_MAKE_PROGRAM} /p:Configuration=$<CONFIGURATION>Utf32 "${CSHARP_PROJECT_PATH}.csproj"
      COMMAND del /S /Q obj
      COMMENT "Building external C# project")
    add_dependencies(${CSHARP_LIB_CUSTOM} ${CSHARP_BINDINGS})
    
    enable_language(CSharp)
    add_library(${CSHARP_LIB} SHARED IMPORTED GLOBAL)
    add_dependencies(${CSHARP_LIB} ${CSHARP_LIB_CUSTOM})
    set_target_properties(${CSHARP_LIB} PROPERTIES IMPORTED_LOCATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)/net46/${PROJECT_NAME}.dll")
endif()
