project(carbon_java)

find_package(SWIG REQUIRED)
find_package(Java REQUIRED)

if(WIN32 OR UNIX AND NOT ANDROID)
    find_package(JNI REQUIRED)
endif()

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
     set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra -Wno-unused-function -Wno-unused-parameter -fno-strict-aliasing -fPIC")
endif()

set(JAVA_BINDINGS ${PROJECT_NAME}_bindings)

set(SWIG_MODULE_${JAVA_BINDINGS}_EXTRA_DEPS
    ${SWIG_INTERFACE}
    ${SOURCES}
    ${SPEECH_C_API_HEADERS}
    ${SPEECH_CXX_API_HEADERS})

# ${SWIG_INTERFACE} should resolve to {repo/root}/bindings/carbon.i
set(CMAKE_SWIG_FLAGS -w401,516,503 -package "com.microsoft.cognitiveservices.speech.internal")
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

# Build the C++ code start into a dynamic library: carbon.dll (on Windows) or libcarbon.so (on Linux)
if("${CMAKE_VERSION}" VERSION_GREATER 3.8)
    swig_add_library(${JAVA_BINDINGS} LANGUAGE java SOURCES ${SOURCES})
else()
    swig_add_module(${JAVA_BINDINGS} java ${SOURCES})
endif()
swig_link_libraries(${JAVA_BINDINGS} libcarbon)
set_target_properties(${SWIG_MODULE_${JAVA_BINDINGS}_REAL_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set_target_properties(${SWIG_MODULE_${JAVA_BINDINGS}_REAL_NAME} PROPERTIES FOLDER bindings/java)


# Build the Java code into ${JAVA_LIB}.jar
set(JAVA_LIB ${PROJECT_NAME}_lib)

# TODO this should be a wildcard, not an explicit list as these files are generated
# set(SWIG_JAVA_SOURCES ${JAVA_SOURCE_DIRECTORY}/*.java)
# SET_SOURCE_FILES_PROPERTIES(${SWIG_JAVA_SOURCES} PROPERTIES GENERATED TRUE)
#file(GLOB SWIG_JAVA_SOURCES "${CMAKE_CURRENT_BINARY_DIR}/swig-generated/*.java")
set(JAVA_SOURCES
    com/microsoft/cognitiveservices/speech/ParameterCollection.java
    com/microsoft/cognitiveservices/speech/SessionEventArgs.java
    com/microsoft/cognitiveservices/speech/SessionEventType.java
    com/microsoft/cognitiveservices/speech/recognition/AudioInputStream.java
    com/microsoft/cognitiveservices/speech/recognition/IntentModel.java
    com/microsoft/cognitiveservices/speech/recognition/ParameterNames.java
    com/microsoft/cognitiveservices/speech/recognition/RecognitionErrorEventArgs.java
    com/microsoft/cognitiveservices/speech/recognition/RecognitionStatus.java
    com/microsoft/cognitiveservices/speech/recognition/Recognizer.java
    com/microsoft/cognitiveservices/speech/recognition/RecognizerFactory.java
    com/microsoft/cognitiveservices/speech/recognition/TranslationRecognizer.java
    com/microsoft/cognitiveservices/speech/recognition/intent/IntentRecognitionResult.java
    com/microsoft/cognitiveservices/speech/recognition/intent/IntentRecognitionResultEventArgs.java
    com/microsoft/cognitiveservices/speech/recognition/intent/IntentRecognizer.java
    com/microsoft/cognitiveservices/speech/recognition/speech/SpeechRecognitionReason.java
    com/microsoft/cognitiveservices/speech/recognition/speech/SpeechRecognitionResult.java
    com/microsoft/cognitiveservices/speech/recognition/speech/SpeechRecognitionResultEventArgs.java
    com/microsoft/cognitiveservices/speech/recognition/speech/SpeechRecognizer.java
    com/microsoft/cognitiveservices/speech/recognition/translation/TranslationSynthesisResult.java
    com/microsoft/cognitiveservices/speech/recognition/translation/TranslationSynthesisResultEventArgs.java
    com/microsoft/cognitiveservices/speech/recognition/translation/TranslationTextResult.java
    com/microsoft/cognitiveservices/speech/recognition/translation/TranslationTextResultEventArgs.java
    com/microsoft/cognitiveservices/speech/util/EventHandler.java
    com/microsoft/cognitiveservices/speech/util/EventHandlerImpl.java
    com/microsoft/cognitiveservices/speech/util/Task.java
    com/microsoft/cognitiveservices/speech/util/TaskRunner.java
)

set(SWIG_JAVA_SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/AudioInputStream.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/AudioInputStreamFormat.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/BaseAsyncRecognizer.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/BaseRecognizerBase.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/BaseValueCollection.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/carbon_java.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/carbon_javaJNI.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/DefaultRecognizerFactory.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/EventArgs.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/FactoryParameter.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/IDefaultRecognizerFactory.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/IntentEventListener.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/IntentEventSignal.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/IntentRecognitionEventArgs.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/IntentRecognitionResult.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/IntentRecognitionResultPtrFuture.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/IntentRecognizer.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/IntentRecognizerBase.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/IntentTrigger.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/IRecognizerFactory.java

    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/LuisModel.java

    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/Reason.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/RecognitionEventArgs.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/RecognitionEventListener.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/RecognitionEventSignal.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/RecognitionResult.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/Recognizer.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/RecognizerFactory.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/RecognizerFactoryParameterCollection.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/RecognizerFactoryParameterValue.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/RecognizerParameter.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/RecognizerParameterValue.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/RecognizerParameterValueCollection.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/ResultProperty.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/ResultPropertyValue.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/ResultPropertyValueCollection.java

    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/Session.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/SessionEventArgs.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/SessionEventListener.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/SessionEventSignal.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/SessionParameter.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/SessionParameterValue.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/SessionParameterValueCollection.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/SpeechRecognitionEventArgs.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/SpeechRecognitionEventListener.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/SpeechRecognitionEventSignal.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/SpeechRecognitionResult.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/SpeechRecognitionResultPtrFuture.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/SpeechRecognizer.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/SpeechRecognizerBase.java

    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/TranslationLanguageResource.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/TranslationRecognizer.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/TranslationRecognizerBase.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/TranslationStatus.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/TranslationSynthesisEventListener.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/TranslationSynthesisEventSignal.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/TranslationSynthesisResult.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/TranslationSynthesisResultEventArgs.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/TranslationTexEventListener.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/TranslationTextEventSignal.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/TranslationTextResult.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/TranslationTextResultPtrFuture.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/TranslationTextResultEventArgs.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/SynthesisScopeResourceValue.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/TextScopeResourceValue.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/SpeechScopeResourceValue.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/WstringVector.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/SWIGTYPE_p_std__unordered_mapT_std__wstring_std__wstring_t.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/SWIGTYPE_p_std__unordered_mapT_std__wstring_Microsoft__CognitiveServices__Speech__Recognition__Translation__SpeechScopeResourceValue_t.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/SWIGTYPE_p_std__unordered_mapT_std__wstring_Microsoft__CognitiveServices__Speech__Recognition__Translation__SynthesisScopeResourceValue_t.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/SWIGTYPE_p_std__unordered_mapT_std__wstring_Microsoft__CognitiveServices__Speech__Recognition__Translation__TextScopeResourceValue_t.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/SWIGTYPE_p_std__vectorT_unsigned_char_t.java

    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/carbon_javaConstants.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/SWIGTYPE_p_SPXEVENTHANDLE.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/SWIGTYPE_p_SPXLUISHANDLE.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/SWIGTYPE_p_SPXRECOFACTORYHANDLE.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/SWIGTYPE_p_SPXRECOHANDLE.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/SWIGTYPE_p_SPXRESULTHANDLE.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/SWIGTYPE_p_SPXSESSIONHANDLE.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/SWIGTYPE_p_SPXTRIGGERHANDLE.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/IDefaultRecognizerFactory.java

    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/Value.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/ValueCollectionRecognizerParameter.java
#    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/ValueCollectionRecognizerProperty.java
#    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/ValueCollectionRecognizerFactoryParameter.java
#    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/ValueCollectionSessionParameter.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/VoidFuture.java
)


SET_SOURCE_FILES_PROPERTIES(${SWIG_JAVA_SOURCES} PROPERTIES GENERATED TRUE)


enable_language(Java)
add_jar(${JAVA_LIB} SOURCES ${SWIG_JAVA_SOURCES} ${JAVA_SOURCES})
add_dependencies(${JAVA_LIB} ${JAVA_BINDINGS} ${SWIG_MODULE_${JAVA_BINDINGS}_REAL_NAME})

set_target_properties(${SWIG_MODULE_${JAVA_LIB}_REAL_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set_target_properties(${JAVA_LIB} PROPERTIES FOLDER bindings/java)

set_property(DIRECTORY ${CMAKE_SOURCE_DIR} APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${SWIG_INTERFACE})
