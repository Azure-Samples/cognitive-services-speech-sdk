project(${JAVA_JNI_LIBRARY})

set(JAVA_AWT_LIBRARY NotNeeded)
set(JAVA_JVM_LIBRARY NotNeeded)
set(JAVA_INCLUDE_PATH2 NotNeeded)
find_package(JNI REQUIRED)

if (JNI_FOUND)
    message (STATUS "JNI_INCLUDE_DIRS=${JNI_INCLUDE_DIRS}")
    message (STATUS "JNI_LIBRARIES=${JNI_LIBRARIES}")
    message (STATUS "JAVA_INCLUDE_PATH=${JAVA_INCLUDE_PATH}")
endif()

set(SRC_DIR "${PROJECT_SOURCE_DIR}")

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-Wno-unused-function -fno-strict-aliasing")
elseif(ANDROID)
    # Since we use CLANG, we need to set __GNUC__=4 because otherwise the generated Java bindings won't export the symbols
    set(CMAKE_CXX_FLAGS "-D__GNUC__=4 -std=c++11 -Wall -Wextra -Wno-unused-function -Wno-unused-parameter -fno-strict-aliasing -fPIC")

    # compilation flags required by security checks
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_FORTIFY_SOURCE=2 -fstack-protector-strong")

    # relro makes the relocation section read only after loading preventing attacks on the table
    set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS_DEBUG} -z nocopyreloc -z relro -z now") # -Wl,-z,relro
    set(CMAKE_MODULE_LINKER_FLAGS_RELEASE "${CMAKE_MODULE_LINKER_FLAGS_RELEASE} -z nocopyreloc -z relro -z now") # -Wl,-z,relro
elseif(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
endif()

if(WIN32)
  set(SPEECHSDK_ORIGINAL_FILENAME "${JAVA_JNI_LIBRARY}.dll")
  configure_file(
    ${CARBON_ROOT}/ci/version.rc.in
    ${CMAKE_CURRENT_BINARY_DIR}/windows/version.rc)

  set(SOURCES_VERSION "${CMAKE_CURRENT_BINARY_DIR}/windows/version.rc")
else()
  set(SOURCES_VERSION "")
endif()

include_directories(${CARBON_C_API})
include_directories(${CARBON_CXX_API})
include_directories(${CARBON_JAVA_JNI})
include_directories(${JAVA_INCLUDE_PATH})
if(WIN32)
    include_directories("${JAVA_INCLUDE_PATH}/win32")
elseif(SPEECHSDK_OS STREQUAL "Linux")
    include_directories("${JAVA_INCLUDE_PATH}/linux")
elseif(SPEECHSDK_OS STREQUAL "OSX")
    include_directories("${JAVA_INCLUDE_PATH}/darwin")
endif()

set(HEADERS
    "${JAVA_INCLUDE_PATH}/jni.h"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_SpeechConfig.h"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_PropertyCollection.h"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_util_Contracts.h"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_CancellationDetails.h"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_RecognitionEventArgs.h"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_RecognitionResult.h"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_Recognizer.h"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_SessionEventArgs.h"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_SpeechRecognizer.h"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_util_SafeHandle.h"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_audio_AudioConfig.h"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_Connection.h"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_AutoDetectSourceLanguageConfig.h"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_SourceLanguageConfig.h"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_SpeechSynthesizer.h"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_SpeechSynthesisWordBoundaryEventArgs.h"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_SpeechSynthesisResult.h"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_SpeechSynthesisEventArgs.h"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_SpeechSynthesisCancellationDetails.h"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_PhraseListGrammar.h"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_NoMatchDetails.h"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_GrammarList.h"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_Grammar.h"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_ClassLanguageModel.h"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_AudioDataStream.h"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_KeywordRecognizer.h"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_PronunciationAssessmentConfig.h"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_audio_AudioStreamFormat.h"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_audio_PullAudioInputStream.h"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_audio_PushAudioInputStream.h"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_audio_PushAudioOutputStream.h"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_audio_PullAudioOutputStream.h"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_dialog_DialogServiceConnector.h"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_dialog_CustomCommandsConfig.h"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_dialog_BotFrameworkConfig.h"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_dialog_ActivityReceivedEventArgs.h"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_intent_IntentRecognitionResult.h"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_intent_IntentRecognizer.h"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_intent_IntentTrigger.h"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_intent_LanguageUnderstandingModel.h"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_KeywordRecognitionModel.h"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_translation_SpeechTranslationConfig.h"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_translation_TranslationSynthesisResult.h"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_translation_TranslationSynthesisEventArgs.h"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_translation_TranslationRecognizer.h"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_translation_TranslationRecognitionResult.h"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_transcription_Conversation.h"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_transcription_ConversationExpirationEventArgs.h"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_transcription_ConversationParticipantsChangedEventArgs.h"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_transcription_ConversationTranscriber.h"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_transcription_ConversationTranscriptionResult.h"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_transcription_ConversationTranslationResult.h"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_transcription_ConversationTranslator.h"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_transcription_User.h"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_transcription_Participant.h"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_diagnostics.h"
    "${SRC_DIR}/jni_utils.h"
    )

set(SOURCES
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_SpeechConfig.cpp"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_PropertyCollection.cpp"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_util_Contracts.cpp"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_CancellationDetails.cpp"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_RecognitionEventArgs.cpp"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_RecognitionResult.cpp"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_Recognizer.cpp"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_SessionEventArgs.cpp"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_SpeechRecognizer.cpp"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_util_SafeHandle.cpp"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_audio_AudioConfig.cpp"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_Connection.cpp"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_AutoDetectSourceLanguageConfig.cpp"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_SourceLanguageConfig.cpp"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_SpeechSynthesizer.cpp"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_SpeechSynthesisWordBoundaryEventArgs.cpp"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_SpeechSynthesisResult.cpp"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_SpeechSynthesisEventArgs.cpp"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_SpeechSynthesisCancellationDetails.cpp"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_PhraseListGrammar.cpp"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_NoMatchDetails.cpp"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_GrammarList.cpp"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_Grammar.cpp"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_ClassLanguageModel.cpp"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_AudioDataStream.cpp"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_KeywordRecognizer.cpp"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_PronunciationAssessmentConfig.cpp"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_audio_AudioStreamFormat.cpp"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_audio_PullAudioInputStream.cpp"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_audio_PushAudioInputStream.cpp"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_audio_PushAudioOutputStream.cpp"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_audio_PullAudioOutputStream.cpp"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_dialog_DialogServiceConnector.cpp"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_dialog_CustomCommandsConfig.cpp"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_dialog_BotFrameworkConfig.cpp"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_dialog_ActivityReceivedEventArgs.cpp"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_intent_IntentRecognitionResult.cpp"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_intent_IntentRecognizer.cpp"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_intent_IntentTrigger.cpp"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_intent_LanguageUnderstandingModel.cpp"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_KeywordRecognitionModel.cpp"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_translation_SpeechTranslationConfig.cpp"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_translation_TranslationSynthesisResult.cpp"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_translation_TranslationSynthesisEventArgs.cpp"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_translation_TranslationRecognizer.cpp"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_translation_TranslationRecognitionResult.cpp"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_transcription_Conversation.cpp"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_transcription_ConversationExpirationEventArgs.cpp"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_transcription_ConversationParticipantsChangedEventArgs.cpp"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_transcription_ConversationTranscriber.cpp"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_transcription_ConversationTranscriptionResult.cpp"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_transcription_ConversationTranslationResult.cpp"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_transcription_ConversationTranslator.cpp"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_transcription_User.cpp"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_transcription_Participant.cpp"
    "${SRC_DIR}/com_microsoft_cognitiveservices_speech_diagnostics.cpp"
    "${SRC_DIR}/jni_utils.cpp"
    "${SRC_DIR}/dll.cpp"
    )

set_source_groups()

add_library(${JAVA_JNI_LIBRARY} SHARED ${HEADERS} ${SOURCES})

if(APPLE)
    set_target_properties(${JAVA_JNI_LIBRARY}
      PROPERTIES BUILD_RPATH "@loader_path")
elseif(UNIX)
    set_target_properties(${JAVA_JNI_LIBRARY} PROPERTIES BUILD_RPATH "$ORIGIN")
endif()

if(WIN32 AND SPECTRE_MITIGATION)
    set_property(TARGET ${JAVA_JNI_LIBRARY} PROPERTY VS_GLOBAL_SpectreMitigation "Spectre")
endif()

target_link_libraries(${JAVA_JNI_LIBRARY} ${SPEECHSDK_CORE_LIBRARY})

# Adding logcat logging
if(ANDROID)
    target_link_libraries(${JAVA_JNI_LIBRARY} log)
endif()

if(${IS_CROSSCOMPILING})
    target_link_libraries(${JAVA_JNI_LIBRARY} pthread)
else()
    target_link_libraries(${JAVA_JNI_LIBRARY})
endif()

set_target_properties(${JAVA_JNI_LIBRARY} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set_target_properties(${JAVA_JNI_LIBRARY} PROPERTIES
    FOLDER bindings/java_jni
)
