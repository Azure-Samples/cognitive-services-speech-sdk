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

        // Replace this value with the endpoint for your Text Analytics subscription.
        static string textAnalyticsHost = "PASTE_YOUR_TEXT_ANALYTICS_ENDPOINT_HERE";
        // Replace this value with the subscription key for your Text Analytics subscription.
        static string textAnalyticsKey = "PASTE_YOUR_TEXT_ANALYTICS_SUBSCRIPTION_KEY_HERE";

        // Replace this value with the endpoint for your Cognitive Language subscription.
        static string languageHost = "PASTE_YOUR_COGNITIVE_LANGUAGE_ENDPOINT_HERE";
        // Replace this value with the subscription key for your Cognitive Language subscription.
        static string languageKey = "PASTE_YOUR_COGNITIVE_LANGUAGE_SUBSCRIPTION_KEY_HERE";

        // Replace this value with the URL of the audio file you want to transcribe.
        static string transcriptionAudioUri = "https://github.com/Azure-Samples/cognitive-services-speech-sdk/raw/v-jaswel-patch-3/scenarios/csharp/dotnetcore/call_center/call_center/sample_1.wav";

        // This should not change unless the Speech REST API changes.
        static string speechHost = $"{speechRegion}.api.cognitive.microsoft.com";

        // This should not change unless you switch to a new version of the Speech REST API.
        static string speechTranscriptionPath = "speechtotext/v3.0/transcriptions";

        // These should not change unless you switch to a new version of the Text Analytics REST API.
        static string detectLanguagePath = "text/analytics/v3.1/languages";
        static string entitiesRecognitionGeneralPath = "text/analytics/v3.1/entities/recognition/general";
        static string entitiesRecognitionPIIPath = "text/analytics/v3.1/entities/recognition/pii";
        static string keyPhrasesPath = "text/analytics/v3.1/keyPhrases";
        static string sentimentAnalysisPath = "text/analytics/v3.1/sentiment";

        // These should not change unless you switch to a new version of the Cognitive Language REST API.
        static string requestConversationSummaryPath = "/language/analyze-conversations/jobs";
        static string requestConversationSummaryQuery = "api-version=2022-05-15-preview";

        static string conversationLanguage = "en";

        // How long to wait while polling transcription status.
        static int waitSeconds = 5;

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
            var payload_1 = new { contentUrls = new string[]{transcriptionAudioUri}, locale = "en-US", displayName = displayName };
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

        // Return transcription content as a list of phrases.
        async static Task<string[]> GetTranscriptionPhrases(string transcriptionUri)
        {
            var response = await SendGet(transcriptionUri, "", new HttpStatusCode[]{ HttpStatusCode.OK });
            using (JsonDocument document = JsonDocument.Parse(response.Item2))
            {
                var recognizedPhrases = document.RootElement.GetProperty("recognizedPhrases").EnumerateArray().ToArray();
                return recognizedPhrases.Select(phrase =>
                    {
                        var best = phrase.GetProperty("nBest").EnumerateArray().ToArray()[0];
                        return best.GetProperty("display").ToString();
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

        // Detect languages for a list of transcription phrases.
        static async Task<string[]> GetTranscriptionLanguages(IEnumerable<string> transcriptionPhrases)
        {
            var uri = new UriBuilder(Uri.UriSchemeHttps, textAnalyticsHost);
            uri.Path = detectLanguagePath;

            // Convert each transcription phrase to a "document" as expected by the Text Analytics Detect Language REST API.
            // Include a counter to use as a document ID.
            // Detect Language API JSON request and response samples:
            // https://docs.microsoft.com/en-us/rest/api/cognitiveservices-textanalytics/3.1preview4/languages/languages
            var documents_1 = transcriptionPhrases.Select((phrase, id) => $@"
{{
    ""id"": ""{id}"",
    ""text"": ""{phrase}""
}}
            ");
            // Concatenate documents with ',' separator.
            var documents_2 = string.Join(',', documents_1.ToArray());
            var content = $@"{{ ""documents"": [{documents_2}] }}";
            var response = await SendPost(uri.Uri.ToString(), content, textAnalyticsKey, new HttpStatusCode[] { HttpStatusCode.OK });
            using (JsonDocument document = JsonDocument.Parse(response.Item2))
            {
                var documents = document.RootElement.GetProperty("documents").EnumerateArray().ToArray();
                // Convert Select results to arrays to prevent them being disposed with the JsonDocument.
                return documents.Select(document => document.GetProperty("detectedLanguage").GetProperty("iso6391Name").ToString()).ToArray();
            }
        }

        // Convert a list of transcription phrases to "document" JSON elements as expected by various Text Analytics REST APIs.
        static string TranscriptionPhrasesToDocuments(string[] transcriptionPhrases, string[] transcriptionLanguages, int maxNumberOfDocuments)
        {
            // Combine the list of transcription phrases with the corresponding list of transcription languages into a single
            // list of tuples. Convert each (phrase, language) tuple into a "document" as expected by various Text Analytics
            // REST APIs. For example, see the sample request body for the
            // Sentiment REST API:
            // https://docs.microsoft.com/en-us/rest/api/cognitiveservices-textanalytics/3.1preview4/sentiment/sentiment#examples
            // Include a counter to use as a document ID.
            var documents_1 = transcriptionPhrases.Zip(transcriptionLanguages).Take(maxNumberOfDocuments).Select((t, id) => $@"
{{
    ""id"": ""{id}"",
    ""language"": ""{t.Second}"",
    ""text"": ""{t.First}""
}}
            ");
            // Concatenate documents with ',' separator.
            var documents_2 = string.Join(',', documents_1.ToArray());
            return $@"{{ ""documents"": [{documents_2}] }}";
        }

        // Get sentiments for transcription phrases.
        async static Task<string[]> GetSentiments(string transcriptionDocuments)
        {
            var uri = new UriBuilder(Uri.UriSchemeHttps, textAnalyticsHost);
            uri.Path = sentimentAnalysisPath;

            // Sentiment JSON request and response samples:
            // https://docs.microsoft.com/en-us/rest/api/cognitiveservices-textanalytics/3.1preview4/sentiment/sentiment#examples
            var response = await SendPost(uri.Uri.ToString(), transcriptionDocuments, textAnalyticsKey, new HttpStatusCode[] { HttpStatusCode.OK });
            using (JsonDocument document = JsonDocument.Parse(response.Item2))
            {
                var documents = document.RootElement.GetProperty("documents").EnumerateArray().ToArray();
                // Convert Select results to arrays to prevent them being disposed with the JsonDocument.
                return documents.Select(document => document.GetProperty("sentiment").ToString()).ToArray();
            }
        }

        // Convert a list of transcription phrases to "conversationItems" JSON elements as expected by the Conversation Summary REST API.
        static string TranscriptionPhrasesToConversationItems(string[] transcriptionPhrases)
        {
            // Include a counter to use as a document ID.
            var documents_1 = transcriptionPhrases.Select((phrase, id) => {
                var agentOrCustomerRole = id % 2 == 0 ? "Agent" : "Customer";
                var agentOrCustomerParticipantId = id % 2 == 0 ? "Agent_1" : "Customer_1";
                return $@"
{{
    ""id"": ""{id}"",
    ""text"": ""{phrase}"",
    ""role"": ""{agentOrCustomerRole}"",
""participantId"": ""{agentOrCustomerParticipantId}""
}}
                ";
            });
            // Concatenate documents with ',' separator.
            return string.Join(',', documents_1.ToArray());
        }

        // Request summary of a conversation.
        async static Task<string> RequestConversationSummary(string conversationItems, string language) {
            var uri = new UriBuilder(Uri.UriSchemeHttps, languageHost);
            uri.Path = requestConversationSummaryPath;
            uri.Query = requestConversationSummaryQuery;

            var displayName = $"call_center_{DateTime.Now.ToString()}";

            // Conversation analysis API JSON request sample and schema:
            // https://westus2.dev.cognitive.microsoft.com/docs/services/Language-2022-05-15-preview/operations/AnalyzeConversation_SubmitJob
            var payload = $@"
{{
    ""displayName"": ""{displayName}"",
    ""analysisInput"": {{
        ""conversations"": [
            {{
                ""id"": ""conversation1"",
                ""language"": ""{language}"",
                ""modality"": ""text"",
                ""conversationItems"": [
                    {conversationItems}
                ]
            }}
        ]
    }},
    ""tasks"": [
        {{
            ""taskName"": ""analyze_1"",
            ""kind"": ""ConversationalSummarizationTask"",
            ""parameters"": {{
                ""modelVersion"": ""2022-05-15-preview"",
                ""summaryAspects"": [
                    ""Issue"",
                    ""Resolution""
                ]
            }}
        }}
    ]
}}
            ";
            var response = await SendPost(uri.Uri.ToString(), payload, languageKey, new HttpStatusCode[]{ HttpStatusCode.Accepted });
            return response.Item1.Headers.GetValues("operation-location").First();
        }

        // Request PII analysis of a conversation.
        async static Task<string> RequestConversationPIIAnalysis(string conversationItems, string language) {
            var uri = new UriBuilder(Uri.UriSchemeHttps, languageHost);
            uri.Path = requestConversationSummaryPath;
            uri.Query = requestConversationSummaryQuery;

            var displayName = $"call_center_{DateTime.Now.ToString()}";

            // Conversation analysis API JSON request sample and schema:
            // https://westus2.dev.cognitive.microsoft.com/docs/services/Language-2022-05-15-preview/operations/AnalyzeConversation_SubmitJob
            var payload = $@"
{{
    ""displayName"": ""{displayName}"",
    ""analysisInput"": {{
        ""conversations"": [
            {{
                ""id"": ""conversation1"",
                ""language"": ""{language}"",
                ""modality"": ""text"",
                ""conversationItems"": [
                    {conversationItems}
                ]
            }}
        ]
    }},
    ""tasks"": [
        {{
          ""kind"": ""ConversationalPIITask"",
          ""taskName"": ""PII_1""
        }}
    ]
}}
            ";
            var response = await SendPost(uri.Uri.ToString(), payload, languageKey, new HttpStatusCode[]{ HttpStatusCode.Accepted });
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
        async static Task WaitForConversationAnalysis(string conversationAnalysisUrl, string analysisType)
        {
            var done = false;
            while(!done) {
                Console.WriteLine($"Waiting {waitSeconds} seconds for conversation analysis ({analysisType}) to complete.");
                Thread.Sleep(waitSeconds * 1000);
                done = await GetConversationAnalysisStatus(conversationAnalysisUrl);
            }
            Console.WriteLine();
            return;
        }

        async static Task<(string, string)[]> GetConversationSummary(string conversationSummaryUrl) {
            var response = await SendGet(conversationSummaryUrl, languageKey, new HttpStatusCode[]{ HttpStatusCode.OK });
            using (JsonDocument document = JsonDocument.Parse(response.Item2))
            {
                var tasks = document.RootElement.GetProperty("tasks");
                var items = tasks.GetProperty("items").EnumerateArray().ToArray();
                var results = items.First().GetProperty("results");
                var conversations = results.GetProperty("conversations").EnumerateArray().ToArray();
                var summaries = conversations.First().GetProperty("summaries").EnumerateArray().ToArray();
                // Convert Select results to arrays to prevent them being disposed with the JsonDocument.
                return summaries.Select(summary => (summary.GetProperty("aspect").ToString(), summary.GetProperty("text").ToString())).ToArray();
            }
        }

        async static Task<(string, string)[][]> GetConversationPIIAnalysis(string conversationPIIAnalysisUrl) {
            var response = await SendGet(conversationPIIAnalysisUrl, languageKey, new HttpStatusCode[]{ HttpStatusCode.OK });
            using (JsonDocument document = JsonDocument.Parse(response.Item2))
            {
                var tasks = document.RootElement.GetProperty("tasks");
                var items = tasks.GetProperty("items").EnumerateArray().ToArray();
                var results = items.First().GetProperty("results");
                var conversations = results.GetProperty("conversations").EnumerateArray().ToArray();
                var conversationItems = conversations.First().GetProperty("conversationItems").EnumerateArray().ToArray();
                // Convert Select results to arrays to prevent them being disposed with the JsonDocument.
                return conversationItems.Select(document =>
                    {
                        var entities = document.GetProperty("entities").EnumerateArray().ToArray();
                        return entities.Select(entity => (entity.GetProperty("category").ToString(), entity.GetProperty("text").ToString())).ToArray();
                    }
                ).ToArray();
            }
        }

        // Print each transcription phrase, followed by its language, sentiment, and so on.
        static void PrintResults(
            string[] transcriptionPhrases,
            string[] transcriptionLanguages,
            string[] transcriptionSentiments,
            (string, string)[] conversationSummary,
            (string, string)[][] conversationPIIAnalysis)
        {
            for (var index = 0; index < transcriptionPhrases.Length; index++)
            {
                Console.WriteLine ($"Phrase: {transcriptionPhrases[index]}");
                if (index < transcriptionLanguages.Length)
                {
                    Console.WriteLine ($"Language: {transcriptionLanguages[index]}");
                }
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
            var phrases = await GetTranscriptionPhrases(transcriptionUrl);
            var languages = await GetTranscriptionLanguages(phrases);
            var sentiments = await GetSentiments(TranscriptionPhrasesToDocuments(phrases, languages, 10));
            var conversationItems = TranscriptionPhrasesToConversationItems(phrases);
            // NOTE: Conversation summary is currently in gated public preview. You can sign up here:
            // https://aka.ms/applyforconversationsummarization/
            var conversationSummaryUrl = await RequestConversationSummary(conversationItems, conversationLanguage);
            await WaitForConversationAnalysis(conversationSummaryUrl, "summary");
            var conversationSummary = await GetConversationSummary(conversationSummaryUrl);
            var conversationPIIAnalysisUrl = await RequestConversationPIIAnalysis(conversationItems, conversationLanguage);
            await WaitForConversationAnalysis(conversationPIIAnalysisUrl, "PII");
            var conversationPIIAnalysis = await GetConversationPIIAnalysis(conversationPIIAnalysisUrl);
            PrintResults(phrases, languages, sentiments, conversationSummary, conversationPIIAnalysis);
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
