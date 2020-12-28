//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//


#include <iostream>
#include <strstream>
#include <Windows.h>
#include <locale>
#include <codecvt>
#include <string>

#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <nlohmann/json.hpp>

using namespace std;
using namespace utility;                    // Common utilities like string conversions
using namespace web;                        // Common features like URIs.
using namespace web::http;                  // Common HTTP functionality
using namespace web::http::client;          // HTTP client features
using namespace concurrency::streams;       // Asynchronous streams
using json = nlohmann::json;

const string_t region = U("YourServiceRegion");
const string_t subscriptionKey = U("YourSubscriptionKey");
const string name = "Simple transcription";
const string description = "Simple transcription description";
const string myLocale = "en-US";
const string recordingsBlobUri = "YourFileUrl";

class TranscriptionDefinition {
private:
    TranscriptionDefinition(string name,
        string description,
        string locale,
        string recordingsUrl,
        std::list<string> models) {

        Name = name;
        Description = description;
        RecordingsUrl = recordingsUrl;
        Locale = locale;
        Models = models;
    }

public:
    string Name;
    string Description;
    string RecordingsUrl;
    string Locale;
    std::list<string> Models;
    std::map<string, string> properties;

    static TranscriptionDefinition Create(string name, string description, string locale, string recordingsUrl) {
        return TranscriptionDefinition(name, description, locale, recordingsUrl, std::list<string>());
    }
    static TranscriptionDefinition Create(string name, string description, string locale, string recordingsUrl,
        std::list<string> models) {
        return TranscriptionDefinition(name, description, locale, recordingsUrl, models);
    }
};

void to_json(nlohmann::json& j, const TranscriptionDefinition& t) {
    j = nlohmann::json{
            { "description", t.Description },
            { "locale", t.Locale },
            { "models", t.Models },
            { "name", t.Name },
            { "properties", t.properties },
            { "recordingsurl",t.RecordingsUrl }
    };
};

void from_json(const nlohmann::json& j, TranscriptionDefinition& t) {
    j.at("locale").get_to(t.Locale);
    j.at("models").get_to(t.Models);
    j.at("name").get_to(t.Name);
    j.at("properties").get_to(t.properties);
    j.at("recordingsurl").get_to(t.RecordingsUrl);
}

class Transcription {
public:
    string name;
    string description;
    string locale;
    string recordingsUrl;
    map<string, string> resultsUrls;
    string id;
    string createdDateTime;
    string lastActionDateTime;
    string status;
    string statusMessage;
};

void to_json(nlohmann::json& j, const Transcription& t) {
    j = nlohmann::json{
            { "description", t.description },
            { "locale", t.locale },
            { "createddatetime", t.createdDateTime },
            { "name", t.name },
            { "id", t.id },
            { "recordingsurl",t.recordingsUrl },
            { "resultUrls", t.resultsUrls},
            { "status", t.status},
            { "statusMessage", t.statusMessage},
    };
};

void from_json(const nlohmann::json& j, Transcription& t) {
    j.at("description").get_to(t.description);
    j.at("locale").get_to(t.locale);
    j.at("createdDateTime").get_to(t.createdDateTime);
    j.at("name").get_to(t.name);
    j.at("recordingsUrl").get_to(t.recordingsUrl);
    t.resultsUrls = j.at("resultsUrls").get<map<string, string>>();
    j.at("status").get_to(t.status);
    t.statusMessage = j.value("statusMessage", "");
}
class Result
{
public:
    string Lexical;
    string ITN;
    string MaskedITN;
    string Display;
};
void from_json(const nlohmann::json& j, Result& r) {
    j.at("Lexical").get_to(r.Lexical);
    j.at("ITN").get_to(r.ITN);
    j.at("MaskedITN").get_to(r.MaskedITN);
    j.at("Display").get_to(r.Display);
}

class NBest : public Result
{
public:
    double Confidence;
};
void from_json(const nlohmann::json& j, NBest& nb) {
    j.at("Confidence").get_to(nb.Confidence);
    j.at("Lexical").get_to(nb.Lexical);
    j.at("ITN").get_to(nb.ITN);
    j.at("MaskedITN").get_to(nb.MaskedITN);
    j.at("Display").get_to(nb.Display);
}

class SegmentResult
{
public:
    string RecognitionStatus;
    ULONG Offset;
    ULONG Duration;
    std::list<NBest> NBest;
};
void from_json(const nlohmann::json& j, SegmentResult& sr) {
    j.at("RecognitionStatus").get_to(sr.RecognitionStatus);
    j.at("Offset").get_to(sr.Offset);
    j.at("Duration").get_to(sr.Duration);
    sr.NBest = j.at("NBest").get<list<NBest>>();
}

class AudioFileResult
{
public:
    string AudioFileName;
    std::list<SegmentResult> SegmentResults;
    std::list<Result> CombinedResults;
};
void from_json(const nlohmann::json& j, AudioFileResult& arf) {
    j.at("AudioFileName").get_to(arf.AudioFileName);
    arf.SegmentResults = j.at("SegmentResults").get<list<SegmentResult>>();
    arf.CombinedResults = j.at("CombinedResults").get<list<Result>>();
}

class RootObject {
public:
    std::list<AudioFileResult> AudioFileResults;
};
void from_json(const nlohmann::json& j, RootObject& r) {
    r.AudioFileResults = j.at("AudioFileResults").get<list<AudioFileResult>>();
}


void recognizeSpeech()
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t >> converter;

    utility::string_t service_url = U("https://") + region + U(".cris.ai/api/speechtotext/v2.0/Transcriptions/");
    uri u(service_url);

    http_client c(u);
    http_request msg(methods::POST);
    msg.headers().add(U("Content-Type"), U("application/json"));
    msg.headers().add(U("Ocp-Apim-Subscription-Key"), subscriptionKey);

    auto transportdef = TranscriptionDefinition::Create(name, description, myLocale, recordingsBlobUri);

    nlohmann::json transportdefJSON = transportdef;

    msg.set_body(transportdefJSON.dump());

    auto response = c.request(msg).get();
    auto statusCode = response.status_code();

    if (statusCode != status_codes::Accepted)
    {
        cout << "Unexpected status code " << statusCode << endl;
        return;
    }

    string_t transcriptionLocation = response.headers()[U("location")];

    cout << "Transcription status is located at " << converter.to_bytes(transcriptionLocation) << endl;

    http_client statusCheckClient(transcriptionLocation);
    http_request statusCheckMessage(methods::GET);
    statusCheckMessage.headers().add(U("Ocp-Apim-Subscription-Key"), subscriptionKey);

    bool completed = false;

    while (!completed)
    {
        auto statusResponse = statusCheckClient.request(statusCheckMessage).get();
        auto statusResponseCode = statusResponse.status_code();

        if (statusResponseCode != status_codes::OK)
        {
            cout << "Fetching the transcription returned unexpected http code " << statusResponseCode << endl;
            return;
        }

        auto body = statusResponse.extract_string().get();
        auto statusJSON = nlohmann::json::parse(body);
        Transcription transcriptionStatus = statusJSON;

        if (!_stricmp(transcriptionStatus.status.c_str(), "Failed"))
        {
            completed = true;
            cout << "Transcription has failed " << transcriptionStatus.statusMessage << endl;
        }
        else if (!_stricmp(transcriptionStatus.status.c_str(), "Succeeded"))
        {
            completed = true;
            cout << "Success!" << endl;
            string result = transcriptionStatus.resultsUrls["channel_0"];
            cout << "Transcription has completed. Results are at " << result << endl;
            cout << "Fetching results" << endl;

            http_client result_client(converter.from_bytes(result));
            http_request resultMessage(methods::GET);
            resultMessage.headers().add(U("Ocp-Apim-Subscription-Key"), subscriptionKey);

            auto resultResponse = result_client.request(resultMessage).get();

            auto responseCode = resultResponse.status_code();

            if (responseCode != status_codes::OK)
            {
                cout << "Fetching the transcription returned unexpected http code " << responseCode << endl;
                return;
            }

            auto resultBody = resultResponse.extract_string().get();

            nlohmann::json resultJSON = nlohmann::json::parse(resultBody);
            RootObject root = resultJSON;

            for (AudioFileResult af : root.AudioFileResults)
            {
                cout << "There were " << af.SegmentResults.size() << " results in " << af.AudioFileName << endl;

                for (SegmentResult segResult : af.SegmentResults)
                {
                    cout << "Status: " << segResult.RecognitionStatus << endl;

                    if (!_stricmp(segResult.RecognitionStatus.c_str(), "success") && segResult.NBest.size() > 0)
                    {
                        cout << "Best text result was: '" << segResult.NBest.front().Display << "'" << endl;
                    }
                }
            }
        }
        else if (!_stricmp(transcriptionStatus.status.c_str(), "Running"))
        {
            cout << "Transcription is running." << endl;
        }
        else if (!_stricmp(transcriptionStatus.status.c_str(), "NotStarted"))
        {
            cout << "Transcription has not started." << endl;
        }

        if (!completed) {
            Sleep(5000);
        }

    }
}

int wmain()
{
    try
    {
        recognizeSpeech();
    }
    catch (exception e)
    {
        cout << e.what();
    }
    cout << "Please press a key to continue.\n";
    cin.get();
    return 0;
}

