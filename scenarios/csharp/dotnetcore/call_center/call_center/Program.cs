//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Http;
using System.Text;
using System.Text.Json;
using System.Threading;
using System.Threading.Tasks;

namespace Call_Center
{
    public class Program
    {
        // Replace this value with the region for your Speech subscription.
        static string speechRegion = "PASTE_YOUR_SPEECH_REGION_HERE";
        // Replace this value with the subscription key for your Speech subscription.
        static string speechKey = "PASTE_YOUR_SPEECH_SUBSCRIPTION_KEY_HERE";
        
        // Replace this value with the endpoint for your Cognitive Language subscription.
        static string languageHost = "PASTE_YOUR_COGNITIVE_LANGUAGE_ENDPOINT_HERE";
        // Replace this value with the subscription key for your Cognitive Language subscription.
        static string languageKey = "PASTE_YOUR_COGNITIVE_LANGUAGE_SUBSCRIPTION_KEY_HERE";

        // Replace this value with the URL of the audio file you want to transcribe.
        static string transcriptionAudioUri = "https://github.com/Azure-Samples/cognitive-services-speech-sdk/raw/v-jaswel-patch-3/scenarios/csharp/dotnetcore/call_center/call_center/sample_1.wav";

        // The language to use for sentiment analysis and conversation analysis.
        // This should be a two-letter ISO 639-1 code.
        static string language = "en";

        // How long to wait while polling transcription status.
        static int waitSeconds = 5;

        // This should not change unless the Speech REST API changes.
        static string speechHost = $"{speechRegion}.api.cognitive.microsoft.com";

        // This should not change unless you switch to a new version of the Speech REST API.
        static string speechTranscriptionPath = "speechtotext/v3.0/transcriptions";

        // These should not change unless you switch to a new version of the Cognitive Language REST API.
        static string sentimentAnalysisPath = "language/:analyze-text";
        static string sentimentAnalysisQuery = "api-version=2022-05-01";
        static string conversationAnalysisPath = "/language/analyze-conversations/jobs";
        static string conversationAnalysisQuery = "api-version=2022-05-15-preview";

        // Send GET request with authentication and return the response and response content.
        async static Task<(HttpResponseMessage, string)> SendGet(string uri, string key, HttpStatusCode[] expectedStatusCodes)
        {
            using (var client = new HttpClient())
            using (var request = new HttpRequestMessage())
            {
                request.Method = HttpMethod.Get;
                request.RequestUri = new Uri(uri);
                request.Headers.Add("Ocp-Apim-Subscription-Key", key);

                var response_1 = await client.SendAsync(request);
                if (response_1.Content == null)
                {
                    throw new Exception($"The response from {uri} contains no content.");
                }
                var response_2 = await response_1.Content.ReadAsStringAsync();
                if (!expectedStatusCodes.Contains(response_1.StatusCode))
                {
                    throw new Exception($"The response from {uri} has an unexpected status code: {response_1.StatusCode}");
                }
                
                return (response_1, response_2);
            }
        }

        // Send POST request with authentication and return the response and response content.
        async static Task<(HttpResponseMessage, string)> SendPost(string uri, string requestBody, string key, HttpStatusCode[] expectedStatusCodes)
        {
            using (var client = new HttpClient())
            using (var request = new HttpRequestMessage())
            {
                request.Method = HttpMethod.Post;
                request.RequestUri = new Uri(uri);
                request.Headers.Add("Ocp-Apim-Subscription-Key", key);

                if (!String.IsNullOrEmpty(requestBody))
                {
                    request.Content = new StringContent(requestBody, Encoding.UTF8, "application/json");
                }
                var response_1 = await client.SendAsync(request);
                if (response_1.Content == null)
                {
                    throw new Exception($"The response from {uri} contains no content.");
                }
                var response_2 = await response_1.Content.ReadAsStringAsync();
                if (!expectedStatusCodes.Contains(response_1.StatusCode))
                {
                    throw new Exception($"The response from {uri} has an unexpected status code: {response_1.StatusCode}. Response:{Environment.NewLine}{response_2}");
                }
                
                return (response_1, response_2);
            }
        }

        // Send DELETE request with authentication.
        async static Task SendDelete (string uri, string key)
        {
            using (var client = new HttpClient())
            using (var request = new HttpRequestMessage())
            {
                request.Method = HttpMethod.Delete;
                request.RequestUri = new Uri(uri);
                request.Headers.Add("Ocp-Apim-Subscription-Key", key);

                var response = await client.SendAsync(request);
                if (HttpStatusCode.NoContent != response.StatusCode)
                {
                    throw new Exception($"HTTP DELETE request returned unexpected status code: {response.StatusCode}. Full response:{Environment.NewLine}{response.ToString()}");
                }
                return;
            }
        }

        // Create a transcription of the specified audio.
        async static Task<string> CreateTranscription(string transcriptionAudioUri) {
            var uri = new UriBuilder(Uri.UriSchemeHttps, speechHost);
            uri.Path = speechTranscriptionPath;

            var displayName = $"call_center_{DateTime.Now.ToString()}";

            // Create Transcription API JSON request sample and schema:
            // https://westus.dev.cognitive.microsoft.com/docs/services/speech-to-text-api-v3-0/operations/CreateTranscription
            // Note: locale and displayName are required.
            var payload_1 = new { contentUrls = new string[]{transcriptionAudioUri}, properties = new { diarizationEnabled = true }, locale = "en-US", displayName = displayName };
            var payload_2 = System.Text.Json.JsonSerializer.Serialize(payload_1);
            var response = await SendPost(uri.Uri.ToString(), payload_2, speechKey, new HttpStatusCode[]{ HttpStatusCode.Created });
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
                    throw new Exception ($"Unable to parse response from Create Transcription API:{Environment.NewLine}{response.Item2}");
                }
                return transcriptionId;
            }
        }

        // Return true if the transcription has status "Succeeded".
        async static Task<bool> GetTranscriptionStatus(string transcriptionId)
        {
            var uri = new UriBuilder(Uri.UriSchemeHttps, speechHost);
            uri.Path = $"{speechTranscriptionPath}/{transcriptionId}";
            var response = await SendGet(uri.Uri.ToString(), speechKey, new HttpStatusCode[]{ HttpStatusCode.OK });
            using (JsonDocument document = JsonDocument.Parse(response.Item2))
            {
                var status = document.RootElement.GetProperty("status").ToString();
                return 0 == string.Compare("Succeeded", status, StringComparison.InvariantCultureIgnoreCase);
            }
        }

        // Poll the transcription status until it has status "Succeeded".
        async static Task WaitForTranscription(string transcriptionId)
        {
            var done = false;
            while(!done) {
                Console.WriteLine($"Waiting {waitSeconds} seconds for transcription to complete.");
                Thread.Sleep(waitSeconds * 1000);
                done = await GetTranscriptionStatus(transcriptionId);
            }
            Console.WriteLine();
            return;
        }

        async static Task<string> GetTranscriptionUri(string transcriptionId)
        {
            var uri = new UriBuilder(Uri.UriSchemeHttps, speechHost);
            uri.Path = $"{speechTranscriptionPath}/{transcriptionId}/files";
            var response = await SendGet(uri.Uri.ToString(), speechKey, new HttpStatusCode[]{ HttpStatusCode.OK });
            using (JsonDocument document = JsonDocument.Parse(response.Item2))
            {
                // Get Transcription Files JSON response sample and schema:
                // https://westus.dev.cognitive.microsoft.com/docs/services/speech-to-text-api-v3-0/operations/GetTranscriptionFiles
                string? contentUri = null;
                foreach (JsonElement value in document.RootElement.GetProperty("values").EnumerateArray())
                {
                    if ("Transcription" == value.GetProperty("kind").ToString())
                    {
                        contentUri = value.GetProperty("links").GetProperty("contentUrl").ToString();
                        break;
                    }
                }
                if (null == contentUri)
                {
                    throw new Exception ($"Unable to parse response from Get Transcription Files API:{Environment.NewLine}{response.Item2}");
                }
                return contentUri.ToString();
            }
        }

        // Return transcription content as a list of phrases and speaker numbers.
        async static Task<(string, int)[]> GetTranscriptionPhrasesAndSpeakers(string transcriptionUri)
        {
            var response = await SendGet(transcriptionUri, "", new HttpStatusCode[]{ HttpStatusCode.OK });
            using (JsonDocument document = JsonDocument.Parse(response.Item2))
            {
                var recognizedPhrases = document.RootElement.GetProperty("recognizedPhrases").EnumerateArray().ToArray();
                return recognizedPhrases.Select(phrase =>
                    {
                        var best = phrase.GetProperty("nBest").EnumerateArray().ToArray()[0];
                        return (best.GetProperty("display").ToString(), phrase.GetProperty("speaker").GetInt32());
                    }
                // Convert Select results to arrays to prevent them being disposed with the JsonDocument.
                ).ToArray();
            }
        }

        async static void DeleteTranscription(string transcriptionId)
        {
            var uri = new UriBuilder(Uri.UriSchemeHttps, speechHost);
            uri.Path = $"{speechTranscriptionPath}/{transcriptionId}";
            await SendDelete(uri.Uri.ToString(), speechKey);
            return;
        }

        // Get sentiments for transcription phrases.
        static string[] GetSentiments(string[] phrases)
        {
            var uri = new UriBuilder(Uri.UriSchemeHttps, languageHost);
            uri.Path = sentimentAnalysisPath;
            uri.Query = sentimentAnalysisQuery;

            // Convert each transcription phrase to a "document" as expected by the sentiment analysis REST API.
            // Include a counter to use as a document ID.
            var documentsToSend = phrases.Select((phrase, id) =>
                {
                    return new {
                        id = id,
                        language = language,
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
                    var response = await SendPost(uri.Uri.ToString(), content_2, languageKey, new HttpStatusCode[] { HttpStatusCode.OK });
                    using (JsonDocument document = JsonDocument.Parse(response.Item2))
                    {
                        var documents = document.RootElement.GetProperty("results").GetProperty("documents").EnumerateArray().ToArray();
                        // Convert Select results to arrays to prevent them being disposed with the JsonDocument.
                        return documents.Select(document => (Int32.Parse(document.GetProperty("id").ToString()), document.GetProperty("sentiment").ToString())).ToArray();
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

        // Convert a list of transcription phrases to "conversationItems" JSON elements as expected by the conversation analysis REST API.
        static object[] TranscriptionPhrasesToConversationItems((string, int)[] transcriptionPhrasesAndSpeakers)
        {
            // Include a counter to use as a document ID.
            return transcriptionPhrasesAndSpeakers.Select((phraseAndSpeaker, id) =>
                {
                    // The first person to speak is probably the agent.
                    var agentOrCustomerRole = 1 == phraseAndSpeaker.Item2 ? "Agent" : "Customer";
                    var agentOrCustomerParticipantId = phraseAndSpeaker.Item2;
                    return new {
                        id = id,
                        text = phraseAndSpeaker.Item1,
                        role = agentOrCustomerRole,
                        participantId = agentOrCustomerParticipantId
                    };
                }
            ).ToArray();
        }

        // Request the analysis of a conversation.
        async static Task<string> RequestConversationAnalysis(object[] conversationItems) {
            var uri = new UriBuilder(Uri.UriSchemeHttps, languageHost);
            uri.Path = conversationAnalysisPath;
            uri.Query = conversationAnalysisQuery;

            var displayName = $"call_center_{DateTime.Now.ToString()}";

            var content_1 = new {
                displayName = displayName,
                analysisInput = new {
                    conversations = new object[] {
                        new {
                            id = "conversation1",
                            language = language,
                            modality = "text",
                            conversationItems = conversationItems
                        }
                    }
                },
                tasks = new object[] {
                    new {
                        taskName = "summary_1",
                        kind = "ConversationalSummarizationTask",
                        parameters = new {
                            modelVersion = "2022-05-15-preview",
                            summaryAspects = new string[] {
                                "Issue",
                                "Resolution"
                            }
                        }
                    },
                    new {
                      kind = "ConversationalPIITask",
                      taskName = "PII_1"
                    }
                }
            };
            var content_2 = System.Text.Json.JsonSerializer.Serialize(content_1);
            var response = await SendPost(uri.Uri.ToString(), content_2, languageKey, new HttpStatusCode[]{ HttpStatusCode.Accepted });
            return response.Item1.Headers.GetValues("operation-location").First();
        }

        // Return true if the conversation analysis has status "Succeeded".
        async static Task<bool> GetConversationAnalysisStatus(string conversationAnalysisUrl)
        {
            var response = await SendGet(conversationAnalysisUrl, languageKey, new HttpStatusCode[]{ HttpStatusCode.OK });
            using (JsonDocument document = JsonDocument.Parse(response.Item2))
            {
                var status = document.RootElement.GetProperty("status").ToString();
                return 0 == string.Compare("succeeded", status, StringComparison.InvariantCultureIgnoreCase);
            }
        }

        // Poll the conversation analysis status until it has status "Succeeded".
        async static Task WaitForConversationAnalysis(string conversationAnalysisUrl)
        {
            var done = false;
            while(!done) {
                Console.WriteLine($"Waiting {waitSeconds} seconds for conversation analysis to complete.");
                Thread.Sleep(waitSeconds * 1000);
                done = await GetConversationAnalysisStatus(conversationAnalysisUrl);
            }
            Console.WriteLine();
            return;
        }

        async static Task<((string, string)[], (string, string)[][])> GetConversationAnalysis(string conversationAnalysisUrl) {
            var response = await SendGet(conversationAnalysisUrl, languageKey, new HttpStatusCode[]{ HttpStatusCode.OK });
            using (JsonDocument document = JsonDocument.Parse(response.Item2))
            {
                var tasks = document.RootElement.GetProperty("tasks").GetProperty("items").EnumerateArray().ToArray();
                
                var summaryTask = tasks.First(task => 0 == String.Compare(task.GetProperty("taskName").ToString(), "summary_1", StringComparison.InvariantCultureIgnoreCase));
                var summaryResults = summaryTask.GetProperty("results");
                var summaryConversations = summaryResults.GetProperty("conversations").EnumerateArray().ToArray();
                var summaries = summaryConversations.First().GetProperty("summaries").EnumerateArray().ToArray();
                // Convert Select results to arrays to prevent them being disposed with the JsonDocument.
                var conversationSummary = summaries.Select(summary => (summary.GetProperty("aspect").ToString(), summary.GetProperty("text").ToString())).ToArray();

                var PIITask = tasks.First(task => 0 == String.Compare(task.GetProperty("taskName").ToString(), "PII_1", StringComparison.InvariantCultureIgnoreCase));
                var PIIResults = PIITask.GetProperty("results");
                var PIIConversations = PIIResults.GetProperty("conversations").EnumerateArray().ToArray();
                var conversationItems = PIIConversations.First().GetProperty("conversationItems").EnumerateArray().ToArray();
                // Convert Select results to arrays to prevent them being disposed with the JsonDocument.
                var conversationPIIAnalysis = conversationItems.Select(document =>
                    {
                        var entities = document.GetProperty("entities").EnumerateArray().ToArray();
                        return entities.Select(entity => (entity.GetProperty("category").ToString(), entity.GetProperty("text").ToString())).ToArray();
                    }
                ).ToArray();
                
                return (conversationSummary, conversationPIIAnalysis);
            }
        }

        // Print each transcription phrase, followed by its language, sentiment, and so on.
        static void PrintResults(
            (string,int)[] transcriptionPhrasesAndSpeakers,
            string[] transcriptionSentiments,
            (string, string)[] conversationSummary,
            (string, string)[][] conversationPIIAnalysis)
        {
            for (var index = 0; index < transcriptionPhrasesAndSpeakers.Length; index++)
            {
                Console.WriteLine ($"Phrase: {transcriptionPhrasesAndSpeakers[index].Item1}");
                Console.WriteLine ($"Speaker: {transcriptionPhrasesAndSpeakers[index].Item2}");
                if (index < transcriptionSentiments.Length)
                {
                    Console.WriteLine ($"Sentiment: {transcriptionSentiments[index]}");
                }
                if (index < conversationPIIAnalysis.Length)
                {
                    if (conversationPIIAnalysis[index].Length > 0)
                    {
                        var entities = conversationPIIAnalysis[index].Aggregate("", (result, entity) => $"{result}    Category: {entity.Item1}. Text: {entity.Item2}.{Environment.NewLine}");
                        Console.Write ($"Recognized entities (PII):{Environment.NewLine}{entities}");
                    }
                    else
                    {
                        Console.WriteLine ($"Recognized entities (PII): none.");
                    }
                }
                Console.WriteLine();
            }
            Console.WriteLine(conversationSummary.Aggregate($"Conversation summary:{Environment.NewLine}", (result, item) => $"{result}    Aspect: {item.Item1}. Summary: {item.Item2}.{Environment.NewLine}"));
            return;
        }

        public static async Task Run()
        {
            // How to use batch transcription:
            // https://github.com/MicrosoftDocs/azure-docs/blob/main/articles/cognitive-services/Speech-Service/batch-transcription.md
            var transcriptionId = await CreateTranscription(transcriptionAudioUri);
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
            DeleteTranscription(transcriptionId);
            return;
        }

        public static void Main(string[] args)
        {
            Run().Wait();
            Console.WriteLine("Press any key to exit.");
            Console.ReadKey();
            return;
        }
    }
}
