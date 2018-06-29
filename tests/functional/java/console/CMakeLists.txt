project(com.microsoft.cognitiveservices.speech.samples)
# TODO change package

find_package(Java REQUIRED)

include(UseJava)
enable_language(Java)

set(JAVA_SOURCES
    ${PROJECT_SOURCE_DIR}/Main.java
    ${PROJECT_SOURCE_DIR}/com/microsoft/cognitiveservices/speech/samples/SampleRecognizeIntent.java
    ${PROJECT_SOURCE_DIR}/com/microsoft/cognitiveservices/speech/samples/SampleRecognizeIntentWithWakeWord.java
    ${PROJECT_SOURCE_DIR}/com/microsoft/cognitiveservices/speech/samples/SampleRecognizeWithIntermediateResults.java
    ${PROJECT_SOURCE_DIR}/com/microsoft/cognitiveservices/speech/samples/SampleRecognizeWithWakeWord.java
    ${PROJECT_SOURCE_DIR}/com/microsoft/cognitiveservices/speech/samples/SampleSettings.java
    ${PROJECT_SOURCE_DIR}/com/microsoft/cognitiveservices/speech/samples/SampleSimpleRecognize.java
    ${PROJECT_SOURCE_DIR}/com/microsoft/cognitiveservices/speech/samples/Stoppable.java
    ${PROJECT_SOURCE_DIR}/com/microsoft/cognitiveservices/speech/samples/WaveFileAudioInputStream.java
)

add_jar(${PROJECT_NAME}
    SOURCES ${JAVA_SOURCES}
    INCLUDE_JARS com.microsoft.cognitiveservices.speech
    OUTPUT_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND  ${CMAKE_COMMAND} -E tar cvf "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}-src.zip" --format=zip Main.java com
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    COMMENT "Generating src package including javadoc information")
