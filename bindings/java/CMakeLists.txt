project(carbon_java)

find_package(SWIG REQUIRED)
find_package(Java REQUIRED)
find_package(JNI REQUIRED)
include(UseJava)
include(UseSWIG)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")
set(INC_DIR "${PROJECT_SOURCE_DIR}/include")

set( CMAKE_SWIG_OUTDIR ${CMAKE_CURRENT_BINARY_DIR} )

set(SOURCES "${SRC_DIR}/carbon_java.i")

if(MSVC)
     # Force to always compile with W4
     set(CMAKE_CXX_FLAGS "/W3")
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
     set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra -Wno-unused-parameter -fPIC")
endif()

set(JAVA_BINDINGS ${PROJECT_NAME}_bindings)

set(SWIG_MODULE_${JAVA_BINDINGS}_EXTRA_DEPS
    ${SWIG_INTERFACE}
    ${SOURCES}
    ${SPEECH_C_API_HEADERS}
    ${SPEECH_CXX_API_HEADERS})

# ${SWIG_INTERFACE} should resolve to {repo/root}/bindings/carbon.i
set(CMAKE_SWIG_FLAGS -package Carbon)
set_source_files_properties("${SRC_DIR}/carbon_java.i" PROPERTIES CPLUSPLUS ON)

# Make sure the nested directory structure exists
set(JAVA_SOURCE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/swig-generated CACHE INTERNAL "")
file(REMOVE_RECURSE ${JAVA_SOURCE_DIRECTORY})
file(MAKE_DIRECTORY ${JAVA_SOURCE_DIRECTORY})

# Create swig target
set(CMAKE_SWIG_OUTDIR ${JAVA_SOURCE_DIRECTORY})

include_directories(${INC_DIR})
include_directories(${CARBON_C_API})
include_directories(${CARBON_CXX_API})
include_directories(${JAVA_INCLUDE_PATH})
include_directories(${JNI_INCLUDE_DIRS})
include_directories("${SRC_DIR}/..")

# Build the C++ code into a dynamic library: example.dll (on Windows) or libexample.so (on Linux)
swig_add_library(${JAVA_BINDINGS} LANGUAGE java SOURCES ${SOURCES})
swig_link_libraries(${JAVA_BINDINGS} carbon)
set_target_properties(${SWIG_MODULE_${JAVA_BINDINGS}_REAL_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set_target_properties(${SWIG_MODULE_${JAVA_BINDINGS}_REAL_NAME} PROPERTIES FOLDER bindings/java)


# Build the Java code into ${JAVA_LIB}.jar
set(JAVA_LIB ${PROJECT_NAME}_lib)

# TODO this should be a wildcard, not an explicit list as these files are generated
#set(SWIG_JAVA_SOURCES ${JAVA_SOURCE_DIRECTORY}/*.java)
#file(GLOB SWIG_JAVA_SOURCES "${CMAKE_CURRENT_BINARY_DIR}/swig-generated/*.java")
set(SWIG_JAVA_SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/IntentEventListener.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/RecognitionEventListener.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/SessionEventListener.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/SpeechRecognitionEventListener.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/BaseAsyncRecognizer.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/BaseRecognizerBase.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/carbon_java.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/carbon_javaJNI.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/EventArgs.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/IntentEventSignal.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/IntentRecognizer.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/IntentRecognizerBase.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/IntPtrFuture.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/Parameter.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/PayloadItems.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/Reason.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/RecognitionEventArgs.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/RecognitionEventSignal.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/RecognitionResult.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/Recognizer.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/RecognizerFactory.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/RecognizerParameter.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/RecognizerParameterCollection.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/Session.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/SessionEventArgs.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/SessionEventSignal.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/SessionParameter.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/SessionParameterCollection.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/SpeechRecognitionEventArgs.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/SpeechRecognitionEventSignal.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/SpeechRecognitionResult.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/SpeechRecognitionResultPtrFuture.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/SpeechRecognizer.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/SpeechRecognizerBase.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/SWIGTYPE_p_int32_t.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/SWIGTYPE_p_SPXEVENTHANDLE.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/SWIGTYPE_p_SPXRECOHANDLE.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/SWIGTYPE_p_SPXRESULTHANDLE.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/SWIGTYPE_p_SPXSESSIONHANDLE.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/SWIGTYPE_p_std__shared_ptrT_Carbon__Session_t.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/SWIGTYPE_p_std__shared_ptrT_int_t.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/SWIGTYPE_p_wchar_t.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/VoidFuture.java
)


SET_SOURCE_FILES_PROPERTIES(${SWIG_JAVA_SOURCES} PROPERTIES GENERATED TRUE)


enable_language(Java)
add_jar(${JAVA_LIB} SOURCES ${SWIG_JAVA_SOURCES})
add_dependencies(${JAVA_LIB} ${JAVA_BINDINGS} ${SWIG_MODULE_${JAVA_BINDINGS}_REAL_NAME})

set_target_properties(${SWIG_MODULE_${JAVA_LIB}_REAL_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set_target_properties(${JAVA_LIB} PROPERTIES FOLDER bindings/java)

set_property(DIRECTORY ${CMAKE_SOURCE_DIR} APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${SWIG_INTERFACE})
