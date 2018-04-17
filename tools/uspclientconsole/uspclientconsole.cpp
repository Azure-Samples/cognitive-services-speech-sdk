//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// uspclientconsole.c: A console application for testing USP client library.
//

#include <stdio.h>

#include <stdbool.h>
#include <inttypes.h>
#include <fstream>
#include <thread>
#include <vector>
#include <map>
#include "audio_sys.h"

// #include "azure_c_shared_utility/audio_sys.h"

#include "usp.h"

#define UNUSED(x) (void)(x)

bool turnEnd = false;
std::thread ttsThread;

// TODO: MSFT 1135317 Move TTS to own source file
// We receive the audio response chunk by chunk (for example from TTS) and store it into a file. 
// Once the entire response is received, we play out the audio file using the audio system of the OS.
static int TTSRenderLoop(IOBUFFER* ioBuffer)
{
    std::ofstream file("data2.bin", std::ios::out | std::ios::binary);
    std::vector<char> buffer;
    AUDIO_SYS_HANDLE hAudio;

    while (IoBufferWaitForNewBytes(ioBuffer, 500) == 0)
    {
        int size = IoBufferGetUnReadBytes(ioBuffer);
        if (size == 0)
        {
            printf("Response: Final Audio Chunk received.\n");
            break;
        }
        if ((int)buffer.size() < size) {
            buffer.resize(size);
        }
        IoBufferRead(ioBuffer, buffer.data(), 0, size, 100);
        file.write(buffer.data(), size);
    }
    file.flush();
    auto fileSize = file.tellp();
    file.close();

    printf("Response: Playing TTS Audio...\n");

#ifdef __MACH__
    // TODO: merge 182438c675f0459f91a4c4f53ad5fd1cbba63ef0 from CortanaSDK
    printf("ERROR: audio system is not yet supported on OSX.");
#else
    hAudio = audio_create();
    audio_output_set_volume(hAudio, 50);
    if (hAudio == NULL)
    {
        printf("WARNING: No audio device");
        return -1;
    }
    audio_playwavfile(hAudio, "test.wav");

    // HACK: Rather than sleeping for the length of the audio...
    // we should modify audio_playwavfile to have a callback to be called when the audio is done playing. 
    auto numMilliSeconds = (fileSize * 8) / (256);
    std::this_thread::sleep_for(std::chrono::milliseconds(numMilliSeconds));
#endif

    IoBufferDelete(ioBuffer);
    return 1;
}

std::map<USP::RecognitionStatus, std::string> recognitionStatusToText =
{
    { USP::RecognitionStatus::Success, "Success" },
    { USP::RecognitionStatus::NoMatch, "No Match" },
    { USP::RecognitionStatus::InitialSilenceTimeout, "Initial Silence Timeout" },
    { USP::RecognitionStatus::BabbleTimeout, "Babble Timeout" },
    { USP::RecognitionStatus::Error, "Error" },
    { USP::RecognitionStatus::EndOfDictation, "End of dictation" }
};

class UspCallbacks : public USP::Callbacks {

protected:

virtual void OnSpeechStartDetected(const USP::SpeechStartDetectedMsg&) override
{
    // offset not supported yet.
    printf("Response: Speech.StartDetected message.\n");
    //printf("Response: Speech.StartDetected message. Speech starts at offset %" PRIu64 ".\n", message->offset);
}

virtual void OnSpeechEndDetected(const USP::SpeechEndDetectedMsg&) override
{
    // offset not supported yet.
    printf("Response: Speech.EndDetected message.\n");
    // printf("Response: Speech.EndDetected message. Speech ends at offset %" PRIu64 "\n", message->offset);
}

virtual void OnSpeechHypothesis(const USP::SpeechHypothesisMsg& message) override
{
    printf("Response: Speech.Hypothesis message. Text: %ls, starts at offset %" PRIu64 ", with duration %" PRIu64 ".\n", message.text.c_str(), message.offset, message.duration);
}

virtual void OnSpeechPhrase(const USP::SpeechPhraseMsg& message) override
{
    printf("Response: Speech.Phrase message. Status: %s, Text: %ls, starts at %" PRIu64 ", with duration %" PRIu64 ".\n", recognitionStatusToText[message.recognitionStatus].c_str(), message.displayText.c_str(), message.offset, message.duration);
}

virtual void OnSpeechFragment(const USP::SpeechFragmentMsg& message) override
{
    printf("Response: Speech.Fragment message. Text: %ls, starts at %" PRIu64 ", with duration %" PRIu64 ".\n", message.text.c_str(), message.offset, message.duration);
}

virtual void OnTurnStart(const USP::TurnStartMsg& message) override
{
    printf("Response: Turn.Start message. Context.ServiceTag: %s\n", message.contextServiceTag.c_str());
}

virtual void OnTurnEnd(const USP::TurnEndMsg&) override
{
    printf("Response: Turn.End message.\n");
    turnEnd = true;
}

virtual void OnError(const std::string& error) override
{
    printf("Response: On Error: %s.\n", error.c_str());
    turnEnd = true;
    exit(1);
}

virtual void OnUserMessage(const std::string& path, const std::string& contentType, const uint8_t* buffer, size_t size) override
{
    printf("Response: User defined message. Path: %s, contentType: %s, size: %zu, content: %s.\n", path.c_str(), contentType.c_str(), size, buffer);
}


virtual void OnAudioStreamStart(const USP::AudioStreamStartMsg& msg) override
{
    printf("Response: First Audio Chunk received.\n");
    IoBufferAddRef(msg.ioBuffer);
    ttsThread = std::thread(TTSRenderLoop, msg.ioBuffer);
}

virtual void OnTranslationHypothesis(const USP::TranslationHypothesisMsg& message) override
{
    printf("Response: Translation.Hypothesis message. Text: %ls, starts at offset %" PRIu64 ", with duration %" PRIu64 ".\n", message.text.c_str(), message.offset, message.duration);
    auto resultMap = message.translation.translations; 
    for (auto it = resultMap.begin(); it != resultMap.end(); ++it)
    {
        printf("          , tranlated to %ls: %ls,\n", it->first.c_str(), it->second.c_str());
    }
}

virtual void OnTranslationPhrase(const USP::TranslationPhraseMsg& message) override
{
    printf("Response: Speech.Phrase message. Status: %s, Text: %ls, starts at %" PRIu64 ", with duration %" PRIu64 ", Translation status: %d.\n", 
        recognitionStatusToText[message.recognitionStatus].c_str(), message.text.c_str(), message.offset, message.duration, (int)message.translation.translationStatus);
    auto resultMap = message.translation.translations;
    for (auto it = resultMap.begin(); it != resultMap.end(); ++it)
    {
        printf("          , tranlated to %ls: %ls,\n", it->first.c_str(), it->second.c_str());
    }
}

};

#define AUDIO_BYTES_PER_SECOND (16000*2) //16KHz, 16bit PCM

int main(int argc, char* argv[])
{
    UspCallbacks testCallbacks;
    USP::Client client(testCallbacks, USP::EndpointType::BingSpeech);
    bool isAudioMessage = true;
    int curArg = 0;
    char* authData = NULL;
    char* messagePath = NULL;

    if (argc < 4)
    {
        printf("Usage: uspclientconsole message_type(audio/message:[path]) file tts(true/false) authentication endpoint_type(speech/cris/cdsdk/url) mode(interactive/conversation/dictation) language output(simple/detailed) user-defined-messages");
        exit(1);
    }

    curArg++;
    if (strstr(argv[curArg], "message") != NULL)
    {
        isAudioMessage = false;
        char *path = strchr(argv[curArg], ':');
        if (path == NULL)
        {
            //default message path if not specified
            messagePath = (char*)"message";
        }
        else
        {
            messagePath = ++path;
        }

    }
    else if (strcmp(argv[curArg], "audio") == 0)
    {
        isAudioMessage = true;
    }
    else
    {
        printf("unknown message type: %s\n", argv[curArg]);
        exit(1);
    }

    // Read input file.
    curArg++;
    std::ifstream data(argv[curArg], std::ios::in | ((isAudioMessage) ? std::ios::binary : std::ios::in));
    if (!data.is_open() || data.fail())
    {
        printf("Error: open file %s failed", argv[curArg]);
        exit(1);
    }

    curArg++;
    if (strcmp(argv[curArg], "true") == 0)
    {
        // do nothing.
    }

    curArg++;
    if (argc > curArg)
    {
        authData = argv[curArg];
    }

    curArg++;
    // Set service endpoint type.
    if (argc > curArg)
    {
        if (strcmp(argv[curArg], "speech") == 0)
        {
            client.SetEndpointType(USP::EndpointType::BingSpeech);
        }
        else if (strcmp(argv[curArg], "cris") == 0)
        {
            client.SetEndpointType(USP::EndpointType::Cris);
        }
        else if (strcmp(argv[curArg], "cdsdk") == 0)
        {
            client.SetEndpointType(USP::EndpointType::CDSDK);
        }
        else if (strcmp(argv[curArg], "url") == 0)
        {
            client.SetEndpointType(USP::EndpointType::Custom);
            if (argc != curArg + 2)
            {
                printf("No URL specified or too many parameters.\n");
                exit(1);
            }
        }
        else
        {
            printf("unknown service endpoint type: %s\n", argv[curArg]);
            exit(1);
        }
    }

    curArg++;
    // Set recognition mode.
    if (argc > curArg)
    {
        if (client.GetEndpointType() != USP::EndpointType::Custom)
        {
            if (strcmp(argv[curArg], "interactive") == 0)
            {
                client.SetRecognitionMode(USP::RecognitionMode::Interactive);
            }
            else if (strcmp(argv[curArg], "conversation") == 0)
            {
                client.SetRecognitionMode(USP::RecognitionMode::Conversation);
            }
            else if (strcmp(argv[curArg], "dictation") == 0)
            {
                client.SetRecognitionMode(USP::RecognitionMode::Dictation);
            }
            else
            {
                printf("unknown reco mode: %s\n", argv[curArg]);
                exit(1);
            }
        }
    }

    // Create USP handle.
    if (client.GetEndpointType() == USP::EndpointType::Custom)
    {
        printf("Using custom URL %s.\n", argv[curArg]);
        client.SetEndpointUrl(argv[curArg]);
    }

    // Set Authentication.
    if (authData != NULL)
    {
        auto type = USP::AuthenticationType::SubscriptionKey;
        if (client.GetEndpointType() == USP::EndpointType::CDSDK) 
        {
            type = USP::AuthenticationType::SearchDelegationRPSToken;
        }

        client.SetAuthentication(type, authData);
    }

    curArg++;
    // Set language.
    if (argc > curArg)
    {
        if (client.GetEndpointType() == USP::EndpointType::Cris)
        {
            client.SetModelId(argv[curArg]);
        }
        else
        {
            client.SetLanguage(argv[curArg]);
        }
    }

    curArg++;
    // Set output format if needed.
    if (argc > curArg)
    {
        if (strcmp(argv[curArg], "detailed") == 0)
        {
            client.SetOutputFormat(USP::OutputFormat::Detailed);
        }
        else if (strcmp(argv[curArg], "simple") == 0)
        {
            client.SetOutputFormat(USP::OutputFormat::Simple);
        }
        else
        {
            printf("unknown output format: %s\n", argv[curArg]);
            exit(1);
        }
    }

    // Connect to service
    auto connection = client.Connect();

    turnEnd = false;

    // Send data to service
    std::vector<char> buffer;
    size_t totalBytesWritten {0};

    data.seekg(0L, data.end);
    size_t fileSize = static_cast<size_t>(data.tellg());
    data.seekg(0L, data.beg);

    if (isAudioMessage)
    {
        size_t chunkSize = AUDIO_BYTES_PER_SECOND * 2 / 5; // 2x real-time, 5 chunks per second.
        buffer.resize(chunkSize);
        while (!data.eof())
        {
            data.read(buffer.data(), chunkSize);
            size_t bytesToWrite = static_cast<size_t>(data.gcount());
            connection->WriteAudio(reinterpret_cast<uint8_t*>(buffer.data()), bytesToWrite);
            totalBytesWritten += (size_t) bytesToWrite;
            // Sleep to simulate real-time traffic
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }

        // Send End of Audio to service to close the session.
        if (data.eof())
        {
            connection->FlushAudio();
        }
    }
    else
    {
        buffer.resize(fileSize);
        data.read(buffer.data(), fileSize);
        auto bytesToWrite = static_cast<size_t>(data.gcount());
        connection->SendMessage(messagePath, reinterpret_cast<uint8_t*>(buffer.data()), bytesToWrite);
        totalBytesWritten += bytesToWrite;
    }

    if (totalBytesWritten != fileSize)
    {
        printf("%s: Error: the total number of bytes sent (%zu) does not match the file size (%zu).\n", __FUNCTION__, totalBytesWritten, fileSize);
        exit(1);
    }
    else
    {
        printf("%s: Totally send %zu bytes of data.\n", __FUNCTION__, totalBytesWritten);
    }

    // Wait for end of recognition.
    while (!turnEnd)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    connection.reset();

    if (ttsThread.joinable())
    {
        ttsThread.join();
    }

    data.close();
}
