//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Text;
using System.Text.Json;
using System.Threading;
using System.Threading.Tasks;

namespace CallCenter
{
    public class Program
    {
        private UserConfig userConfig;
        
        // How long to wait while polling batch transcription and conversation analysis status.
        private const int waitSeconds = 5;

        // This should not change unless you switch to a new version of the Speech REST API.
        private const string speechTranscriptionPath = "speechtotext/v3.0/transcriptions";

        // These should not change unless you switch to a new version of the Cognitive Language REST API.
        private const string sentimentAnalysisPath = "language/:analyze-text";
        private const string sentimentAnalysisQuery = "api-version=2022-05-01";
        private const string conversationAnalysisPath = "/language/analyze-conversations/jobs";
        private const string conversationAnalysisQuery = "api-version=2022-05-15-preview";
        private const string conversationSummaryModelVersion = "2022-05-15-preview";

        private async Task<string> CreateTranscription(string transcriptionAudioUri)
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
                    contentUrls = new string[] { transcriptionAudioUri },
                    properties = new { diarizationEnabled = !this.userConfig.useStereoAudio },
                    locale = this.userConfig.locale,
                    displayName = $"call_center_{DateTime.Now.ToString()}"
                };
            var response = await RestHelper.SendPost(uri.Uri.ToString(), System.Text.Json.JsonSerializer.Serialize(content), this.userConfig.speechSubscriptionKey, new HttpStatusCode[]{ HttpStatusCode.Created });
            using (JsonDocument document = JsonDocument.Parse(response.Item2))
            {
                // Create Transcription API JSON response sample and schema:
                // https://westus.dev.cognitive.microsoft.com/docs/services/speech-to-text-api-v3-0/operations/CreateTranscription
                var transcriptionUri = document.RootElement.GetProperty("self").ToString();
                // The transcription ID is at the end of the transcription URI.
                var transcriptionId = transcriptionUri.Split("/").Last();
                // Verify the transcription ID is a valid GUID.
                Guid guid;
                if (!Guid.TryParse(transcriptionId, out guid))
                {
                    throw new Exception($"Unable to parse response from Create Transcription API:{Environment.NewLine}{response.Item2}");
                }
                return transcriptionId;
            }
        }

        private async Task<bool> GetTranscriptionStatus(string transcriptionId)
        {
            var uri = new UriBuilder(Uri.UriSchemeHttps, this.userConfig.speechEndpoint);
            uri.Path = $"{speechTranscriptionPath}/{transcriptionId}";
            var response = await RestHelper.SendGet(uri.Uri.ToString(), this.userConfig.speechSubscriptionKey, new HttpStatusCode[]{ HttpStatusCode.OK });
            using (JsonDocument document = JsonDocument.Parse(response.Item2))
            {
                return 0 == string.Compare("Succeeded", document.RootElement.GetProperty("status").ToString(), StringComparison.InvariantCultureIgnoreCase);
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
            Console.WriteLine();
            return;
        }

        private async Task<string> GetTranscriptionUri(string transcriptionId)
        {
            var uri = new UriBuilder(Uri.UriSchemeHttps, this.userConfig.speechEndpoint);
            uri.Path = $"{speechTranscriptionPath}/{transcriptionId}/files";
            var response = await RestHelper.SendGet(uri.Uri.ToString(), this.userConfig.speechSubscriptionKey, new HttpStatusCode[]{ HttpStatusCode.OK });
            using (JsonDocument document = JsonDocument.Parse(response.Item2))
            {
                // Get Transcription Files JSON response sample and schema:
                // https://westus.dev.cognitive.microsoft.com/docs/services/speech-to-text-api-v3-0/operations/GetTranscriptionFiles
                // NOTE: Convert JsonDocument GetProperty results to strings or arrays, as appropriate,
                // to keep them from being disposed with the JsonDocument.
                return document.RootElement.GetProperty("values").EnumerateArray().ToArray()
                    .First(value => 0 == String.Compare("Transcription", value.GetProperty("kind").ToString(), StringComparison.InvariantCultureIgnoreCase))
                    .GetProperty("links").GetProperty("contentUrl").ToString();
            }
        }

        private async Task<(string, int)[]> GetTranscriptionPhrasesAndSpeakers(string transcriptionUri)
        {
            var response = await RestHelper.SendGet(transcriptionUri, "", new HttpStatusCode[]{ HttpStatusCode.OK });
            using (JsonDocument document = JsonDocument.Parse(response.Item2))
            {
                var recognizedPhrases = document.RootElement.GetProperty("recognizedPhrases").EnumerateArray().ToArray();
                return recognizedPhrases.Select(phrase =>
                    {
                        var best = phrase.GetProperty("nBest").EnumerateArray().ToArray().First();
                        return (best.GetProperty("display").ToString(), phrase.GetProperty("speaker").GetInt32());
                    }
                ).ToArray();
            }
        }

        private async Task DeleteTranscription(string transcriptionId)
        {
            var uri = new UriBuilder(Uri.UriSchemeHttps, this.userConfig.speechEndpoint);
            uri.Path = $"{speechTranscriptionPath}/{transcriptionId}";
            await RestHelper.SendDelete(uri.Uri.ToString(), this.userConfig.speechSubscriptionKey);
            return;
        }

        private string[] GetSentiments(string[] phrases)
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
                    var content_2 = System.Text.Json.JsonSerializer.Serialize(content_1);
                    var response = await RestHelper.SendPost(uri.Uri.ToString(), content_2, this.userConfig.languageSubscriptionKey, new HttpStatusCode[] { HttpStatusCode.OK });
                    using (JsonDocument document = JsonDocument.Parse(response.Item2))
                    {
                        return document.RootElement.GetProperty("results").GetProperty("documents").EnumerateArray().ToArray()
                            .Select(document => (Int32.Parse(document.GetProperty("id").ToString()), document.GetProperty("sentiment").ToString()))
                            .ToArray();
                    }
                }
            ).ToArray();
            Task.WaitAll(tasks);
            return tasks
                .Select(task => task.Result)
                .Aggregate(new (int, string)[] {}, (acc, sublist) => acc.Concat(sublist).ToArray())
                .OrderBy(x => x.Item1)
                .Select(x => x.Item2)
                .ToArray();
        }

        private object[] TranscriptionPhrasesToConversationItems((string, int)[] transcriptionPhrasesAndSpeakers)
        {
            // Include a counter to use as a document ID.
            return transcriptionPhrasesAndSpeakers.Select((phraseAndSpeaker, id) =>
                {
                    // The first person to speak is probably the agent.
                    var agentOrCustomerRole = 1 == phraseAndSpeaker.Item2 ? "Agent" : "Customer";
                    var agentOrCustomerParticipantId = phraseAndSpeaker.Item2;
                    return new
                    {
                        id = id,
                        text = phraseAndSpeaker.Item1,
                        role = agentOrCustomerRole,
                        participantId = agentOrCustomerParticipantId
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

            var content_1 = new
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
            var content_2 = System.Text.Json.JsonSerializer.Serialize(content_1);
            var response = await RestHelper.SendPost(uri.Uri.ToString(), content_2, this.userConfig.languageSubscriptionKey, new HttpStatusCode[]{ HttpStatusCode.Accepted });
            return response.Item1.Headers.GetValues("operation-location").First();
        }

        private async Task<bool> GetConversationAnalysisStatus(string conversationAnalysisUrl)
        {
            var response = await RestHelper.SendGet(conversationAnalysisUrl, this.userConfig.languageSubscriptionKey, new HttpStatusCode[]{ HttpStatusCode.OK });
            using (JsonDocument document = JsonDocument.Parse(response.Item2))
            {
                return 0 == string.Compare("succeeded", document.RootElement.GetProperty("status").ToString(), StringComparison.InvariantCultureIgnoreCase);
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
            Console.WriteLine();
            return;
        }

        private async Task<((string, string)[], (string, string)[][])> GetConversationAnalysis(string conversationAnalysisUrl)
        {
            var response = await RestHelper.SendGet(conversationAnalysisUrl, this.userConfig.languageSubscriptionKey, new HttpStatusCode[]{ HttpStatusCode.OK });
            using (JsonDocument document = JsonDocument.Parse(response.Item2))
            {
                var tasks = document.RootElement.GetProperty("tasks").GetProperty("items").EnumerateArray().ToArray();
                
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
                
                return (conversationSummary, conversationPIIAnalysis);
            }
        }

        // Print each transcription phrase, followed by its language, sentiment, and so on.
        private void PrintResults(
            (string,int)[] transcriptionPhrasesAndSpeakers,
            string[] transcriptionSentiments,
            (string, string)[] conversationSummary,
            (string, string)[][] conversationPIIAnalysis)
        {
            for (var index = 0; index < transcriptionPhrasesAndSpeakers.Length; index++)
            {
                Console.WriteLine($"Phrase: {transcriptionPhrasesAndSpeakers[index].Item1}");
                Console.WriteLine($"Speaker: {transcriptionPhrasesAndSpeakers[index].Item2}");
                if (index < transcriptionSentiments.Length)
                {
                    Console.WriteLine($"Sentiment: {transcriptionSentiments[index]}");
                }
                if (index < conversationPIIAnalysis.Length)
                {
                    if (conversationPIIAnalysis[index].Length > 0)
                    {
                        var entities = conversationPIIAnalysis[index].Aggregate("", (result, entity) => $"{result}    Category: {entity.Item1}. Text: {entity.Item2}.{Environment.NewLine}");
                        Console.Write($"Recognized entities (PII):{Environment.NewLine}{entities}");
                    }
                    else
                    {
                        Console.WriteLine($"Recognized entities (PII): none.");
                    }
                }
                Console.WriteLine();
            }
            Console.WriteLine(conversationSummary.Aggregate($"Conversation summary:{Environment.NewLine}", (result, item) => $"{result}    Aspect: {item.Item1}. Summary: {item.Item2}.{Environment.NewLine}"));
            return;
        }

        //
        // Constructor
        //
        public Program(string[] args, string usage)
        {
            this.userConfig = new UserConfig(args, usage);
        }

        // Note: To pass command-line arguments, run:
        // dotnet run -- [args]
        // For example:
        // dotnet run -- --help
        public async Task Run()
        {
            // How to use batch transcription:
            // https://github.com/MicrosoftDocs/azure-docs/blob/main/articles/cognitive-services/Speech-Service/batch-transcription.md
            var transcriptionId = await CreateTranscription(this.userConfig.inputAudioURL);
            await WaitForTranscription(transcriptionId);
            var transcriptionUrl = await GetTranscriptionUri(transcriptionId);
            var phrasesAndSpeakers = await GetTranscriptionPhrasesAndSpeakers(transcriptionUrl);
            var phrases = phrasesAndSpeakers.Select(phraseAndSpeaker => phraseAndSpeaker.Item1).ToArray();
            var sentiments = GetSentiments(phrases);
            var conversationItems = TranscriptionPhrasesToConversationItems(phrasesAndSpeakers);
            // NOTE: Conversation summary is currently in gated public preview. You can sign up here:
            // https://aka.ms/applyforconversationsummarization/
            var conversationAnalysisUrl = await RequestConversationAnalysis(conversationItems);
            await WaitForConversationAnalysis(conversationAnalysisUrl);
            var (conversationSummary, conversationPIIAnalysis) = await GetConversationAnalysis(conversationAnalysisUrl);
            PrintResults(phrasesAndSpeakers, sentiments, conversationSummary, conversationPIIAnalysis);
            // Clean up resources.
            Console.WriteLine("Deleting transcription.");
            await DeleteTranscription(transcriptionId);
            return;
        }

        public static void Main(string[] args)
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
    --stereo FORMAT                 Use stereo audio format.
                                    If this is not present, mono is assumed.
";
            
            try
            {
                if (args.Contains("--help"))
                {
                    Console.WriteLine(usage);
                }
                else
                {
                    Program program = new Program(args, usage);
                    program.Run().Wait();
                }
            }
            catch(Exception e)
            {
                Console.WriteLine(e.Message);
            }            
        }
    }
}
