# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

# Set SPEECHSDK_ROOT to the directory containing the macOS .framework, or set
# SPEECHSDK_FRAMEWORK to its full path (use the macos-arm64_x86_64 XCFramework slice).
SPEECHSDK_ROOT ?=
SPEECHSDK_FRAMEWORK ?= $(SPEECHSDK_ROOT)/MicrosoftCognitiveServicesSpeech.framework
CC = clang

.PHONY: all check-sdk clean
all: live-interpreter

check-sdk:
	@test -d "$(SPEECHSDK_FRAMEWORK)" || { echo "Set SPEECHSDK_ROOT or SPEECHSDK_FRAMEWORK to the macOS Speech SDK framework." >&2; exit 1; }
	@test -f "$(SPEECHSDK_FRAMEWORK)/Headers/SPXSpeechApi.h" || { echo "Speech SDK headers not found." >&2; exit 1; }
	@test -f "$(SPEECHSDK_FRAMEWORK)/MicrosoftCognitiveServicesSpeech" || { echo "Speech SDK framework binary not found." >&2; exit 1; }

live-interpreter: main.m Makefile | check-sdk
	@framework_dir=$$(cd "$$(dirname "$(SPEECHSDK_FRAMEWORK)")" && pwd) && \
		xcrun --sdk macosx $(CC) -fobjc-arc -Wall $(CFLAGS) \
		-F"$$framework_dir" -framework Foundation -framework MicrosoftCognitiveServicesSpeech \
		-Wl,-rpath,"$$framework_dir" $(LDFLAGS) "$<" -o "$@"

clean:
	rm -f live-interpreter
