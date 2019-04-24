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
#include <string>
#include <map>
#include "audio_sys.h"
#include "guid_utils.h"

#include "exception.h"
#define __SPX_THROW_HR_IMPL(hr) Microsoft::CognitiveServices::Speech::Impl::ThrowWithCallstack(hr)
#include "thread_service.h"

#include <iostream>
#include <assert.h>

// #include "azure_c_shared_utility/audio_sys.h"

#include "usp.h"

#define UNUSED(x) (void)(x)

#ifndef _MSC_VER
#define localtime_s(__tm, __timet) localtime_r(__timet, __tm)
#define gmtime_s(__tm, __timet) gmtime_r(__timet, __tm)
#endif

using namespace std;
using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Impl;

bool turnEnd = false;

map<USP::RecognitionStatus, string> recognitionStatusToText =
{
    { USP::RecognitionStatus::Success, "Success" },
    { USP::RecognitionStatus::NoMatch, "No Match" },
    { USP::RecognitionStatus::InitialSilenceTimeout, "Initial Silence Timeout" },
    { USP::RecognitionStatus::InitialBabbleTimeout, "Initial Babble Timeout" },
    { USP::RecognitionStatus::Error, "Error" },
    { USP::RecognitionStatus::TooManyRequests, "Too Many Requests. The number of allowed concurrent transcriptions for the subscription is exceeded." },
    { USP::RecognitionStatus::BadRequest, "Invalid parameter or unsupported audio format." },
    { USP::RecognitionStatus::Forbidden, "The free subscription used by the request ran out of quota." },
    { USP::RecognitionStatus::ServiceUnavailable, "The service is unavailable." },
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

virtual void OnError(bool /*transport*/, USP::ErrorCode errorCode, const string& errorMessage) override
{
    printf("Response: On Error: ErrorCode: %d, ErrorMessage: %s.\n", (int)errorCode, errorMessage.c_str());
    turnEnd = true;
    exit(1);
}

virtual void OnUserMessage(const USP::UserMsg& msg) override
{
    printf("Response: User defined message. Path: %s, contentType: %s, size: %zu, content: %s.\n", msg.path.c_str(), msg.contentType.c_str(), msg.size, msg.buffer);
}

virtual void OnTranslationHypothesis(const USP::TranslationHypothesisMsg& message) override
{
    printf("Response: Translation.Hypothesis message. Text: %ls, starts at offset %" PRIu64 ", with duration %" PRIu64 ".\n", message.text.c_str(), message.offset, message.duration);
    auto resultMap = message.translation.translations;
    for (const auto& it : resultMap)
    {
        printf("          , tranlated to %ls: %ls,\n", it.first.c_str(), it.second.c_str());
    }
}

virtual void OnTranslationPhrase(const USP::TranslationPhraseMsg& message) override
{
    printf("Response: Speech.Phrase message. Status: %s, Text: %ls, starts at %" PRIu64 ", with duration %" PRIu64 ", Translation status: %d.\n",
        recognitionStatusToText[message.recognitionStatus].c_str(), message.text.c_str(), message.offset, message.duration, (int)message.translation.translationStatus);
    auto resultMap = message.translation.translations;
    for (const auto& it : resultMap)
    {
        printf("          , translated to %ls: %ls,\n", it.first.c_str(), it.second.c_str());
    }
}

};

#define AUDIO_BYTES_PER_SECOND (16000*2) // 16 kHz, 16 bit PCM

const map<string, USP::EndpointType> typeMap = {
    { "speech", USP::EndpointType::Speech },
    { "intent", USP::EndpointType::Intent },
    { "translation", USP::EndpointType::Translation },
    { "cdsdk", USP::EndpointType::CDSDK }
};

const map<string, USP::RecognitionMode> modeMap = {
    { "interactive", USP::RecognitionMode::Interactive },
    { "conversation", USP::RecognitionMode::Conversation },
    { "dictation", USP::RecognitionMode::Dictation }
};

#if defined(_MSC_VER) && defined(_DEBUG)
// in case of asserts in debug mode, print the message into stderr and throw exception
int HandleDebugAssert(int,               // reportType  - ignoring reportType, printing message and aborting for all reportTypes
    char *message,     // message     - fully assembled debug user message
    int * returnValue) // returnValue - retVal value of zero continues execution
{
    fprintf(stderr, "C-Runtime: %s\n", message);

    if (returnValue) {
        *returnValue = 0;   // return value of 0 will continue operation and NOT start the debugger
    }

    return 1;            // make sure no message box is displayed
}
#endif

int main(int argc, char* argv[])
{
    auto testCallbacks = std::make_shared<UspCallbacks>();

    bool isAudioMessage = true;
    bool useFiddlerProxy = false;
    string inputMessagePath;
    string authData;
    string endpointTypeStr;
    string recoMode;
    string customUrl;
    string modelId;
    string language;
    string format;
    string inputFile;
    string region{ "westus" };

#if defined(_MSC_VER) && defined(_DEBUG)
    // in case of asserts in debug mode, print the message into stderr and throw exception
    if (_CrtSetReportHook2(_CRT_RPTHOOK_INSTALL, HandleDebugAssert) == -1) {
        fprintf(stderr, "_CrtSetReportHook2 failed.\n");
        return -1;
    }
#endif

    if (argc < 2)
    {
        printf("Usage: uspclientconsole options inputfile\n");
        printf("The following options are available:\n");
        printf("      inputType:audio|message:[path]\n");
        printf("      auth:key\n");
        printf("      region:region\n");
        printf("      type:speech|intent|translation|cdsdk\n");
        printf("      mode:interactive|conversation|dictation\n");
        printf("      url:endpoint\n");
        printf("      lang:language\n");
        printf("      model:id\n");
        printf("      output:simple|detailed\n");
        printf("      useFiddlerProxy");
        exit(1);
    }

    for (int curArg = 1; curArg < argc; curArg++)
    {
        string argStr(argv[curArg]);
        size_t pos;

        if (argStr.find("inputType:") != string::npos)
        {
            pos = argStr.find("inputType:message");
            if (pos != string::npos)
            {
                isAudioMessage = false;
                if (argStr.find("inputType:message:") == string::npos)
                {
                    //default message path if not specified
                    inputMessagePath = "message";
                }
                else
                {
                    pos = argStr.rfind(':');
                    inputMessagePath = argStr.substr(pos + 1);
                }
            }
            else if (argStr.compare("inputType:audio") == 0)
            {
                isAudioMessage = true;
            }
            else
            {
                printf("Invalid inputType: %s", argStr.c_str());
                exit(1);
            }
        }
        else if (argStr.find("auth:") != string::npos)
        {
            pos = argStr.find(':');
            authData = argStr.substr(pos + 1);
        }
        else if (argStr.find("region:") != string::npos)
        {
            pos = argStr.find(':');
            region = argStr.substr(pos + 1);
        }
        else if (argStr.find("type:") != string::npos)
        {
            pos = argStr.find(':');
            endpointTypeStr = argStr.substr(pos + 1);
        }
        else if (argStr.find("mode:") != string::npos)
        {
            pos = argStr.find(':');
            recoMode = argStr.substr(pos + 1);
        }
        else if (argStr.find("url:") != string::npos)
        {
            pos = argStr.find(':');
            customUrl = argStr.substr(pos + 1);
        }
        else if (argStr.find("lang:") != string::npos)
        {
            pos = argStr.find(':');
            language = argStr.substr(pos + 1);
        }
        else if (argStr.find("model:") != string::npos)
        {
            pos = argStr.find(':');
            modelId = argStr.substr(pos + 1);
        }
        else if (argStr.find("output:") != string::npos)
        {
            pos = argStr.find(':');
            format = argStr.substr(pos + 1);
        }
        else if (argStr.find("useFiddlerProxy") != string::npos)
        {
            useFiddlerProxy = true;
        }
        else
        {
            if (!inputFile.empty())
            {
                printf("Only one input file is allowed.");
                exit(1);
            }
            else
            {
                inputFile = argStr;
            }
        }
    }

    USP::EndpointType endpointType;
    if (!endpointTypeStr.empty())
    {
        auto it = typeMap.find(endpointTypeStr);
        if (it == typeMap.end())
        {
            printf("Invalid endpoint type: %s", endpointTypeStr.c_str());
            exit(1);
        }
        else
        {
            endpointType = it->second;
        }
    }
    else
    {
        printf("No endpoint type is provided.");
        exit(1);
    }

    auto connectionId = PAL::CreateGuidWithoutDashes();

    {
        struct tm newtime;
        char buf[32];
        time_t now;
        time(&now);
        gmtime_s(&newtime, &now);
        strftime(buf, sizeof buf, "%FT%TZ", &newtime);
        printf("Connection ID '%ls' at %s\n", connectionId.c_str(), buf);
    }

    auto threadService = std::make_shared<CSpxThreadService>();
    threadService->Init();
    USP::Client client(testCallbacks, endpointType, connectionId, threadService);
    client.SetRegion(region);
    if (!customUrl.empty())
    {
        client.SetEndpointUrl(customUrl.c_str());
    }

    // Set Authentication.
    if (!authData.empty())
    {
        vector<string> authInfo((size_t)USP::AuthenticationType::SIZE_AUTHENTICATION_TYPE);
        if (client.GetEndpointType() == USP::EndpointType::CDSDK)
        {
            authInfo[(size_t)USP::AuthenticationType::SearchDelegationRPSToken] = authData;;
        }
        else
        {
            authInfo[(size_t)USP::AuthenticationType::SubscriptionKey] = authData;
        }

        client.SetAuthentication(authInfo);
    }

    if (useFiddlerProxy == true)
    {
        client.SetProxyServerInfo("localhost", 8888, nullptr, nullptr);
    }

    if (!recoMode.empty())
    {
        auto it = modeMap.find(recoMode);
        if (it == modeMap.end())
        {
            printf("Invalid recognition mode: %s", recoMode.c_str());
            exit(1);
        }
        else
        {
            client.SetRecognitionMode(it->second);
        }
    }

    if (!modelId.empty())
    {
        client.SetModelId(modelId.c_str());
    }

    if (!language.empty())
    {
        client.SetLanguage(language.c_str());
    }

    if (!format.empty())
    {
        if (format.compare("detailed") == 0)
        {
            client.SetOutputFormat(USP::OutputFormat::Detailed);
        }
        else if (format.compare("simple") == 0)
        {
            client.SetOutputFormat(USP::OutputFormat::Simple);
        }
        else
        {
            printf("unknown output format: %s\n", format.c_str());
            exit(1);
        }
    }

    ifstream input(inputFile, ios::in | ((isAudioMessage) ? ios::binary : ios::in));
    if (!input.is_open() || input.fail())
    {
        printf("Error: open file %s failed", inputFile.c_str());
        exit(1);
    }

    // Connect to service
    auto connection = client.Connect();

    size_t totalBytesWritten{ 0 };
    input.seekg(0L, input.end);
    size_t fileSize = static_cast<size_t>(input.tellg());
    input.seekg(0L, input.beg);

    turnEnd = false;

    if (isAudioMessage)
    {
        // Send data to service
        constexpr uint32_t chunkSize = AUDIO_BYTES_PER_SECOND * 2 / 5; // 2x real-time, 5 chunks per second.
        std::shared_ptr<uint8_t> buffer(new uint8_t[chunkSize], [](uint8_t* p) { delete[] p; });
        auto audioChunk = std::make_shared<Microsoft::CognitiveServices::Speech::Impl::DataChunk>(buffer, chunkSize);

        while (!input.eof())
        {
            input.read((char *)buffer.get(), chunkSize);
            size_t bytesToWrite = static_cast<size_t>(input.gcount());
            connection->WriteAudio(audioChunk);
            totalBytesWritten += (size_t) bytesToWrite;
            // Sleep to simulate real-time traffic
            this_thread::sleep_for(chrono::milliseconds(200));
        }

        // Send End of Audio to service to close the session.
        if (input.eof())
        {
            connection->FlushAudio();
        }
    }
    else
    {
        vector<char> msgBuffer;
        msgBuffer.resize(fileSize);
        input.read(msgBuffer.data(), fileSize);
        auto bytesToWrite = static_cast<size_t>(input.gcount());
        connection->SendMessage(inputMessagePath.c_str(), reinterpret_cast<uint8_t*>(msgBuffer.data()), bytesToWrite, USP::MessageType::Agent);
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
        this_thread::sleep_for(chrono::milliseconds(500));
    }

    connection.reset();

    input.close();
}
