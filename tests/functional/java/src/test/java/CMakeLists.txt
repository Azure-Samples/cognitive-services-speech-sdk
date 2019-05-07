project(com.microsoft.cognitiveservices.speech.tests)

find_package(Java REQUIRED)

include(UseJava)
enable_language(Java)

set(JAVA_SOURCES_TESTS
    tests/AllTests.java
    tests/Settings.java

    tests/runner/AntXmlRunListener.java
    tests/runner/DateUtils.java
    tests/runner/DomElementWriter.java
    tests/runner/Runner.java

    tests/endtoend/AllEnd2EndTests.java
    tests/endtoend/RecognizerTests.java
    tests/endtoend/SampleRecognizeIntent.java
    tests/endtoend/SampleRecognizeIntentTest.java
    tests/endtoend/SampleRecognizeWithIntermediateResults.java
    tests/endtoend/SampleRecognizeWithIntermediateResultsTest.java
    tests/endtoend/SampleSimpleRecognize.java
    tests/endtoend/SampleSimpleRecognizeTest.java
    tests/endtoend/VirtualAssistantTests.java
    tests/endtoend/WavFileAudioInputStreamTest.java

    tests/unit/AllUnitTests.java
    tests/unit/AudioInputStreamTests.java
    tests/unit/ConversationTranscriberTests.java
    tests/unit/IntentRecognizerTests.java
    tests/unit/KeywordRecognitionModelTests.java
    tests/unit/LanguageUnderstandingModelTests.java
    tests/unit/PhraseListGrammarTests.java
    tests/unit/PropertyCollectionTests.java
    tests/unit/RecognizerTests.java
    tests/unit/SpeechConfigTests.java
    tests/unit/SpeechPropertyIdTests.java
    tests/unit/SpeechRecognizerTests.java
    tests/unit/TranslationRecognizerTests.java
    tests/unit/WavFileAudioInputStream.java
)

add_jar(${PROJECT_NAME}
    SOURCES ${JAVA_SOURCES_TESTS}
    INCLUDE_JARS ${PROJECT_NAME} com.microsoft.cognitiveservices.speech ${CARBON_EXTERNAL}/junit/junit-4.12.jar ${CARBON_EXTERNAL}/junit/hamcrest-core-1.3.jar
    OUTPUT_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

set_target_properties(${PROJECT_NAME} PROPERTIES
    FOLDER tests
    )

add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E tar cvf "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}-src.zip" --format=zip tests
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    COMMENT "Generating src package including javadoc information")
