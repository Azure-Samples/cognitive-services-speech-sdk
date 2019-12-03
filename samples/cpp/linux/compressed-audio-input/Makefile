#
# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
#
# Microsoft Cognitive Services Speech SDK - Recognize speech from an MP3/Opus file
#
# Check out https://aka.ms/csspeech for documentation.
#

SPEECHSDK_ROOT:=/change/to/point/to/extracted/SpeechSDK

# If you'd like to build for
# - Linux x86 (32-bit), replace "x64" below with "x86".
# - Linux ARM64 (64-bit), replace "x64" below with "arm64".
TARGET_PLATFORM:=x64

CHECK_FOR_SPEECHSDK := $(shell test -f $(SPEECHSDK_ROOT)/lib/$(TARGET_PLATFORM)/libMicrosoft.CognitiveServices.Speech.core.so && echo Success)
ifneq ("$(CHECK_FOR_SPEECHSDK)","Success")
  $(error Please set SPEECHSDK_ROOT to point to your extracted Speech SDK, $$SPEECHSDK_ROOT/lib/$(TARGET_PLATFORM)/libMicrosoft.CognitiveServices.Speech.core.so should exist.)
endif

LIBPATH:=$(SPEECHSDK_ROOT)/lib/$(TARGET_PLATFORM)

INCPATH:=$(SPEECHSDK_ROOT)/include/cxx_api $(SPEECHSDK_ROOT)/include/c_api

LIBS:=-lMicrosoft.CognitiveServices.Speech.core -lpthread -l:libasound.so.2

all: compressed-audio-input

# Note: to run, LD_LIBRARY_PATH should point to $LIBPATH.
compressed-audio-input: compressed-audio-input.cpp
	g++ $< -o $@ \
	    --std=c++14 \
	    $(patsubst %,-I%, $(INCPATH)) \
	    $(patsubst %,-L%, $(LIBPATH)) \
	    $(LIBS)
