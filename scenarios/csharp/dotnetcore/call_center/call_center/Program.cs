//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Text;
using System.Text.Json;
using System.Threading;
using System.Threading.Tasks;

namespace CallCenter
{
    public class ConversationAnalysisResult
    {
        readonly public (string aspect, string summary)[] summary;
        readonly public (string category, string text)[][] PIIAnalysis;
        
        public ConversationAnalysisResult((string, string)[] summary, (string, string)[][] PIIAnalysis)
        {
            this.summary = summary;
            this.PIIAnalysis = PIIAnalysis;
        }
    }
    
    public class Program
    {
        private UserConfig userConfig;
        
        // How long to wait while polling batch transcription and conversation analysis status.
        private const int waitSeconds = 10;

        // This should not change unless you switch to a new version of the Speech REST API.
        private const string speechTranscriptionPath = "speechtotext/v3.0/transcriptions";

        // These should not change unless you switch to a new version of the Cognitive Language REST API.
        private const string sentimentAnalysisPath = "language/:analyze-text";
        private const string sentimentAnalysisQuery = "api-version=2022-05-01";
        private const string conversationAnalysisPath = "/language/analyze-conversations/jobs";
        private const string conversationAnalysisQuery = "api-version=2022-05-15-preview";
        private const string conversationSummaryModelVersion = "2022-05-15-preview";

        private async Task<string> CreateTranscription()
        {
            var uri = new UriBuilder(Uri.UriSchemeHttps, this.userConfig.speechEndpoint);
            uri.Path = speechTranscriptionPath;

            // Create Transcription API JSON request sample and schema:
            // https://westus.dev.cognitive.microsoft.com/docs/services/speech-to-text-api-v3-0/operations/CreateTranscription
            // Notes:
            // - locale and displayName are required.
            // - diarizationEnabled should only be used with mono audio input.
            var content = new
                {
                    contentUrls = new string[] { this.userConfig.inputAudioURL },
                    properties = new { diarizationEnabled = !this.userConfig.useStereoAudio },
                    locale = this.userConfig.locale,
                    displayName = $"call_center_{DateTime.Now.ToString()}"
                };
            var response = await RestHelper.SendPost(uri.Uri.ToString(), JsonSerializer.Serialize(content), this.userConfig.speechSubscriptionKey, new HttpStatusCode[]{ HttpStatusCode.Created });
            // NOTE: Clone the root element of the JsonDocument. Otherwise, once it is disposed,
            // when you try to access any data you obtained from it, you may get the error:
            // Cannot access a disposed object.
            // Object name: 'JsonDocument'.
            // See:
            // https://docs.microsoft.com/dotnet/api/system.text.json.jsonelement.clone
            using (JsonDocument document = JsonDocument.Parse(response.content))
            {
                // Create Transcription API JSON response sample and schema:
                // https://westus.dev.cognitive.microsoft.com/docs/services/speech-to-text-api-v3-0/operations/CreateTranscription
                var transcriptionUri = document.RootElement.Clone().GetProperty("self").ToString();
                // The transcription ID is at the end of the transcription URI.
                var transcriptionId = transcriptionUri.Split("/").Last();
                // Verify the transcription ID is a valid GUID.
                Guid guid;
                if (!Guid.TryParse(transcriptionId, out guid))
                {
                    throw new Exception($"Unable to parse response from Create Transcription API:{Environment.NewLine}{response.content}");
                }
                return transcriptionId;
            }
        }

        private async Task<bool> GetTranscriptionStatus(string transcriptionId)
        {
            var uri = new UriBuilder(Uri.UriSchemeHttps, this.userConfig.speechEndpoint);
            uri.Path = $"{speechTranscriptionPath}/{transcriptionId}";
            var response = await RestHelper.SendGet(uri.Uri.ToString(), this.userConfig.speechSubscriptionKey, new HttpStatusCode[]{ HttpStatusCode.OK });
            using (JsonDocument document = JsonDocument.Parse(response.content))
            {
                if (0 == string.Compare("Failed", document.RootElement.Clone().GetProperty("status").ToString(), StringComparison.InvariantCultureIgnoreCase))
                {
                    throw new Exception($"Unable to transcribe audio input. Response:{Environment.NewLine}{response.content}");
                }
                else
                {
                    return 0 == string.Compare("Succeeded", document.RootElement.Clone().GetProperty("status").ToString(), StringComparison.InvariantCultureIgnoreCase);
                }
            }
        }

        private async Task WaitForTranscription(string transcriptionId)
        {
            var done = false;
            while(!done)
            {
                Console.WriteLine($"Waiting {waitSeconds} seconds for transcription to complete.");
                Thread.Sleep(waitSeconds * 1000);
                done = await GetTranscriptionStatus(transcriptionId);
            }
            return;
        }

        private async Task<string> GetTranscriptionFiles(string transcriptionId)
        {
            var uri = new UriBuilder(Uri.UriSchemeHttps, this.userConfig.speechEndpoint);
            uri.Path = $"{speechTranscriptionPath}/{transcriptionId}/files";
            var response = await RestHelper.SendGet(uri.Uri.ToString(), this.userConfig.speechSubscriptionKey, new HttpStatusCode[]{ HttpStatusCode.OK });
            return response.content;
        }

        private string GetTranscriptionUri(string transcriptionFiles)
        {
            using (JsonDocument document = JsonDocument.Parse(transcriptionFiles))
            {
                // Get Transcription Files JSON response sample and schema:
                // https://westus.dev.cognitive.microsoft.com/docs/services/speech-to-text-api-v3-0/operations/GetTranscriptionFiles
                return document.RootElement.Clone().GetProperty("values").EnumerateArray().ToArray()
                    .First(value => 0 == String.Compare("Transcription", value.GetProperty("kind").ToString(), StringComparison.InvariantCultureIgnoreCase))
                    .GetProperty("links").GetProperty("contentUrl").ToString();
            }
        }

        private async Task<JsonElement> GetTranscription(string transcriptionUri)
        {
            var response = await RestHelper.SendGet(transcriptionUri, "", new HttpStatusCode[]{ HttpStatusCode.OK });
            using (JsonDocument document = JsonDocument.Parse(response.content))
            {
                return document.RootElement.Clone();
            }
        }

        private (string phrase, int speakerID)[] GetTranscriptionPhrasesAndSpeakers(JsonElement transcription)
        {
            var recognizedPhrases_1 = transcription.GetProperty("recognizedPhrases").EnumerateArray().ToArray();
            var recognizedPhrases_2 = recognizedPhrases_1.Select(phrase =>
                {
                    var best = phrase.GetProperty("nBest").EnumerateArray().ToArray().First();
                    return new {
                        phrase = best.GetProperty("display").ToString(),
                        // If the user specified stereo audio, and therefore we turned off diarization,
                        // the speaker number is replaced by a channel number.
                        speakerNumber = this.userConfig.useStereoAudio ? phrase.GetProperty("channel").GetInt32() : phrase.GetProperty("speaker").GetInt32(),
                        offset = phrase.GetProperty("offsetInTicks").GetDouble()
                    };
                }
            );
            var recognizedPhrases_3 = this.userConfig.useStereoAudio
                // For stereo audio, the phrases are sorted by channel number, so to resort them by offset.
                ? recognizedPhrases_2.OrderBy(x => x.offset)
                : recognizedPhrases_2;
            // Discard the offset.
            return recognizedPhrases_3.Select(x => (x.phrase, x.speakerNumber)).ToArray();
        }

        private async Task DeleteTranscription(string transcriptionId)
        {
            var uri = new UriBuilder(Uri.UriSchemeHttps, this.userConfig.speechEndpoint);
            uri.Path = $"{speechTranscriptionPath}/{transcriptionId}";
            await RestHelper.SendDelete(uri.Uri.ToString(), this.userConfig.speechSubscriptionKey);
            return;
        }

        private JsonElement GetSentimentAnalysis(string[] phrases)
        {
            var uri = new UriBuilder(Uri.UriSchemeHttps, this.userConfig.languageEndpoint);
            uri.Path = sentimentAnalysisPath;
            uri.Query = sentimentAnalysisQuery;

            // Convert each transcription phrase to a "document" as expected by the sentiment analysis REST API.
            // Include a counter to use as a document ID.
            var documentsToSend = phrases.Select((phrase, id) =>
                {
                    return new
                    {
                        id = id,
                        language = this.userConfig.language,
                        text = phrase
                    };
                }
            );
            
            // We can only analyze sentiment for 10 documents per request.
            // We cannot use SelectMany here because the lambda returns a Task.
            var tasks = documentsToSend.Chunk(10).Select(async chunk =>
                {
                    var content_1 = new { kind = "SentimentAnalysis", analysisInput = new { documents = chunk }};
                    var content_2 = JsonSerializer.Serialize(content_1);
                    var response = await RestHelper.SendPost(uri.Uri.ToString(), content_2, this.userConfig.languageSubscriptionKey, new HttpStatusCode[] { HttpStatusCode.OK });
                    return response.content;
                }
            ).ToArray();
            Task.WaitAll(tasks);
            var documents = tasks.SelectMany(task => {
                var result = task.Result;
                using (JsonDocument document = JsonDocument.Parse(result))
                {
                    return document.RootElement.Clone().GetProperty("results").GetProperty("documents").EnumerateArray().ToArray();
                }
            }).ToArray();
            var document = new
            {
                kind = "SentimentAnalysisResults",
                results = new
                {
                    documents = documents
                }
            };
            using (JsonDocument document_2 = JsonDocument.Parse(JsonSerializer.Serialize(document)))
            {
                return document_2.RootElement.Clone();
            }
        }

        private string[] GetSentiments(JsonElement sentimentAnalysis)
        {
            return sentimentAnalysis
                .GetProperty("results")
                .GetProperty("documents").EnumerateArray().ToArray()
                .Select(document => new { id = Int32.Parse(document.GetProperty("id").ToString()), sentiment = document.GetProperty("sentiment").ToString() } ).ToArray()
                .OrderBy(x => x.id)
                .Select(x => x.sentiment)
                .ToArray();
        }

        private object[] TranscriptionPhrasesToConversationItems((string phrase, int speakerID)[] transcriptionPhrasesAndSpeakers)
        {
            // Include a counter to use as a document ID.
            return transcriptionPhrasesAndSpeakers.Select((phraseAndSpeaker, documentID) =>
                {
                    return new
                    {
                        id = documentID,
                        text = phraseAndSpeaker.phrase,
                        // The first person to speak is probably the agent.
                        role = 1 == phraseAndSpeaker.speakerID ? "Agent" : "Customer",
                        participantId = phraseAndSpeaker.speakerID
                    };
                }
            ).ToArray();
        }

        private async Task<string> RequestConversationAnalysis(object[] conversationItems)
        {
            var uri = new UriBuilder(Uri.UriSchemeHttps, this.userConfig.languageEndpoint);
            uri.Path = conversationAnalysisPath;
            uri.Query = conversationAnalysisQuery;

            var displayName = $"call_center_{DateTime.Now.ToString()}";

            var content = new
            {
                displayName = displayName,
                analysisInput = new
                {
                    conversations = new object[] {
                        new
                        {
                            id = "conversation1",
                            language = this.userConfig.language,
                            modality = "text",
                            conversationItems = conversationItems
                        }
                    }
                },
                tasks = new object[]
                {
                    new
                    {
                        taskName = "summary_1",
                        kind = "ConversationalSummarizationTask",
                        parameters = new
                        {
                            modelVersion = conversationSummaryModelVersion,
                            summaryAspects = new string[]
                            {
                                "Issue",
                                "Resolution"
                            }
                        }
                    },
                    new
                    {
                      kind = "ConversationalPIITask",
                      taskName = "PII_1"
                    }
                }
            };
            var response = await RestHelper.SendPost(uri.Uri.ToString(), JsonSerializer.Serialize(content), this.userConfig.languageSubscriptionKey, new HttpStatusCode[]{ HttpStatusCode.Accepted });
            return response.response.Headers.GetValues("operation-location").First();
        }

        private async Task<bool> GetConversationAnalysisStatus(string conversationAnalysisUrl)
        {
            var response = await RestHelper.SendGet(conversationAnalysisUrl, this.userConfig.languageSubscriptionKey, new HttpStatusCode[]{ HttpStatusCode.OK });
            using (JsonDocument document = JsonDocument.Parse(response.content))
            {
                if (0 == string.Compare("Failed", document.RootElement.Clone().GetProperty("status").ToString(), StringComparison.InvariantCultureIgnoreCase))
                {
                    throw new Exception($"Unable to analyze conversation. Response:{Environment.NewLine}{response.content}");
                }
                else
                {
                    return 0 == string.Compare("Succeeded", document.RootElement.Clone().GetProperty("status").ToString(), StringComparison.InvariantCultureIgnoreCase);
                }                
            }
        }

        private async Task WaitForConversationAnalysis(string conversationAnalysisUrl)
        {
            var done = false;
            while (!done)
            {
                Console.WriteLine($"Waiting {waitSeconds} seconds for conversation analysis to complete.");
                Thread.Sleep(waitSeconds * 1000);
                done = await GetConversationAnalysisStatus(conversationAnalysisUrl);
            }
            return;
        }

        private async Task<JsonElement> GetConversationAnalysis(string conversationAnalysisUrl)
        {
            var response = await RestHelper.SendGet(conversationAnalysisUrl, this.userConfig.languageSubscriptionKey, new HttpStatusCode[]{ HttpStatusCode.OK });
            using (JsonDocument document = JsonDocument.Parse(response.content))
            {
                return document.RootElement.Clone();
            }
        }
        
        private ConversationAnalysisResult GetConversationAnalysisResult(JsonElement conversationAnalysis)
        {
            var tasks = conversationAnalysis.GetProperty("tasks").GetProperty("items").EnumerateArray().ToArray();
            
            var summaryTask = tasks.First(task => 0 == String.Compare(task.GetProperty("taskName").ToString(), "summary_1", StringComparison.InvariantCultureIgnoreCase));
            var conversationSummary = summaryTask.GetProperty("results").GetProperty("conversations").EnumerateArray().ToArray()
                .First().GetProperty("summaries").EnumerateArray().ToArray()
                .Select(summary => (summary.GetProperty("aspect").ToString(), summary.GetProperty("text").ToString()))
                .ToArray();

            var PIITask = tasks.First(task => 0 == String.Compare(task.GetProperty("taskName").ToString(), "PII_1", StringComparison.InvariantCultureIgnoreCase));
            var conversationPIIAnalysis = PIITask.GetProperty("results").GetProperty("conversations").EnumerateArray().ToArray()
                .First().GetProperty("conversationItems").EnumerateArray().ToArray()
                .Select(document =>
                    {
                        return document.GetProperty("entities").EnumerateArray().ToArray()
                            .Select(entity => (entity.GetProperty("category").ToString(), entity.GetProperty("text").ToString())).ToArray();
                    })
                .ToArray();
            
            return new ConversationAnalysisResult(conversationSummary, conversationPIIAnalysis);
        }

        // Create a string that contains each transcription phrase, followed by sentiment, PII, and so on.
        private String GetResults(
            (string phrase, int speakerID)[] transcriptionPhrasesAndSpeakers,
            string[] transcriptionSentiments,
            ConversationAnalysisResult conversationAnalysis)
        {
            var result = new StringBuilder();
            for (var index = 0; index < transcriptionPhrasesAndSpeakers.Length; index++)
            {
                result.AppendLine($"Phrase: {transcriptionPhrasesAndSpeakers[index].phrase}");
                result.AppendLine($"Speaker: {transcriptionPhrasesAndSpeakers[index].speakerID}");
                if (index < transcriptionSentiments.Length)
                {
                    result.AppendLine($"Sentiment: {transcriptionSentiments[index]}");
                }
                if (index < conversationAnalysis.PIIAnalysis.Length)
                {
                    if (conversationAnalysis.PIIAnalysis[index].Length > 0)
                    {
                        var entities = conversationAnalysis.PIIAnalysis[index].Aggregate("",
                            (result, entity) => $"{result}    Category: {entity.category}. Text: {entity.text}.{Environment.NewLine}");
                        result.Append($"Recognized entities (PII):{Environment.NewLine}{entities}");
                    }
                    else
                    {
                        result.AppendLine($"Recognized entities (PII): none.");
                    }
                }
                result.AppendLine();
            }
            result.AppendLine(conversationAnalysis.summary.Aggregate($"Conversation summary:{Environment.NewLine}",
                (result, item) => $"{result}    Aspect: {item.aspect}. Summary: {item.summary}.{Environment.NewLine}"));
            return result.ToString();
        }

        private void PrintResults(JsonElement transcription, JsonElement sentimentAnalysis, JsonElement conversationAnalysis)
        {
            var results = new {
                transcription = transcription,
                sentimentAnalysis = sentimentAnalysis,
                conversationAnalysis = conversationAnalysis
            };
            Console.WriteLine(JsonSerializer.Serialize(results, new JsonSerializerOptions() { WriteIndented = true}));
        }

        // Note: To pass command-line arguments, run:
        // dotnet run -- [args]
        // For example:
        // dotnet run -- --help
        public async Task Run()
        {
            // How to use batch transcription:
            // https://github.com/MicrosoftDocs/azure-docs/blob/main/articles/cognitive-services/Speech-Service/batch-transcription.md
            var transcriptionId = await CreateTranscription();
            Console.WriteLine($"Transcription ID: {transcriptionId}");
            await WaitForTranscription(transcriptionId);
            var transcriptionFiles = await GetTranscriptionFiles(transcriptionId);
            var transcriptionUri = GetTranscriptionUri(transcriptionFiles);
            var transcription = await GetTranscription(transcriptionUri);
            var phrasesAndSpeakers = GetTranscriptionPhrasesAndSpeakers(transcription);
            var phrases = phrasesAndSpeakers.Select(phraseAndSpeaker => phraseAndSpeaker.phrase).ToArray();
            var sentimentAnalysis = GetSentimentAnalysis(phrases);
            var conversationItems = TranscriptionPhrasesToConversationItems(phrasesAndSpeakers);
            // NOTE: Conversation summary is currently in gated public preview. You can sign up here:
            // https://aka.ms/applyforconversationsummarization/
            var conversationAnalysisUrl = await RequestConversationAnalysis(conversationItems);
            await WaitForConversationAnalysis(conversationAnalysisUrl);
            var conversationAnalysis = await GetConversationAnalysis(conversationAnalysisUrl);
            if (this.userConfig.outputFilePath is string outputFilePathValue)
            {
                var sentiments = GetSentiments(sentimentAnalysis);
                var conversationAnalysisResult = GetConversationAnalysisResult(conversationAnalysis);
                File.WriteAllText(outputFilePathValue, GetResults(phrasesAndSpeakers, sentiments, conversationAnalysisResult));
            }
            PrintResults(transcription, sentimentAnalysis, conversationAnalysis);
            // Clean up resources.
            Console.WriteLine("Deleting transcription.");
            await DeleteTranscription(transcriptionId);
            return;
        }

        public async static Task Main(string[] args)
        {
            string usage = @"USAGE: dotnet run -- [...]

  HELP
    --help                          Show this help and stop.

  CONNECTION
    --speechKey KEY                 Your Azure Speech service subscription key. Required.
    --speechRegion REGION           Your Azure Speech service region. Required.
                                    Examples: westus, eastus
    --languageKey KEY               Your Azure Cognitive Language subscription key. Required.
    --languageEndpoint ENDPOINT     Your Azure Cognitive Language endpoint. Required.

  LANGUAGE
    --language LANGUAGE             The language to use for sentiment analysis and conversation analysis.
                                    This should be a two-letter ISO 639-1 code.
                                    Default: en
    --locale LOCALE                 The locale to use for batch transcription of audio.
                                    Default: en-US

  INPUT
    --input URL                     Input audio from URL. Required.
    --stereo                        Use stereo audio format.
                                    If this is not present, mono is assumed.
  OUTPUT
    --output FILE                   Output phrase list and conversation summary to text file.
";
            
            try
            {
                if (args.Contains("--help"))
                {
                    Console.WriteLine(usage);
                }
                else
                {
                    await (new Program { userConfig = new UserConfig(args, usage) }).Run();
                }
            }
            catch(Exception e)
            {
                Console.WriteLine(e.Message);
            }            
        }
    }
}
