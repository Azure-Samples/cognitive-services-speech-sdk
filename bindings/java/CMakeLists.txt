project(com.microsoft.cognitiveservices.speech)

set(JAVA_BINDINGS Microsoft.CognitiveServices.Speech.java.bindings)

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

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
     set(CMAKE_CXX_FLAGS "-Wno-unused-function -fno-strict-aliasing")
elseif(ANDROID)
# Since we use CLANG, we need to set __GNUC__=4 because otherwise the generated swig bindings won't export the symbols
     set(CMAKE_CXX_FLAGS "-D__GNUC__=4 -std=c++11 -Wall -Wextra -Wno-unused-function -Wno-unused-parameter -fno-strict-aliasing -fPIC")
endif()

set(SWIG_MODULE_${JAVA_BINDINGS}_EXTRA_DEPS
    ${SWIG_INTERFACE}
    ${SOURCES}
    ${SPEECH_C_API_HEADERS}
    ${SPEECH_CXX_API_HEADERS})

# ${SWIG_INTERFACE} should resolve to {repo/root}/bindings/carbon.i
# warning 302: identifier redifined by ...
set(CMAKE_SWIG_FLAGS -w401,516,503,302 -package "com.microsoft.cognitiveservices.speech.internal")
set_source_files_properties("${SRC_DIR}/carbon_java.i" PROPERTIES CPLUSPLUS ON)

# Make sure the nested directory structure exists
set(JAVA_SOURCE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal CACHE INTERNAL "")
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

if("${CMAKE_VERSION}" VERSION_GREATER 3.8)
    swig_add_library(${JAVA_BINDINGS} LANGUAGE java SOURCES ${SOURCES})
else()
    swig_add_module(${JAVA_BINDINGS} java ${SOURCES})
endif()
swig_link_libraries(${JAVA_BINDINGS} ${SPEECHSDK_CORE_LIBRARY})
set_target_properties(${SWIG_MODULE_${JAVA_BINDINGS}_REAL_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set_target_properties(${SWIG_MODULE_${JAVA_BINDINGS}_REAL_NAME} PROPERTIES FOLDER bindings/java)


# Build the Java code into ${JAVA_LIB}.jar
set(JAVA_LIB ${PROJECT_NAME})

# TODO this should be a wildcard, not an explicit list as these files are generated
# set(SWIG_JAVA_SOURCES ${JAVA_SOURCE_DIRECTORY}/*.java)
# SET_SOURCE_FILES_PROPERTIES(${SWIG_JAVA_SOURCES} PROPERTIES GENERATED TRUE)
#file(GLOB SWIG_JAVA_SOURCES "${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/*.java")
set(JAVA_SOURCES
    com/microsoft/cognitiveservices/speech/AudioInputStream.java
    com/microsoft/cognitiveservices/speech/AudioInputStreamFormat.java
    com/microsoft/cognitiveservices/speech/FactoryParameterNames.java
    com/microsoft/cognitiveservices/speech/ParameterCollection.java
    com/microsoft/cognitiveservices/speech/RecognitionErrorEventArgs.java
    com/microsoft/cognitiveservices/speech/RecognitionEventArgs.java
    com/microsoft/cognitiveservices/speech/RecognitionEventType.java
    com/microsoft/cognitiveservices/speech/RecognitionStatus.java
    com/microsoft/cognitiveservices/speech/Recognizer.java
    com/microsoft/cognitiveservices/speech/RecognizerParameterNames.java
    com/microsoft/cognitiveservices/speech/ResultParameterNames.java
    com/microsoft/cognitiveservices/speech/SessionEventArgs.java
    com/microsoft/cognitiveservices/speech/SessionEventType.java
    com/microsoft/cognitiveservices/speech/SpeechFactory.java
    com/microsoft/cognitiveservices/speech/SpeechRecognitionReason.java
    com/microsoft/cognitiveservices/speech/SpeechRecognitionResult.java
    com/microsoft/cognitiveservices/speech/SpeechRecognitionResultEventArgs.java
    com/microsoft/cognitiveservices/speech/SpeechRecognizer.java

    com/microsoft/cognitiveservices/speech/intent/IntentRecognitionResult.java
    com/microsoft/cognitiveservices/speech/intent/IntentRecognitionResultEventArgs.java
    com/microsoft/cognitiveservices/speech/intent/IntentRecognizer.java
    com/microsoft/cognitiveservices/speech/intent/LanguageUnderstandingModel.java

    com/microsoft/cognitiveservices/speech/translation/TranslationRecognizer.java
    com/microsoft/cognitiveservices/speech/translation/TranslationStatus.java
    com/microsoft/cognitiveservices/speech/translation/TranslationSynthesisResult.java
    com/microsoft/cognitiveservices/speech/translation/TranslationSynthesisResultEventArgs.java
    com/microsoft/cognitiveservices/speech/translation/TranslationTextResult.java
    com/microsoft/cognitiveservices/speech/translation/TranslationTextResultEventArgs.java

    com/microsoft/cognitiveservices/speech/util/EventHandler.java
    com/microsoft/cognitiveservices/speech/util/EventHandlerImpl.java
    com/microsoft/cognitiveservices/speech/util/Task.java
    com/microsoft/cognitiveservices/speech/util/TaskRunner.java
)

set(SWIG_JAVA_SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/AudioInputStream.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/AudioInputStreamFormat.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/BaseAsyncRecognizer.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/BaseRecognizerBase.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/BaseValueCollection.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/carbon_java.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/carbon_javaConstants.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/carbon_javaJNI.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/EventArgs.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/FactoryParameter.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/FactoryParameterCollection.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/ICognitiveServicesSpeechFactory.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/IntentEventListener.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/IntentEventSignal.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/IntentRecognitionEventArgs.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/IntentRecognitionResult.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/IntentRecognitionResultPtrFuture.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/IntentRecognizer.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/IntentRecognizerBase.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/IntentTrigger.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/ISpeechFactory.java

    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/LanguageUnderstandingModel.java

    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/Reason.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/RecognitionEventArgs.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/RecognitionEventListener.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/RecognitionEventSignal.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/RecognitionResult.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/Recognizer.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/FactoryParameterCollection.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/FactoryParameterCollectionBase.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/RecognizerParameter.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/RecognizerParameterValueCollection.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/RecognizerParameterValueCollectionBase.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/ResultProperty.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/ResultPropertyValueCollection.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/ResultPropertyValueCollectionBase.java

    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/Session.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/SessionEventArgs.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/SessionEventListener.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/SessionEventSignal.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/SessionParameter.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/SessionParameterValueCollection.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/SessionParameterValueCollectionBase.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/SpeechFactory.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/SpeechRecognitionEventArgs.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/SpeechRecognitionEventListener.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/SpeechRecognitionEventSignal.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/SpeechRecognitionResult.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/SpeechRecognitionResultPtrFuture.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/SpeechRecognizer.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/SpeechRecognizerBase.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/SpeechScopeResourceValue.java

    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/TextScopeResourceValue.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/TranslationLanguageResource.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/TranslationRecognizer.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/TranslationRecognizerBase.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/TranslationTextStatus.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/TranslationSynthesisStatus.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/TranslationSynthesisEventListener.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/TranslationSynthesisEventSignal.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/TranslationSynthesisResult.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/TranslationSynthesisResultEventArgs.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/TranslationTexEventListener.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/TranslationTextEventSignal.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/TranslationTextResult.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/TranslationTextResultEventArgs.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/TranslationTextResultPtrFuture.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/SynthesisScopeResourceValue.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/WstringVector.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/SWIGTYPE_p_std__mapT_std__wstring_Microsoft__CognitiveServices__Speech__Translation__SpeechScopeResourceValue_t.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/SWIGTYPE_p_std__mapT_std__wstring_Microsoft__CognitiveServices__Speech__Translation__SynthesisScopeResourceValue_t.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/SWIGTYPE_p_std__mapT_std__wstring_Microsoft__CognitiveServices__Speech__Translation__TextScopeResourceValue_t.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/SWIGTYPE_p_std__mapT_std__wstring_std__wstring_t.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/SWIGTYPE_p_std__vectorT_unsigned_char_t.java

    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/SWIGTYPE_p_SPXEVENTHANDLE.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/SWIGTYPE_p_SPXFACTORYHANDLE.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/SWIGTYPE_p_SPXLUMODELHANDLE.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/SWIGTYPE_p_SPXRECOHANDLE.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/SWIGTYPE_p_SPXRESULTHANDLE.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/SWIGTYPE_p_SPXSESSIONHANDLE.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/SWIGTYPE_p_SPXTRIGGERHANDLE.java

    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/Value.java
#    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/ValueCollectionRecognizerProperty.java
#    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/ValueCollectionRecognizerFactoryParameter.java
#    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/ValueCollectionSessionParameter.java
    ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/internal/VoidFuture.java
)


SET_SOURCE_FILES_PROPERTIES(${SWIG_JAVA_SOURCES} PROPERTIES GENERATED TRUE)


enable_language(Java)
add_jar(${JAVA_LIB} SOURCES ${SWIG_JAVA_SOURCES} ${JAVA_SOURCES} OUTPUT_DIR ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
add_dependencies(${JAVA_LIB} ${JAVA_BINDINGS} ${SWIG_MODULE_${JAVA_BINDINGS}_REAL_NAME})

set_target_properties(${SWIG_MODULE_${JAVA_LIB}_REAL_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set_target_properties(${JAVA_LIB} PROPERTIES FOLDER bindings/java)

set_property(DIRECTORY ${CMAKE_SOURCE_DIR} APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${SWIG_INTERFACE})

add_custom_command(TARGET ${JAVA_LIB}
                   POST_BUILD
                   COMMAND  ${CMAKE_COMMAND} -E tar cvf "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${JAVA_LIB}-src.zip" --format=zip com
                   WORKING_DIRECTORY "${SRC_DIR}"
                   COMMENT "Generating src package including javadoc information")

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/SessionEventArgs.java
                   COMMAND  ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/com ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com
                   COMMENT "Copying v2 api files to output directory")

add_custom_target(copy-over-api-files
                  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/swig-generated/com/microsoft/cognitiveservices/speech/SessionEventArgs.java)
add_dependencies(${JAVA_BINDINGS} copy-over-api-files)

set(JAVA_LIB_JAVADOC "${JAVA_LIB}-doc")
create_javadoc(copy-over-api-files
  PACKAGES com.microsoft.cognitiveservices.speech
           com.microsoft.cognitiveservices.speech.util
           com.microsoft.cognitiveservices.speech.translation
           com.microsoft.cognitiveservices.speech.intent
  SOURCEPATH "${CMAKE_CURRENT_BINARY_DIR}/swig-generated"
  WINDOWTITLE "Speech DDK"
  DOCTITLE "<h1>Speech DDK</h1>"
  INSTALLPATH "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${JAVA_LIB_JAVADOC}"
  AUTHOR TRUE
  USE TRUE
  VERSION TRUE)

