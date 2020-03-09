//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"
#include "test_utils.h"
#include "microphone_pump.h"
#include <atomic>
#include <cstring>
#include <numeric>
#include <functional>
#include <memory>

#include "site_helpers.h"
#include "create_object_helpers.h"
#include "internal_audio_codec_adapter.h"

#include "ispxinterfaces.h"

using namespace Microsoft::CognitiveServices::Speech::Impl;
using namespace std;

const char c_silkV3Preamble[] = "#!SILK_V3";
const uint16_t c_silkTermMark = 0xffff;
const char c_silkContentType[] = "audio/SILK";

TEST_CASE("Silk codec can be loaded", "[audio][compression][codec]")
{
    shared_ptr<CSpxInternalAudioCodecAdapter> codecAdapter = make_shared<CSpxInternalAudioCodecAdapter>();

    SPXWAVEFORMATEX format{ 0 };
    format.wFormatTag = WAVE_FORMAT_PCM;
    format.nChannels = 1;
    format.nSamplesPerSec = 16000;
    format.nAvgBytesPerSec = 16000 * 2;
    format.nBlockAlign = 2;
    format.wBitsPerSample = 16;
    format.cbSize = 0;

    vector<uint8_t> receivedData;
    size_t receivedDataOffset = 0;
    int dataCalls = 0;
    bool finalReceived = false;
    auto dataCallBack =
        [&](const uint8_t* outData, size_t nBytesOut)
    {
        if (outData == NULL)
        {
            finalReceived = true;
        }
        else
        {
            if ((receivedData.capacity() - receivedDataOffset) < nBytesOut)
            {
                receivedData.reserve(receivedData.capacity() - receivedDataOffset + nBytesOut);
            }

            receivedData.insert(receivedData.end(), outData, outData + nBytesOut);
            receivedDataOffset += nBytesOut;
            dataCalls++;
        }
    };

    REQUIRE(SPX_NOERROR == codecAdapter->Load("Microsoft.CognitiveServices.Speech.extension.silk_codec.dll", "silk", dataCallBack));

    SECTION("Silk codec Init / Term")
    {
        REQUIRE_NOTHROW(codecAdapter->Init());
        REQUIRE_NOTHROW(codecAdapter->Term());
    }

    SECTION("Silk codec content type")
    {
        REQUIRE_NOTHROW(codecAdapter->Init());

        string contentType;
        REQUIRE_NOTHROW(contentType = codecAdapter->GetContentType());
        REQUIRE(contentType.compare(c_silkContentType) == 0);
        REQUIRE_NOTHROW(codecAdapter->Term());
    }

    SECTION("Silk codec InitCodec / Flush")
    {
        REQUIRE_NOTHROW(codecAdapter->Init());
        REQUIRE_NOTHROW(codecAdapter->InitCodec(&format));
        REQUIRE_NOTHROW(codecAdapter->Flush());

        size_t expectedSize = sizeof(c_silkV3Preamble) - 1 + sizeof(c_silkTermMark);
        REQUIRE(finalReceived);
        REQUIRE(dataCalls == 1);
        REQUIRE(receivedData.size() == expectedSize);
        REQUIRE(memcmp(receivedData.data(), c_silkV3Preamble, sizeof(c_silkV3Preamble) - 1) == 0);
        REQUIRE(memcmp(receivedData.data() + receivedData.size() - sizeof(c_silkTermMark), &c_silkTermMark, sizeof(c_silkTermMark)) == 0);
        REQUIRE_NOTHROW(codecAdapter->Term());
    }

    // one audio frame (100ms) - 3200 for 16kbps
    const int c_ZeroDataUnitSize = 3200;

    SECTION("Silk codec send 1 frame of zeros")
    {
        REQUIRE_NOTHROW(codecAdapter->Init());

        REQUIRE_NOTHROW(codecAdapter->InitCodec(&format));

        vector<uint8_t> dataToEncode;

        // .resize sets the buffer to 0
        dataToEncode.resize(c_ZeroDataUnitSize);
        REQUIRE_NOTHROW(codecAdapter->Encode(dataToEncode.data(), dataToEncode.size()));

        REQUIRE_NOTHROW(codecAdapter->Flush());

        size_t expectedPreTermSize = sizeof(c_silkV3Preamble) - 1 + sizeof(c_silkTermMark);
        REQUIRE(finalReceived);
        REQUIRE(dataCalls == 2);
        REQUIRE(receivedData.size() > expectedPreTermSize);
        REQUIRE(receivedData.size() < dataToEncode.size());
        REQUIRE(memcmp(receivedData.data(), c_silkV3Preamble, sizeof(c_silkV3Preamble) - 1) == 0);
        REQUIRE(memcmp(receivedData.data() + receivedData.size() - sizeof(c_silkTermMark), &c_silkTermMark, sizeof(c_silkTermMark)) == 0);
        REQUIRE_NOTHROW(codecAdapter->Term());
    }

    SECTION("Silk codec send 1/16 frame of zeros")
    {
        REQUIRE_NOTHROW(codecAdapter->Init());
        int uncompressedDataSize = c_ZeroDataUnitSize / 16;
        REQUIRE_NOTHROW(codecAdapter->InitCodec(&format));

        vector<uint8_t> dataToEncode;

        // .resize sets the buffer to 0
        dataToEncode.resize(uncompressedDataSize);
        REQUIRE_NOTHROW(codecAdapter->Encode(dataToEncode.data(), dataToEncode.size()));

        REQUIRE_NOTHROW(codecAdapter->Flush());

        size_t expectedPreTermSize = sizeof(c_silkV3Preamble) - 1 + sizeof(c_silkTermMark);
        REQUIRE(finalReceived);
        REQUIRE(dataCalls == 1);

        // not enought data for a 1/10th of a frame to encode but there will be padding added by Flush.
        REQUIRE(receivedData.size() > expectedPreTermSize);
        REQUIRE(memcmp(receivedData.data(), c_silkV3Preamble, sizeof(c_silkV3Preamble) - 1) == 0);
        REQUIRE(memcmp(receivedData.data() + receivedData.size() - sizeof(c_silkTermMark), &c_silkTermMark, sizeof(c_silkTermMark)) == 0);
        REQUIRE_NOTHROW(codecAdapter->Term());
    }

    SECTION("Silk codec send 10 frames of zeros")
    {
        const int c_iterationCount = 10;
        REQUIRE_NOTHROW(codecAdapter->Init());

        REQUIRE_NOTHROW(codecAdapter->InitCodec(&format));

        vector<uint8_t> dataToEncode;

        // .resize sets the buffer to 0
        dataToEncode.resize(c_ZeroDataUnitSize);
        for (int crtIter = 0; crtIter < c_iterationCount; crtIter++)
        {
            REQUIRE_NOTHROW(codecAdapter->Encode(dataToEncode.data(), dataToEncode.size()));
        }

        REQUIRE_NOTHROW(codecAdapter->Flush());

        size_t expectedPreTermSize = sizeof(c_silkV3Preamble) - 1 + sizeof(c_silkTermMark);
        REQUIRE(finalReceived);
        REQUIRE(dataCalls == c_iterationCount + 1);
        REQUIRE(receivedData.size() > expectedPreTermSize);
        REQUIRE(receivedData.size() < c_iterationCount * dataToEncode.size());
        REQUIRE(memcmp(receivedData.data(), c_silkV3Preamble, sizeof(c_silkV3Preamble) - 1) == 0);
        REQUIRE(memcmp(receivedData.data() + receivedData.size() - sizeof(c_silkTermMark), &c_silkTermMark, sizeof(c_silkTermMark)) == 0);
        REQUIRE_NOTHROW(codecAdapter->Term());
    }

    SECTION("Silk codec send 10 1/16 frames of zeros")
    {
        const int c_iterationCount = 10;
        int uncompressedDataSize = c_ZeroDataUnitSize / 16;
        REQUIRE_NOTHROW(codecAdapter->Init());

        REQUIRE_NOTHROW(codecAdapter->InitCodec(&format));

        vector<uint8_t> dataToEncode;

        // .resize sets the buffer to 0
        dataToEncode.resize(uncompressedDataSize);
        for (int crtIter = 0; crtIter < c_iterationCount; crtIter++)
        {
            REQUIRE_NOTHROW(codecAdapter->Encode(dataToEncode.data(), dataToEncode.size()));
        }

        REQUIRE_NOTHROW(codecAdapter->Flush());

        size_t expectedPreTermSize = sizeof(c_silkV3Preamble) - 1 + sizeof(c_silkTermMark);
        REQUIRE(finalReceived);
        REQUIRE(dataCalls == 1);
        REQUIRE(receivedData.size() > expectedPreTermSize);
        REQUIRE(receivedData.size() < c_iterationCount * dataToEncode.size());
        REQUIRE(memcmp(receivedData.data(), c_silkV3Preamble, sizeof(c_silkV3Preamble) - 1) == 0);
        REQUIRE(memcmp(receivedData.data() + receivedData.size() - sizeof(c_silkTermMark), &c_silkTermMark, sizeof(c_silkTermMark)) == 0);
        REQUIRE_NOTHROW(codecAdapter->Term());
    }

}
