project(com.microsoft.cognitiveservices.speech)

find_package(Java REQUIRED)

include(UseJava)
add_subdirectory(java_jni)

#
# Set up target to build the JAR, ${JAVA_LIB}
#

set(JAVA_LIB ${PROJECT_NAME})

string(REPLACE . / PACKAGE_DIR ${PROJECT_NAME})

set(JAVA_SOURCES
    ${PACKAGE_DIR}/CancellationDetails.java
    ${PACKAGE_DIR}/KeywordRecognitionModel.java
    ${PACKAGE_DIR}/PropertyCollection.java
    ${PACKAGE_DIR}/RecognitionEventArgs.java
    ${PACKAGE_DIR}/RecognitionResult.java
    ${PACKAGE_DIR}/ResultReason.java
    ${PACKAGE_DIR}/CancellationReason.java
    ${PACKAGE_DIR}/CancellationErrorCode.java
    ${PACKAGE_DIR}/NoMatchDetails.java
    ${PACKAGE_DIR}/NoMatchReason.java
    ${PACKAGE_DIR}/Connection.java
    ${PACKAGE_DIR}/Grammar.java
    ${PACKAGE_DIR}/GrammarList.java
    ${PACKAGE_DIR}/ClassLanguageModel.java
    ${PACKAGE_DIR}/PhraseListGrammar.java
    ${PACKAGE_DIR}/Recognizer.java
    ${PACKAGE_DIR}/SessionEventArgs.java
    ${PACKAGE_DIR}/ConnectionEventArgs.java
    ${PACKAGE_DIR}/ServicePropertyChannel.java
    ${PACKAGE_DIR}/SpeechConfig.java
    ${PACKAGE_DIR}/SpeechRecognitionResult.java
    ${PACKAGE_DIR}/SpeechRecognitionEventArgs.java
    ${PACKAGE_DIR}/SpeechRecognitionCanceledEventArgs.java
    ${PACKAGE_DIR}/SpeechRecognizer.java
    ${PACKAGE_DIR}/ProfanityOption.java
    ${PACKAGE_DIR}/AutoDetectSourceLanguageConfig.java
    ${PACKAGE_DIR}/AutoDetectSourceLanguageResult.java
    ${PACKAGE_DIR}/SourceLanguageConfig.java
    ${PACKAGE_DIR}/KeywordRecognizer.java
    ${PACKAGE_DIR}/KeywordRecognitionResult.java
    ${PACKAGE_DIR}/KeywordRecognitionEventArgs.java
    ${PACKAGE_DIR}/Diagnostics.java

    ${PACKAGE_DIR}/transcription/Conversation.java
    ${PACKAGE_DIR}/transcription/ConversationTranscriptionResult.java
    ${PACKAGE_DIR}/transcription/ConversationTranscriptionEventArgs.java
    ${PACKAGE_DIR}/transcription/ConversationTranscriptionCanceledEventArgs.java
    ${PACKAGE_DIR}/transcription/ConversationTranscriber.java
    ${PACKAGE_DIR}/transcription/Participant.java
    ${PACKAGE_DIR}/transcription/User.java

    ${PACKAGE_DIR}/transcription/ConversationExpirationEventArgs.java
    ${PACKAGE_DIR}/transcription/ConversationParticipantsChangedEventArgs.java
    ${PACKAGE_DIR}/transcription/ConversationTranslationCanceledEventArgs.java
    ${PACKAGE_DIR}/transcription/ConversationTranslationEventArgs.java
    ${PACKAGE_DIR}/transcription/ConversationTranslationResult.java
    ${PACKAGE_DIR}/transcription/ConversationTranslator.java
    ${PACKAGE_DIR}/transcription/ParticipantChangedReason.java

    ${PACKAGE_DIR}/AudioDataStream.java
    ${PACKAGE_DIR}/SpeechSynthesisEventArgs.java
    ${PACKAGE_DIR}/SpeechSynthesisWordBoundaryEventArgs.java
    ${PACKAGE_DIR}/SpeechSynthesisResult.java
    ${PACKAGE_DIR}/SpeechSynthesizer.java
    ${PACKAGE_DIR}/StreamStatus.java
    ${PACKAGE_DIR}/SpeechSynthesisCancellationDetails.java
    ${PACKAGE_DIR}/SpeechSynthesisOutputFormat.java

    ${PACKAGE_DIR}/OutputFormat.java
    ${PACKAGE_DIR}/audio/AudioStreamContainerFormat.java

    ${PACKAGE_DIR}/audio/AudioConfig.java
    ${PACKAGE_DIR}/audio/AudioInputStream.java
    ${PACKAGE_DIR}/audio/AudioOutputStream.java
    ${PACKAGE_DIR}/audio/AudioStreamFormat.java
    ${PACKAGE_DIR}/audio/PullAudioInputStream.java
    ${PACKAGE_DIR}/audio/PushAudioInputStream.java
    ${PACKAGE_DIR}/audio/PullAudioInputStreamCallback.java
    ${PACKAGE_DIR}/audio/PullAudioOutputStream.java
    ${PACKAGE_DIR}/audio/PushAudioOutputStream.java
    ${PACKAGE_DIR}/audio/PushAudioOutputStreamCallback.java

    ${PACKAGE_DIR}/dialog/ActivityReceivedEventArgs.java
    ${PACKAGE_DIR}/dialog/DialogServiceConfig.java
    ${PACKAGE_DIR}/dialog/BotFrameworkConfig.java
    ${PACKAGE_DIR}/dialog/CustomCommandsConfig.java
    ${PACKAGE_DIR}/dialog/DialogServiceConnector.java

    ${PACKAGE_DIR}/intent/IntentRecognitionResult.java
    ${PACKAGE_DIR}/intent/IntentRecognitionEventArgs.java
    ${PACKAGE_DIR}/intent/IntentRecognitionCanceledEventArgs.java
    ${PACKAGE_DIR}/intent/IntentRecognizer.java
    ${PACKAGE_DIR}/intent/LanguageUnderstandingModel.java

    ${PACKAGE_DIR}/translation/TranslationRecognizer.java
    ${PACKAGE_DIR}/translation/TranslationSynthesisResult.java
    ${PACKAGE_DIR}/translation/TranslationSynthesisEventArgs.java
    ${PACKAGE_DIR}/translation/TranslationRecognitionResult.java
    ${PACKAGE_DIR}/translation/TranslationRecognitionEventArgs.java
    ${PACKAGE_DIR}/translation/TranslationRecognitionCanceledEventArgs.java
    ${PACKAGE_DIR}/translation/SpeechTranslationConfig.java
    ${PACKAGE_DIR}/PropertyId.java

    ${PACKAGE_DIR}/PronunciationAssessmentGradingSystem.java
    ${PACKAGE_DIR}/PronunciationAssessmentGranularity.java
    ${PACKAGE_DIR}/PronunciationAssessmentConfig.java
    ${PACKAGE_DIR}/PronunciationAssessmentResult.java

    ${PACKAGE_DIR}/util/EventHandler.java
    ${PACKAGE_DIR}/util/EventHandlerImpl.java
    ${PACKAGE_DIR}/util/Contracts.java
    ${PACKAGE_DIR}/util/CloseGuard.java
    ${PACKAGE_DIR}/util/IntRef.java
    ${PACKAGE_DIR}/util/SafeHandle.java
    ${PACKAGE_DIR}/util/SafeHandleType.java
    ${PACKAGE_DIR}/util/StringRef.java
    ${PACKAGE_DIR}/util/StringMapRef.java
)
if(NOT ANDROID)
  list(APPEND JAVA_SOURCES ${PACKAGE_DIR}/NativeLibraryLoader.java)
endif()

enable_language(Java)
set(CMAKE_JAVA_COMPILE_FLAGS -source 1.7 -target 1.7)
add_jar(${JAVA_LIB}
        SOURCES ${JAVA_SOURCES}
        OUTPUT_DIR ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

set_target_properties(${JAVA_LIB} PROPERTIES FOLDER bindings/java)

# Note: this could also run earlier than post-build
add_custom_command(TARGET ${JAVA_LIB}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E tar cvf "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${JAVA_LIB}-src.zip" --format=zip com
                   WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
                   COMMENT "Generating src package including javadoc information")
