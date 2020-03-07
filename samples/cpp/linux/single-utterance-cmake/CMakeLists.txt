cmake_minimum_required(VERSION 3.15)

# Project's name
project(speechtest   VERSION 1.0
                      DESCRIPTION "Testing Cognitive Services"
                      LANGUAGES CXX)

#set(SPEECHSDK_ROOT /home/chief/speechsdk)
set(SPEECHSDK_ROOT $ENV{SPEECHSDK_ROOT})
link_directories(${SPEECHSDK_ROOT}/lib/x64 libasound.so.2)

add_executable(hello ${PROJECT_SOURCE_DIR}/single-utterance.cpp)

target_compile_features(hello PUBLIC cxx_std_14)

target_link_libraries(hello pthread Microsoft.CognitiveServices.Speech.core)

target_include_directories(hello PUBLIC ${SPEECHSDK_ROOT}/include/cxx_api ${SPEECHSDK_ROOT}/include/c_api)

