project(com.microsoft.cognitiveservices.speech.tests)

find_package(Java REQUIRED)

include(UseJava)
enable_language(Java)


set(JAVA_SOURCES_TESTS
    ${PROJECT_SOURCE_DIR}/tests/AllTests.java
    ${PROJECT_SOURCE_DIR}/tests/Settings.java

    ${PROJECT_SOURCE_DIR}/tests/endtoend/AllEnd2EndTests.java
    ${PROJECT_SOURCE_DIR}/tests/endtoend/SampleRecognizeIntentTest.java
    ${PROJECT_SOURCE_DIR}/tests/endtoend/MicrophoneAudioInputStreamTest.java
    ${PROJECT_SOURCE_DIR}/tests/endtoend/SampleRecognizeWithIntermediateResultsTest.java
    ${PROJECT_SOURCE_DIR}/tests/endtoend/SampleSimpleRecognizeTest.java
    ${PROJECT_SOURCE_DIR}/tests/endtoend/WaveFileAudioInputStreamTest.java
    ${PROJECT_SOURCE_DIR}/tests/endtoend/SampleRecognizeWithIntermediateResults.java
    ${PROJECT_SOURCE_DIR}/tests/endtoend/SampleSimpleRecognize.java
    ${PROJECT_SOURCE_DIR}/tests/endtoend/SampleRecognizeIntent.java

    ${PROJECT_SOURCE_DIR}/tests/unit/AllUnitTests.java
    ${PROJECT_SOURCE_DIR}/tests/unit/IntentRecognizerTests.java
    ${PROJECT_SOURCE_DIR}/tests/unit/KeywordRecognitionModelTests.java
    ${PROJECT_SOURCE_DIR}/tests/unit/LanguageUnderstandingModelTests.java
    ${PROJECT_SOURCE_DIR}/tests/unit/SpeechFactoryTests.java
    ${PROJECT_SOURCE_DIR}/tests/unit/SpeechRecognizerTests.java
    ${PROJECT_SOURCE_DIR}/tests/unit/TranslationRecognizerTests.java
    ${PROJECT_SOURCE_DIR}/tests/unit/RecognizerTests.java
    ${PROJECT_SOURCE_DIR}/tests/unit/RecognizerParameterNamesTests.java
    ${PROJECT_SOURCE_DIR}/tests/unit/AudioInputStreamTests.java
    ${PROJECT_SOURCE_DIR}/tests/unit/ParameterCollectionTests.java
    ${PROJECT_SOURCE_DIR}/tests/unit/WaveFileAudioInputStream.java
)

add_jar(${PROJECT_NAME}
    SOURCES ${JAVA_SOURCES_TESTS}
    INCLUDE_JARS ${PROJECT_NAME} com.microsoft.cognitiveservices.speech ${CARBON_EXTERNAL}/junit/junit-4.12.jar ${CARBON_EXTERNAL}/junit/hamcrest-core-1.3.jar
    OUTPUT_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND  ${CMAKE_COMMAND} -E tar cvf "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}-src.zip" --format=zip tests
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    COMMENT "Generating src package including javadoc information")
