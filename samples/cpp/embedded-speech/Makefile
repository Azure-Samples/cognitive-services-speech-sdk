#
# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
#
# Microsoft Cognitive Services Speech SDK - Embedded speech C++ samples (Linux, macOS)
#
# To install required Speech SDK headers and libraries, run (bash or compatible shell):
#   nuget restore samples.sln
#   for nupkg in packages/Microsoft.*/*.nupkg ; do unzip -o $nupkg -d SpeechSDK ; done
#
# Update SPEECHSDK_ROOT below, and check and adjust TARGET_PLATFORM if necessary
# (see $SPEECHSDK_ROOT/runtimes).
# Note that RHEL/CentOS 7 is not supported with embedded speech.
#
SPEECHSDK_ROOT:=/change/to/point/to/extracted/SpeechSDK
TARGET_PLATFORM:=linux-x64
TARGET_BINARY:=samples-$(TARGET_PLATFORM)

UNAME_S:=$(shell uname -s)

ifeq ($(UNAME_S),Linux)
  DYNLIB_SUFFIX=.so
else
  DYNLIB_SUFFIX=.dylib
endif

CHECK_FOR_SPEECHSDK := $(shell test -f $(SPEECHSDK_ROOT)/runtimes/$(TARGET_PLATFORM)/native/libMicrosoft.CognitiveServices.Speech.core$(DYNLIB_SUFFIX) && echo Success)
ifneq ("$(CHECK_FOR_SPEECHSDK)","Success")
  $(error Please set SPEECHSDK_ROOT to point to your extracted Speech SDK, $$SPEECHSDK_ROOT/runtimes/$$TARGET_PLATFORM/native/libMicrosoft.CognitiveServices.Speech.core$$DYNLIB_SUFFIX should exist.)
endif

INCPATH:=$(SPEECHSDK_ROOT)/build/native/include/cxx_api $(SPEECHSDK_ROOT)/build/native/include/c_api packages/nlohmann.json.3.11.2/build/native/include
LIBPATH:=$(SPEECHSDK_ROOT)/runtimes/$(TARGET_PLATFORM)/native

LIBS:=-lMicrosoft.CognitiveServices.Speech.core -lpthread
ifeq ($(UNAME_S),Linux)
  LIBS:=${LIBS} -l:libasound.so.2
endif

all: $(TARGET_BINARY)

$(TARGET_BINARY): samples/main.cpp samples/settings.cpp samples/intent_recognition_samples.cpp samples/speech_recognition_samples.cpp samples/speech_synthesis_samples.cpp samples/speech_translation_samples.cpp
	g++ $^ -o $@ \
	    --std=c++14 \
	    $(patsubst %,-I%, $(INCPATH)) \
	    $(patsubst %,-L%, $(LIBPATH)) \
	    $(LIBS)
	$(info Note: Before running the samples, set LD_LIBRARY_PATH (Linux) / DYLD_LIBRARY_PATH (macOS) to point to $(LIBPATH))
