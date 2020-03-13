//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// uspcommon.h: common definitions and declaration used by USP internal implementation
//

#pragma once

#include <proxy_server_info.h>
#include <http_headers.h>
#include "usperror.h"

#define UNUSED(x) (void)(x)

#define USP_RETURN_NOT_IMPLEMENTED() \
    do { \
        LogError("Not implemented"); \
        return USP_NOT_IMPLEMENTED; \
    } while (0)

#define USE_BUFFER_SIZE    ((size_t)-1)

#if defined _MSC_VER
#define PROTOCOL_VIOLATION(__fmt, ...)  LogError("ProtocolViolation:" __fmt, __VA_ARGS__)
#else
#define PROTOCOL_VIOLATION(__fmt, ...)  LogError("ProtocolViolation:" __fmt, ##__VA_ARGS__)
#endif

#include <string>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace USP {

    class path
    {
    public:
        static const char* speechHypothesis;
        static const char* speechPhrase;
        static const char* speechFragment;
        static const char* speechKeyword;
        static const char* turnStart;
        static const char* turnEnd;
        static const char* speechStartDetected;
        static const char* speechEndDetected;
        static const char* translationHypothesis;
        static const char* translationPhrase;
        static const char* translationSynthesis;
        static const char* translationSynthesisEnd;
        static const char* translationResponse;
        static const char* audio;
        static const char* audioMetaData;
        static const char* audioStart;
        static const char* audioEnd;
    };

    class json_properties
    {
    public:
        static const char* offset;
        static const char* duration;
        static const char* status;
        static const char* text;
        static const char* recoStatus;
        static const char* displayText;
        static const char* context;
        static const char* tag;
        static const char* speaker;
        static const char* id;
        static const char* nbest;
        static const char* confidence;
        static const char* display;

        static const char* translation;
        static const char* translationStatus;
        static const char* failureReason;
        static const char* translations;
        static const char* synthesisStatus;
        static const char* lang;
        static const char* translationLanguage;

        static const char* metadata;
        static const char* type;
        static const char* data;
        static const char* wordBoundary;
        static const char* lowerText;

        static const char* primaryLanguage;
        static const char* speechHypothesis;
        static const char* speechPhrase;
    };

}
}
}
}

