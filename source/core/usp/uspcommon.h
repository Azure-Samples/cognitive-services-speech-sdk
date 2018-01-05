//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// uspcommon.h: common definitions and declaration used by USP internal implementation
//

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define USE_BUFFER_SIZE    ((size_t)-1)

#if defined _MSC_VER
#define PROTOCOL_VIOLATION(__fmt, ...)  LogError("ProtocolViolation:" __fmt, __VA_ARGS__)
#else
#define PROTOCOL_VIOLATION(__fmt, ...)  LogError("ProtocolViolation:" __fmt, ##__VA_ARGS__)
#endif

// Todo: change the name to follow coding convention.
extern const char* g_keywordContentType;

extern const char g_messagePathSpeechHypothesis[];
extern const char g_messagePathSpeechPhrase[];
extern const char g_messagePathTurnStart[];
extern const char g_messagePathTurnEnd[];
extern const char g_messagePathSpeechEndDetected[];
extern const char g_messagePathSpeechStartDetected[];
extern const char g_messagePathResponse[];

#ifdef __cplusplus
} // extern "C" 
#endif
