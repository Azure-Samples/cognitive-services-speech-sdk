//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace USP {

    namespace Constants
    {
        constexpr auto HEADER_TIMESTAMP = "X-Timestamp";
        constexpr auto HEADER_PATH = "Path";
        constexpr auto HEADER_CONTENT_TYPE = "Content-Type";
        constexpr auto HEADER_REQUEST_ID = "X-RequestId";
        constexpr auto HEADER_PRESENTATION_TIMESTAMP = "PTS";
        constexpr auto HEADER_SPEAKER_ID = "SpeakerId";
        constexpr auto HEADER_STREAM_ID = "X-StreamId";

        constexpr auto CONTENT_TYPE_JSON = "application/json; charset=utf-8";
        constexpr auto CONTENT_TYPE_WAVE = "audio/x-wav";

        constexpr size_t HEADER_SIZE_IN_BYTES = 2;

        constexpr auto FORMAT_ISO_8601_TIMESTAMP = "%Y-%m-%dT%H:%M:%SZ";
    }

}}}}
