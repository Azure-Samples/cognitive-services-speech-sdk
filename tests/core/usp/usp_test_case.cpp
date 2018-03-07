//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include <chrono>
#include <thread>
#include <random>
#include "catch.hpp"
#include "test_utils.h"
#include "usp.h"


using namespace std;

#define REQUIRE_SUCESS(x) REQUIRE((x) == USP_SUCCESS)

class UspClient {
public:
    UspClient(UspEndpointType endpoint = USP_ENDPOINT_BING_SPEECH, 
        UspRecognitionMode mode = USP_RECO_MODE_INTERACTIVE) {
        m_callbacks.onSpeechStartDetected = NULL;
        m_callbacks.onSpeechEndDetected = NULL;
        m_callbacks.onSpeechHypothesis = NULL;
        m_callbacks.onSpeechPhrase = NULL;
        m_callbacks.onSpeechFragment = NULL;
        m_callbacks.onTurnStart = NULL;
        m_callbacks.onTurnEnd = NULL;
        m_callbacks.version = USP_CALLBACK_VERSION;
        m_callbacks.size = sizeof(UspCallbacks);
        m_callbacks.OnError = [](UspHandle, void*, const UspError* error) { FAIL(string(error->description)); };
        REQUIRE_SUCESS(UspInit(endpoint, mode, &m_callbacks, NULL, &m_handle));
        REQUIRE_SUCESS(UspConnect(m_handle));
    }
    ~UspClient() {
        REQUIRE_SUCESS(UspClose(m_handle));
    }
    
    template <class T>
    size_t WriteAudio(T* buffer, size_t size) {
        REQUIRE_SUCESS(UspWriteAudio(m_handle, reinterpret_cast<const uint8_t*>(buffer), size, &size));
        return size;
    }

private:
    UspCallbacks m_callbacks;
    UspHandle m_handle;
};


TEST_CASE("USP is properly functioning", "[usp]")
{    
    SECTION("usp can be initialized, connected and closed")
    {
        UspClient client;
        (void)(client);
    }

    wstring input_file(L"whatstheweatherlike.wav");
    REQUIRE(exists(input_file));

    SECTION("usp can be used to upload binary data")
    {
        string dummy = "RIFF1234567890";
        UspClient client;
        REQUIRE(client.WriteAudio(dummy.data(), dummy.length()) == dummy.length());
    }

    random_engine rnd(12345);
    size_t buffer_size_8k = 1 << 13;
    vector<char> buffer(buffer_size_8k);

    SECTION("usp can be used to upload audio from file")
    {
        UspClient client;
        auto is = get_stream(input_file);

        while (is) {
            auto size_to_read = max(size_t(1 << 10), rnd() % buffer_size_8k);
            is.read(buffer.data(), size_to_read);
            auto bytesRead = (size_t)is.gcount();
            auto count = client.WriteAudio(buffer.data(), bytesRead);
            REQUIRE(count == bytesRead);
            std::this_thread::sleep_for(std::chrono::milliseconds(rnd() % 100));
        }
    }

    SECTION("usp can toggled on/off multiple times in a row")
    {
        for (unsigned int i = 10; i > 0; i--) 
        {
            UspClient client;
            auto is = get_stream(input_file);
            while (is && (rnd()%i < i>>1)) {
                is.read(buffer.data(), buffer_size_8k);
                auto bytesRead = (size_t)is.gcount();
                auto count = client.WriteAudio(buffer.data(), bytesRead);
                REQUIRE(count == bytesRead);
                std::this_thread::sleep_for(std::chrono::milliseconds(rnd() % 100));
            }
        }
    }

    SECTION("several usp clients can coexist peacefully")
    {
        // TODO: 1117295 (this test fails on linux).
        int num_handles = 10;
        vector<UspClient> clients(num_handles);

        auto is = get_stream(input_file);
        is.read(buffer.data(), buffer_size_8k);
        REQUIRE(is.good());

        for (int i = 0; i < num_handles; i++)
        {
            auto bytesRead = (size_t)is.gcount();
            auto count = clients[i].WriteAudio(buffer.data(), bytesRead);
            REQUIRE(count == bytesRead);
        }

        while (is) {
            auto size_to_read = max(size_t(1 << 10), rnd() % buffer_size_8k);
            is.read(buffer.data(), size_to_read);
            auto bytesRead = (size_t)is.gcount();
            auto count = clients[rnd() % num_handles].WriteAudio(buffer.data(), bytesRead);
            REQUIRE(count == bytesRead);
            std::this_thread::sleep_for(std::chrono::milliseconds(rnd() % 100));
        }
    }

}