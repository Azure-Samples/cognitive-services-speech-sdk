//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <optional>
#include <set>
#include <sstream>
#include <thread>
#include <vector>
// You can download json.hpp from:
// https://github.com/nlohmann/json/releases
#include "json.hpp"
#include "json_helper.h"
#include "rest_helper.h"
#include "string_helper.h"
#include "user_config.h"

class CallCenter
{
private:
    // This should not change unless you switch to a new version of the Speech REST API.
    const std::string speechTranscriptionPath = "/speechtotext/v3.0/transcriptions";

    // These should not change unless you switch to a new version of the Cognitive Language REST API.
    const std::string sentimentAnalysisPath = "/language/:analyze-text";
    const std::string sentimentAnalysisQuery = "?api-version=2022-05-01";
    const std::string conversationAnalysisPath = "/language/analyze-conversations/jobs";
    const std::string conversationAnalysisQuery = "?api-version=2022-05-15-preview";
    const std::string conversationSummaryModelVersion = "2022-05-15-preview";

    // How long to wait while polling batch transcription and conversation analysis status.
    const int waitSeconds = 10;

    std::shared_ptr<UserConfig> m_userConfig = NULL;
    
public:
    CallCenter(std::shared_ptr<UserConfig> userConfig)
        : m_userConfig(userConfig)
    {
        RestHelper::Initialize();
        
        if (m_userConfig->outputFilePath.has_value())
        {
            std::filesystem::path outputFilePath { m_userConfig->outputFilePath.value() };
            // If the output file exists, truncate it.
            if (std::filesystem::exists(outputFilePath))
            {
                std::ofstream outputStream;
                outputStream.open(m_userConfig->outputFilePath.value());
                outputStream.close();            
            }
        }
    }

    ~CallCenter()
    {
        RestHelper::Dispose();
    }

    std::string CreateTranscription(std::string uriToTranscribe)
    {
        // Create Transcription REST API request and response JSON sample and schema:
        // https://westus.dev.cognitive.microsoft.com/docs/services/speech-to-text-api-v3-0/operations/CreateTranscription
        // Notes:
        // - locale and displayName are required.
        // - diarizationEnabled should only be used with mono audio input.
        std::string uri = m_userConfig->speechEndpoint + speechTranscriptionPath;
        nlohmann::json content = {
            // Note the value is a single-item array.
            {"contentUrls", {uriToTranscribe}},
            {"properties",
                // Start object definition
                {
                    {"diarizationEnabled", !m_userConfig->useStereoAudio},
                }
            },
            {"locale", m_userConfig->locale},
            {"displayName", "call_center_create_transcription"}
        };
        std::shared_ptr<RestResult> result = RestHelper::SendPost(m_userConfig->certificatePath, uri, content.dump(), m_userConfig->speechSubscriptionKey, std::set<int> { HTTP_CREATED });
        // The transcription ID is at the end of the transcription URI.
        std::string transcriptionUri = result->json["self"];
        std::string transcriptionId = StringHelper::Split(transcriptionUri, '/').back();
        // Verify the transcription ID is a valid GUID.
        if (!StringHelper::IsUUID(transcriptionId))
        {
            throw std::exception(std::string("Unable to parse response from Create Transcription API. Response:\n" + result->text).c_str());
        }
        return transcriptionId;
    }

    bool GetTranscriptionStatus(std::string transcriptionId)
    {
        // Get Transcription REST API request and response JSON sample and schema:
        // https://westus.dev.cognitive.microsoft.com/docs/services/speech-to-text-api-v3-0/operations/GetTranscription
        std::string uri = m_userConfig->speechEndpoint + speechTranscriptionPath + "/" + transcriptionId;
        std::shared_ptr<RestResult> result = RestHelper::SendGet(m_userConfig->certificatePath, uri, m_userConfig->speechSubscriptionKey, std::set<int> { HTTP_OK });
        if (StringHelper::CaseInsensitiveCompare("failed", result->json["status"].get<std::string>()))
        {
            throw std::exception(std::string("Unable to transcribe audio input. Response:\n" + result->text).c_str());
        }
        else
        {
            return StringHelper::CaseInsensitiveCompare("succeeded", result->json["status"].get<std::string>());
        }
    }

    void WaitForTranscription(std::string transcriptionId)
    {
        bool done = false;
        while (!done) {
            std::cout << "Waiting " + std::to_string(waitSeconds) + " seconds for transcription to complete." << std::endl;
            Sleep(waitSeconds * 1000);
            done = GetTranscriptionStatus(transcriptionId);
        }
        return;
    }

    std::shared_ptr<RestResult> GetTranscriptionFiles(std::string transcriptionId)
    {
        std::string uri = m_userConfig->speechEndpoint + speechTranscriptionPath + "/" + transcriptionId + "/files";
        return RestHelper::SendGet(m_userConfig->certificatePath, uri, m_userConfig->speechSubscriptionKey, std::set<int> { HTTP_OK });
    }

    std::string GetTranscriptionUri(std::shared_ptr<RestResult> transcriptionFiles)
    {
        // Get Transcription Files JSON response sample and schema:
        // https://westus.dev.cognitive.microsoft.com/docs/services/speech-to-text-api-v3-0/operations/GetTranscriptionFiles
        std::optional<nlohmann::json> transcription = JsonHelper::TryFirst([](nlohmann::json value) -> bool {
            return StringHelper::CaseInsensitiveCompare("transcription", value["kind"].get<std::string>());
        }, transcriptionFiles->json["values"]);
        if (!transcription.has_value())
        {
            throw std::exception(std::string("Unable to parse transcription files. Response:\n" + transcriptionFiles->text).c_str());
        }
        else
        {
            return transcription.value()["links"]["contentUrl"];
        }
    }

    nlohmann::json GetTranscription(std::string transcriptionUri)
    {
        return RestHelper::SendGet(m_userConfig->certificatePath, transcriptionUri, m_userConfig->speechSubscriptionKey, std::set<int> { HTTP_OK })->json;
    }

    nlohmann::json GetTranscriptionPhrasesAndSpeakers(nlohmann::json transcription)
    {
        nlohmann::json phrases = JsonHelper::Map([this](nlohmann::json phrase) -> nlohmann::json {
            nlohmann::json best = phrase["nBest"][0];
            return
            {
                {"phrase", best["display"]},
                // If the user specified stereo audio, and therefore we turned off diarization,
                // the speaker number is replaced by a channel number.
                {"speakerNumber", this->m_userConfig->useStereoAudio ? phrase["channel"] : phrase["speaker"]},
                {"offset", phrase["offsetInTicks"]}
            };
        }, transcription["recognizedPhrases"]);
        return m_userConfig->useStereoAudio
            // For stereo audio, the phrases are sorted by channel number, so resort them by offset.
            ? JsonHelper::SortBy([](nlohmann::json phrase_1, nlohmann::json phrase_2) -> bool { return phrase_1["offset"] < phrase_2["offset"]; }, phrases)
            : phrases;
    }

    void DeleteTranscription(std::string transcriptionId)
    {
        std::string uri = m_userConfig->speechEndpoint + speechTranscriptionPath + "/" + transcriptionId;
        RestHelper::SendDelete(m_userConfig->certificatePath, uri, m_userConfig->speechSubscriptionKey, std::set<int> { HTTP_NO_CONTENT });
        return;
    }
    
    nlohmann::json GetSentimentAnalysis(nlohmann::json phrasesAndSpeakers)
    {
        std::string uri = m_userConfig->languageEndpoint + sentimentAnalysisPath + sentimentAnalysisQuery;
        
        // Convert each transcription phrase to a "document" as expected by the sentiment analysis REST API.
        // Include a counter to use as a document ID.
        nlohmann::json documents_1 = JsonHelper::Map([this](nlohmann::json phraseAndSpeaker) -> nlohmann::json {
            static int id = 0;
            return
            {
                {"id", id++},
                {"language", this->m_userConfig->language},
                {"text", phraseAndSpeaker["phrase"]}
            };
        }, phrasesAndSpeakers);

        // We can only analyze sentiment for 10 documents per request.
        nlohmann::json documents_2 = JsonHelper::Chunk(documents_1, 10);
        
        nlohmann::json results_1 = JsonHelper::Map([this, uri](nlohmann::json documents_chunk) -> nlohmann::json {
            nlohmann::json content =
            {
                {"kind", "SentimentAnalysis"},
                {"analysisInput",
                    // Start object definition
                    {
                        {"documents", documents_chunk}
                    }
                }
            };
            return RestHelper::SendPost(this->m_userConfig->certificatePath, uri, content.dump(), this->m_userConfig->languageSubscriptionKey, std::set<int> { HTTP_OK })->json;
        }, documents_2);
        
        nlohmann::json result_documents_1 = JsonHelper::Map([](nlohmann::json results_chunk) -> nlohmann::json {
            return results_chunk["results"]["documents"];
        }, results_1);
        nlohmann::json result_documents_2 = JsonHelper::Concat(result_documents_1);
        
        return
        {
            {"kind", "SentimentAnalysisResults"},
            {"results",
                // Start object definition
                {
                    {"documents", result_documents_2}
                }
            }
        };
    }
    
    std::vector<std::string> GetSentiments(nlohmann::json sentimentAnalysis)
    {
        std::vector<std::string> retval;
        nlohmann::json documents_1 = sentimentAnalysis["results"]["documents"];
        nlohmann::json documents_2 = JsonHelper::SortBy([](nlohmann::json document_1, nlohmann::json document_2) -> bool {
            return std::stoi(document_1["id"].get<std::string>()) < std::stoi(document_2["id"].get<std::string>());
        }, documents_1);
        for (auto& item : documents_2)
        {
            retval.push_back(item["sentiment"].get<std::string>());
        }
        return retval;
    }
    
    nlohmann::json TranscriptionPhrasesToConversationItems(nlohmann::json transcriptionPhrasesAndSpeakers)
    {
        return JsonHelper::Map([](nlohmann::json phraseAndSpeaker) -> nlohmann::json {
            static int id = 0;
            return
            {
                {"id", id++},
                {"text", phraseAndSpeaker["phrase"]},
                // The first person to speak is probably the agent.
                {"role", 1 == phraseAndSpeaker["speakerNumber"] ? "Agent" : "Customer"},
                {"participantId", phraseAndSpeaker["speakerNumber"]}
            };
        }, transcriptionPhrasesAndSpeakers);
    }

    std::string RequestConversationAnalysis(nlohmann::json conversationItems)
    {
        std::string uri = m_userConfig->languageEndpoint + conversationAnalysisPath + conversationAnalysisQuery;
        nlohmann::json content =
        {
            {"displayName", "call_center_analyze_conversation_task"},
            {"analysisInput",
                // Start object definition
                {
                    {"conversations",
                        // Start array
                        {
                            // Start object definition
                            {
                                {"id", "conversation1"},
                                {"language", m_userConfig->language},
                                {"modality", "text"},
                                {"conversationItems", conversationItems}
                            }
                        }
                    }
                }
            },
            {"tasks",
                // Start array
                {
                    // Start object definition
                    {
                        {"taskName", "summary_1"},
                        {"kind", "ConversationalSummarizationTask"},
                        {"parameters",
                            // Start object definition
                            {
                                {"modelVersion", conversationSummaryModelVersion},
                                {"summaryAspects",
                                    // Start array
                                    {
                                        "Issue",
                                        "Resolution"
                                    }
                                }
                            }
                        }
                    },
                    // Start object definition
                    {
                        {"taskName", "PII_1"},
                        {"kind", "ConversationalPIITask"}
                    }
                }
            }
        };
        std::shared_ptr<RestResult> result = RestHelper::SendPost(m_userConfig->certificatePath, uri, content.dump(), m_userConfig->languageSubscriptionKey, std::set<int> { HTTP_ACCEPTED });
        return result->headers["operation-location"];
    }
    
    bool GetConversationAnalysisStatus(std::string conversationAnalysisUrl)
    {
        std::shared_ptr<RestResult> result = RestHelper::SendGet(m_userConfig->certificatePath, conversationAnalysisUrl, m_userConfig->languageSubscriptionKey, std::set<int> { HTTP_OK });
        if (StringHelper::CaseInsensitiveCompare("failed", result->json["status"].get<std::string>()))
        {
            throw std::exception(std::string("Unable to analyze conversation. Response:\n" + result->text).c_str());
        }
        else
        {
            return StringHelper::CaseInsensitiveCompare("succeeded", result->json["status"].get<std::string>());
        }
    }

    void WaitForConversationAnalysis(std::string conversationAnalysisUrl)
    {
        bool done = false;
        while (!done) {
            std::cout << "Waiting " + std::to_string(waitSeconds) + " seconds for conversation analysis to complete." << std::endl;
            Sleep(waitSeconds * 1000);
            done = GetConversationAnalysisStatus(conversationAnalysisUrl);
        }
        return;
    }
    
    nlohmann::json GetConversationAnalysis(std::string conversationAnalysisUrl)
    {
        return RestHelper::SendGet(m_userConfig->certificatePath, conversationAnalysisUrl, m_userConfig->languageSubscriptionKey, std::set<int> { HTTP_OK })->json;
    }

    nlohmann::json GetConversationAnalysisResult(nlohmann::json conversationAnalysis)
    {
        nlohmann::json tasks = conversationAnalysis["tasks"]["items"];
        
        nlohmann::json summaryTask = JsonHelper::TryFirst([](nlohmann::json task) -> bool {
            return StringHelper::CaseInsensitiveCompare("summary_1", task["taskName"].get<std::string>());
        }, tasks).value();
        nlohmann::json conversationSummaries = summaryTask["results"]["conversations"][0]["summaries"];
        nlohmann::json conversationSummary = JsonHelper::Map([](nlohmann::json summary) -> nlohmann::json {
            return
            {
                {"aspect", summary["aspect"]},
                {"summary", summary["text"]}
            };
        }, conversationSummaries);

        nlohmann::json PIITask = JsonHelper::TryFirst([](nlohmann::json task) -> bool {
            return StringHelper::CaseInsensitiveCompare("pii_1", task["taskName"].get<std::string>());
        }, tasks).value();
        nlohmann::json conversationItems = PIITask["results"]["conversations"][0]["conversationItems"];
        nlohmann::json conversationPIIAnalysis = JsonHelper::Map([](nlohmann::json conversationItem) -> nlohmann::json {
            return JsonHelper::Map([](nlohmann::json entity) -> nlohmann::json {
                return
                {
                    {"category", entity["category"]},
                    {"text", entity["text"]}
                };
            }, conversationItem["entities"]);
        }, conversationItems);

        return
        {
            {"conversationSummary", conversationSummary},
            {"conversationPIIAnalysis", conversationPIIAnalysis}
        };
    }
    
    // Create a string that contains each transcription phrase, followed by sentiment, PII, and so on.
    std::string GetResults(nlohmann::json transcriptionPhrasesAndSpeakers, std::vector<std::string> transcriptionSentiments, nlohmann::json conversationAnalysis)
    {
        std::ostringstream result;
        for (auto index = 0; index < transcriptionPhrasesAndSpeakers.size(); index++)
        {
            result << "Phrase: " << transcriptionPhrasesAndSpeakers[index]["phrase"] << "\n";
            result << "Speaker: " << transcriptionPhrasesAndSpeakers[index]["speakerNumber"] << "\n";
            if (index < transcriptionSentiments.size())
            {
                result << "Sentiment: " << transcriptionSentiments[index] << "\n";
            }
            if (index < conversationAnalysis["conversationPIIAnalysis"].size())
            {
                if (conversationAnalysis["conversationPIIAnalysis"][index].size() > 0)
                {
                    std::string entities = JsonHelper::Fold([](std::string acc, nlohmann::json entity) -> std::string {
                        std::ostringstream result;
                        result << acc << "    Category: " << entity["category"] << ". Text: " << entity["text"] << ".\n";
                        return result.str();
                    }, "Recognized entities (PII):\n", conversationAnalysis["conversationPIIAnalysis"][index]);
                    result << entities;
                }
                else
                {
                    result << "Recognized entities (PII): none.\n";
                }
            }
            result << "\n";
        }
        std::string summary = JsonHelper::Fold([](std::string acc, nlohmann::json item) -> std::string {
            std::ostringstream result;
            result << acc << "    Aspect: " << item["aspect"] << ". Summary: " << item["summary"] << ".\n";
            return result.str();
        }, "Conversation summary:\n", conversationAnalysis["conversationSummary"]);
        result << summary;
        
        return result.str();
    }
    
    void PrintResults(nlohmann::json transcription, nlohmann::json sentimentAnalysis, nlohmann::json conversationAnalysis)
    {
        nlohmann::json results =
        {
            {"transcription", transcription},
            {"sentimentAnalysis", sentimentAnalysis},
            {"conversationAnalysis", conversationAnalysis}
        };
        std::cout << results.dump(4) << std::endl;
    }
};

int main(int argc, char* argv[])
{    
    const std::string usage = "Usage: call_center.exe [...]\n\n"
"  HELP\n"
"    --help                        Show this help and stop.\n\n"
"  CONNECTION\n"
"    --certificate CERTIFICATE       The path to your cacert.pem file. Required.\n"
"                                    You can downloaded cacert.pem from:\n"
"                                    https://curl.se/ca/cacert.pem\n"
"    --speechKey KEY                 Your Azure Speech service subscription key. Required.\n"
"    --speechRegion REGION           Your Azure Speech service region. Required.\n"
"                                    Examples: westus, eastus\n"
"    --languageKey KEY               Your Azure Cognitive Language subscription key. Required.\n"
"    --languageEndpoint ENDPOINT     Your Azure Cognitive Language endpoint. Required.\n\n"
"  LANGUAGE\n"
"    --language LANGUAGE             The language to use for sentiment analysis and conversation analysis.\n"
"                                    This should be a two-letter ISO 639-1 code.\n"
"                                    Default: en\n"
"    --locale LOCALE                 The locale to use for batch transcription of audio.\n"
"                                    Default: en-US\n\n"
"  INPUT\n"
"    --input URL                     Input audio from URL. Required.\n"
"    --stereo                        Use stereo audio format.\n"
"                                    If this is not present, mono is assumed.\n\n"
"  OUTPUT\n"
"    --output FILE                   Output phrase list and conversation summary to text file.\n";

    try
    {
        if (CommandLineOptionExists(argv, argv + argc, "--help"))
        {
            std::cout << usage << std::endl;
        }
        else
        {
            std::shared_ptr<UserConfig> userConfig = UserConfigFromArgs(argc, argv, usage);
            auto callCenter = std::make_shared<CallCenter>(userConfig);

            // How to use batch transcription:
            // https://github.com/MicrosoftDocs/azure-docs/blob/main/articles/cognitive-services/Speech-Service/batch-transcription.md
            std::string transcriptionId = callCenter->CreateTranscription(userConfig->inputAudioURL);
            callCenter->WaitForTranscription(transcriptionId);
            std::cout << "Transcription ID: " << transcriptionId << std::endl;
            std::shared_ptr<RestResult> transcriptionFiles = callCenter->GetTranscriptionFiles(transcriptionId);
            std::string transcriptionUri = callCenter->GetTranscriptionUri(transcriptionFiles);
            nlohmann::json transcription = callCenter->GetTranscription(transcriptionUri);
            nlohmann::json phrasesAndSpeakers = callCenter->GetTranscriptionPhrasesAndSpeakers(transcription);
            nlohmann::json sentimentAnalysis = callCenter->GetSentimentAnalysis(phrasesAndSpeakers);
            nlohmann::json conversationItems = callCenter->TranscriptionPhrasesToConversationItems(phrasesAndSpeakers);
            // NOTE: Conversation summary is currently in gated public preview. You can sign up here:
            // https://aka.ms/applyforconversationsummarization/
            std::string conversationAnalysisUrl = callCenter->RequestConversationAnalysis(conversationItems);
            callCenter->WaitForConversationAnalysis(conversationAnalysisUrl);
            nlohmann::json conversationAnalysis = callCenter->GetConversationAnalysis(conversationAnalysisUrl);
            if (userConfig->outputFilePath.has_value())
            {
                nlohmann::json sentiments = callCenter->GetSentiments(sentimentAnalysis);
                nlohmann::json conversationAnalysisResult = callCenter->GetConversationAnalysisResult(conversationAnalysis);
                std::string results = callCenter->GetResults(phrasesAndSpeakers, sentiments, conversationAnalysisResult);
                std::ofstream outputStream;
                outputStream.open(userConfig->outputFilePath.value(), std::ios_base::app);
                outputStream << results;
                outputStream.close();
            }
            callCenter->PrintResults(transcription, sentimentAnalysis, conversationAnalysis);
            // Clean up resources.
            std::cout << "Deleting transcription.\n";
            callCenter->DeleteTranscription(transcriptionId);
        }
    }
    catch (std::exception e)
    {
        std::cout << e.what() << std::endl;
    }
}
