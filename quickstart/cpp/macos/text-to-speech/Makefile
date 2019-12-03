#
# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
#
# Microsoft Cognitive Services Speech SDK - Quickstart for macOS and C++
#
# Check out https://aka.ms/csspeech for documentation.
#

SPEECHSDK_ROOT:=/change/to/point/to/extracted/SpeechSDK

CHECK_FOR_SPEECHSDK := $(shell test -f $(SPEECHSDK_ROOT)/MicrosoftCognitiveServicesSpeech.framework/MicrosoftCognitiveServicesSpeech && echo Success)
ifneq ("$(CHECK_FOR_SPEECHSDK)","Success")
  $(error Please set SPEECHSDK_ROOT to point to your extracted Speech SDK, $$SPEECHSDK_ROOT/MicrosoftCognitiveServicesSpeech.framework/MicrosoftCognitiveServicesSpeech should exist.)
endif

LIBS:=-framework MicrosoftCognitiveServicesSpeech
all: helloworld

# Note: to run, DYLD_FRAMEWORK_PATH should point to $SPEECHSDK_ROOT.
helloworld: helloworld.cpp
	g++ $< -o $@ \
	    --std=c++14 \
	    $(patsubst %,-F%, $(SPEECHSDK_ROOT)) \
	    $(LIBS)

