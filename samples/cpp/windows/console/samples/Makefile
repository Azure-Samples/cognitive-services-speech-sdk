#
# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
#
# Microsoft Cognitive Services Speech SDK - Samples for C++
#
# Check out https://aka.ms/csspeech for documentation.
#
# To compile on Linux using this Makefile, follow instructions similar to the ones
# documented for the C++ Linux Quickstart at:
# https://github.com/Azure-Samples/cognitive-services-speech-sdk/blob/master/quickstart/cpp/linux/from-microphone/README.md
#
# You will also need to install the NLOHMANN JSON package ("JSON for Modern C++") as one of the samples
# needs to parse a JSON string containing detailed recognition results. The header file /usr/include/nlohmann/json.hpp
# must be present to compile this sample.
#

# Update this path as needed
SPEECHSDK_ROOT:=/change/to/point/to/extracted/SpeechSDK

# If you'd like to build for
# - Linux x86 (32-bit), replace "x64" below with "x86".
# - Linux ARM64 (64-bit), replace "x64" below with "arm64".
# - Linux ARM32 (32-bit), replace "x64" below with "arm32".
TARGET_PLATFORM:=x64

CHECK_FOR_SPEECHSDK := $(shell test -f $(SPEECHSDK_ROOT)/lib/$(TARGET_PLATFORM)/libMicrosoft.CognitiveServices.Speech.core.so && echo Success)
ifneq ("$(CHECK_FOR_SPEECHSDK)","Success")
  $(error Please set SPEECHSDK_ROOT to point to your extracted Speech SDK, $$SPEECHSDK_ROOT/lib/$$TARGET_PLATFORM/libMicrosoft.CognitiveServices.Speech.core.so should exist.)
endif

USR_INC_ROOT:=/usr/include
CHECK_FOR_NLOHMANN_JSON := $(shell test -f $(USR_INC_ROOT)/nlohmann/json.hpp && echo Success)
ifneq ("$(CHECK_FOR_NLOHMANN_JSON)","Success")
  $(error Please install nlohmann.json package. $$USR_INC_ROOT/nlohmann/json.hpp should exist.)
endif

LIBPATH:=$(SPEECHSDK_ROOT)/lib/$(TARGET_PLATFORM)

INCPATH:=$(SPEECHSDK_ROOT)/include/cxx_api $(SPEECHSDK_ROOT)/include/c_api

LIBS:=-lMicrosoft.CognitiveServices.Speech.core -lpthread -l:libasound.so.2

all: sample

sample: main.cpp speech_recognition_samples.cpp speech_synthesis_samples.cpp translation_samples.cpp intent_recognition_samples.cpp conversation_transcriber_samples.cpp meeting_transcriber_samples.cpp speaker_recognition_samples.cpp standalone_language_detection_samples.cpp diagnostics_logging_samples.cpp
	g++ $^ -o $@ \
	    --std=c++14 \
	    $(patsubst %,-I%, $(INCPATH)) \
	    $(patsubst %,-L%, $(LIBPATH)) \
	    $(LIBS)

# Note: to run, LD_LIBRARY_PATH should point to $LIBPATH. For example:
# export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$SPEECHSDK_ROOT/lib/x64"