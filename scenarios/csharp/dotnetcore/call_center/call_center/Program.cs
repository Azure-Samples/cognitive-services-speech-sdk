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

        // Replace this value with the URL of the audio file you want to transcribe.
        static string transcriptionUri = "https://github.com/Azure-Samples/cognitive-services-speech-sdk/raw/master/sampledata/audiofiles/wikipediaOcelot.wav";

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
                var response_2 = await response_1.Content.ReadAsStringAsync();
                if (!expectedStatusCodes.Contains(response_1.StatusCode) || response_1.Content == null)
                {
                    throw new Exception(response_2);
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
                var response_2 = await response_1.Content.ReadAsStringAsync();
                if (!expectedStatusCodes.Contains(response_1.StatusCode) || response_1.Content == null)
                {
                    throw new Exception(response_2);
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
                request.Method = HttpMethod.Get;
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
        async static Task<string> CreateTranscription(string transcriptionUri) {
            var uri = new UriBuilder(Uri.UriSchemeHttps, speechHost);
            uri.Path = speechTranscriptionPath;

            var displayName = $"call_center_{DateTime.Now.ToString()}";

            // Create Transcription API JSON request sample and schema:
            // https://westus.dev.cognitive.microsoft.com/docs/services/speech-to-text-api-v3-0/operations/CreateTranscription
            // Note: locale and displayName are required.
            var payload_1 = new { contentUrls = new string[]{transcriptionUri}, locale = "en-US", displayName = displayName };
            var payload_2 = System.Text.Json.JsonSerializer.Serialize(payload_1);
            var response = await SendPost(uri.Uri.ToString(), payload_2, speechKey, new HttpStatusCode[]{ HttpStatusCode.Created });
            using (JsonDocument document = JsonDocument.Parse(response.Item2))
            {
                // Create Transcription API JSON response sample and schema:
                // https://westus.dev.cognitive.microsoft.com/docs/services/speech-to-text-api-v3-0/operations/CreateTranscription
                var transcription_uri = document.RootElement.GetProperty("self").ToString();
                // The transcription ID is at the end of the transcription URI.
                var transcription_id = transcription_uri.Split("/").Last();
                // Verify the transcription ID is a valid GUID.
                Guid guid;
                if (!Guid.TryParse(transcription_id, out guid))
                {
                    throw new Exception ($"Unable to parse response from Create Transcription API:{Environment.NewLine}{response.Item2}");
                }
                return transcription_id;
            }
        }

        // Return true if the transcription has status "Succeeded".
        async static Task<bool> GetTranscriptionStatus(string transcription_id)
        {
            var uri = new UriBuilder(Uri.UriSchemeHttps, speechHost);
            uri.Path = $"{speechTranscriptionPath}/{transcription_id}";
            var response = await SendGet(uri.Uri.ToString(), speechKey, new HttpStatusCode[]{ HttpStatusCode.OK });
            using (JsonDocument document = JsonDocument.Parse(response.Item2))
            {
                var status = document.RootElement.GetProperty("status").ToString();
                return 0 == string.Compare("Succeeded", status);
            }
        }

        // Poll the transcription status until it has status "Succeeded".
        async static void WaitForTranscription(string transcription_id)
        {
            var done = false;
            while(!done) {
                Console.WriteLine($"Waiting {waitSeconds} seconds for transcription to complete.");
                Thread.Sleep(waitSeconds * 1000);
                done = await GetTranscriptionStatus(transcription_id);
            }
            return;
        }

        async static Task<string> GetTranscriptionUri(string transcription_id)
        {
            var uri = new UriBuilder(Uri.UriSchemeHttps, speechHost);
            uri.Path = $"{speechTranscriptionPath}/{transcription_id}/files";
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
        async static Task<IEnumerable<string>> GetTranscriptionPhrases(string transcription_uri)
        {
            var response = await SendGet(transcription_uri, "", new HttpStatusCode[]{ HttpStatusCode.OK });
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

        async static void DeleteTranscription(string transcription_id)
        {
            var uri = new UriBuilder(Uri.UriSchemeHttps, speechHost);
            uri.Path = $"{speechTranscriptionPath}/{transcription_id}";
            await SendDelete(uri.Uri.ToString(), speechKey);
            return;
        }

        // Detect languages for a list of transcription phrases.
        static async Task<IEnumerable<string>> GetTranscriptionLanguages(IEnumerable<string> transcription_phrases)
        {
            var uri = new UriBuilder(Uri.UriSchemeHttps, speechHost);
            uri.Path = detectLanguagePath;

            // Convert each transcription phrase to a "document" as expected by the Text Analytics Detect Language REST API.
            // Include a counter to use as a document ID.
            // Detect Language API JSON request and response samples:
            // https://docs.microsoft.com/en-us/rest/api/cognitiveservices-textanalytics/3.1preview4/languages/languages
            var documents_1 = transcription_phrases.Select((phrase, id) => $@"
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
        static string TranscriptionPhrasesToDocuments(IEnumerable<string> transcription_phrases, IEnumerable<string> transcription_languages)
        {
            // Combine the list of transcription phrases with the corresponding list of transcription languages into a single
            // list of tuples. Convert each (phrase, language) tuple into a "document" as expected by various Text Analytics
            // REST APIs. For example, see the sample request body for the
            // Sentiment REST API:
            // https://docs.microsoft.com/en-us/rest/api/cognitiveservices-textanalytics/3.1preview4/sentiment/sentiment#examples
            // Include a counter to use as a document ID.
            var documents_1 = transcription_phrases.Zip(transcription_languages).Select((t, id) => $@"
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
        async static Task<IEnumerable<string>> GetSentiments(string transcription_documents)
        {
            var uri = new UriBuilder(Uri.UriSchemeHttps, textAnalyticsHost);
            uri.Path = sentimentAnalysisPath;

            // Sentiment JSON request and response samples:
            // https://docs.microsoft.com/en-us/rest/api/cognitiveservices-textanalytics/3.1preview4/sentiment/sentiment#examples
            var response = await SendPost(uri.Uri.ToString(), transcription_documents, textAnalyticsKey, new HttpStatusCode[] { HttpStatusCode.OK });
            using (JsonDocument document = JsonDocument.Parse(response.Item2))
            {
                var documents = document.RootElement.GetProperty("documents").EnumerateArray().ToArray();
                // Convert Select results to arrays to prevent them being disposed with the JsonDocument.
                return documents.Select(document => document.GetProperty("sentiment").ToString()).ToArray();
            }
        }

        // Get key phrases for transcription phrases.
        async static Task<IEnumerable<IEnumerable<string>>> GetKeyPhrases(string transcription_documents)
        {
            var uri = new UriBuilder(Uri.UriSchemeHttps, textAnalyticsHost);
            uri.Path = keyPhrasesPath;

            // Key phrases JSON request and response samples:
            // https://docs.microsoft.com/en-us/rest/api/cognitiveservices-textanalytics/3.1preview4/key-phrases/key-phrases#examples
            var response = await SendPost(uri.Uri.ToString(), transcription_documents, textAnalyticsKey, new HttpStatusCode[] { HttpStatusCode.OK });
            using (JsonDocument document = JsonDocument.Parse(response.Item2))
            {
                var documents = document.RootElement.GetProperty("documents").EnumerateArray().ToArray();
                // Convert Select results to array to prevent them being disposed with the JsonDocument.
                return documents.Select(document => document.GetProperty("keyPhrases").EnumerateArray().ToArray().Select(phrase => phrase.ToString()).ToArray()).ToArray();
            }
        }

        // Get recognized entities (general) for transcription phrases.
        async static Task<IEnumerable<IEnumerable<(string, string)>>> GetRecognizedEntitiesGeneral(string transcription_documents)
        {
            var uri = new UriBuilder(Uri.UriSchemeHttps, textAnalyticsHost);
            uri.Path = entitiesRecognitionGeneralPath;

            // Entities recognition JSON request and response samples:
            // https://docs.microsoft.com/en-us/rest/api/cognitiveservices-textanalytics/3.1preview4/entities-recognition-general/entities-recognition-general#examples
            var response = await SendPost(uri.Uri.ToString(), transcription_documents, textAnalyticsKey, new HttpStatusCode[] { HttpStatusCode.OK });
            using (JsonDocument document = JsonDocument.Parse(response.Item2))
            {
                var documents = document.RootElement.GetProperty("documents").EnumerateArray().ToArray();
                // Convert Select results to arrays to prevent them being disposed with the JsonDocument.
                return documents.Select(document =>
                    {
                        var entities = document.GetProperty("entities").EnumerateArray().ToArray();
                        return entities.Select(entity => (entity.GetProperty("category").ToString(), entity.GetProperty("text").ToString())).ToArray();
                    }
                ).ToArray();
            }
        }

        // Get recognized entities (PII) for transcription phrases.
        async static Task<IEnumerable<IEnumerable<(string, string)>>> GetRecognizedEntitiesPII(string transcription_documents)
        {
            var uri = new UriBuilder(Uri.UriSchemeHttps, textAnalyticsHost);
            uri.Path = entitiesRecognitionPIIPath;

            // Entities recognition JSON request and response samples:
            // https://docs.microsoft.com/en-us/rest/api/cognitiveservices-textanalytics/3.1preview4/entities-recognition-general/entities-recognition-general#examples
            var response = await SendPost(uri.Uri.ToString(), transcription_documents, textAnalyticsKey, new HttpStatusCode[] { HttpStatusCode.OK });
            using (JsonDocument document = JsonDocument.Parse(response.Item2))
            {
                var documents = document.RootElement.GetProperty("documents").EnumerateArray().ToArray();
                // Convert Select results to arrays to prevent them being disposed with the JsonDocument.
                return documents.Select(document =>
                    {
                        var entities = document.GetProperty("entities").EnumerateArray().ToArray();
                        return entities.Select(entity => (entity.GetProperty("category").ToString(), entity.GetProperty("text").ToString())).ToArray();
                    }
                ).ToArray();
            }
        }

        // Print each transcription phrase, followed by its language, sentiment, and so on.
        static void PrintResults(
            IEnumerable<string> transcription_phrases,
            IEnumerable<string> transcription_languages,
            IEnumerable<string> transcription_sentiments,
            IEnumerable<IEnumerable<string>> key_phrases,
            IEnumerable<IEnumerable<(string, string)>> transcription_entities_general,
            IEnumerable<IEnumerable<(string, string)>> transcription_entities_pii)
        {
            var phrases_2 = transcription_phrases.Zip(transcription_languages).Select(t =>
                $"Phrase: {t.First}{Environment.NewLine}Language: {t.Second}{Environment.NewLine}"
            );
            var phrases_3 = phrases_2.Zip(transcription_sentiments).Select(t =>
                $"{t.First}Sentiment: {t.Second}{Environment.NewLine}"
            );
            var phrases_4 = phrases_3.Zip(key_phrases).Select(t =>
                {
                    var key_phrases = t.Second.Aggregate("", (result, key_phrase) => $"{result}    {key_phrase}{Environment.NewLine}");
                    return $"{t.First}Key phrases:{Environment.NewLine}{key_phrases}";
                }
            );
            var phrases_5 = phrases_4.Zip(transcription_entities_general).Select(t =>
                {
                    if (t.Second.Any())
                    {
                        var entities = t.Second.Aggregate("", (result, entity) => $"{result}    Category: {entity.Item1}. Text: {entity.Item2}.{Environment.NewLine}");
                        return $"{t.First}Recognized entities (general):{Environment.NewLine}{entities}";
                    }
                    else
                    {
                        return $"{t.First}Recognized entities (general): none.{Environment.NewLine}";
                    }
                }
            );
            var phrases_6 = phrases_5.Zip(transcription_entities_pii).Select(t =>
                {
                    if (t.Second.Any())
                    {
                        var entities = t.Second.Aggregate("", (result, entity) => $"{result}    Category: {entity.Item1}. Text: {entity.Item2}.{Environment.NewLine}");
                        return $"{t.First}Recognized entities (PII):{Environment.NewLine}{entities}";
                    }
                    else
                    {
                        return $"{t.First}Recognized entities (PII): none.{Environment.NewLine}";
                    }
                }
            );
            foreach (var phrase in phrases_6)
            {
                Console.WriteLine(phrase);
            }
            return;
        }

        public static async Task Run()
        {
            // How to use batch transcription:
            // https://github.com/MicrosoftDocs/azure-docs/blob/main/articles/cognitive-services/Speech-Service/batch-transcription.md
            var transcription_id = await CreateTranscription(transcriptionUri);
            WaitForTranscription(transcription_id);
            var transcription_url = await GetTranscriptionUri(transcription_id);
            var phrases = await GetTranscriptionPhrases(transcription_url);
            var languages = await GetTranscriptionLanguages(phrases);
            var documents = TranscriptionPhrasesToDocuments(phrases, languages);
            var sentiments = await GetSentiments(documents);
            var key_phrases = await GetKeyPhrases(documents);
            var entities_general = await GetRecognizedEntitiesGeneral(documents);
            var entities_pii = await GetRecognizedEntitiesPII(documents);
            PrintResults(phrases, languages, sentiments, key_phrases, entities_general, entities_pii);
            // Clean up resources.
            DeleteTranscription(transcription_id);
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
