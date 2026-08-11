using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Net.Http;
using System.Net.Http.Json;
using System.Runtime.InteropServices;
using System.Text.Json.Serialization;
using Newtonsoft.Json;

namespace fastTranscriptionClient
{
    public class FastTranscriptionOptions
    {
        public string[] Locales { get; set; } = { "en-US" };
        public string ProfanityFilterMode { get; set; } = "masked";
        public int[] Channels { get; set; } = { 0 };
        public Dictionary<string, int> DiarizationSettings { get; set; } = new Dictionary<string, int>{
             { "minSpeakers", 1 },
             { "maxSpeakers", 4 }
            };

    }

    public class Program
    {
        //Replace with your subscription key
        public const string SubscriptionKey = "YourSubscriptionKey"; //Update with your subscription key

        // Update with your service region
        public const string Region =  "YourServiceRegion"; //Update with your service region

        // locale and endpoint URL
        private const string Locale = "en-US";
        
        //local storage location for testing 
        private const string DisplayName = "Fast transcription";


        private static async Task Main(string[] args)
        {
            var audio_path = @"YourAudio_FilePath"; //- update to your audio file location
            var transcription_path = @"transcription_response.json"; // path to store your transcription output
            try
            {
                await TranscribeAsync(audio_path, transcription_path).ConfigureAwait(false);
            }
            catch (Exception ex)
            {
                Console.WriteLine($"An error occurred: {ex.Message}");
            }
        }

        public static async Task TranscribeAsync(string audio_filepath, string transcriptions_filepath)
        {
            var fastTranscriptionEndpointPath = $"https://{Region}.api.cognitive.microsoft.com//speechtotext/transcriptions:transcribe?api-version=2024-05-15-preview";

            var client = new HttpClient();
            var request = new HttpRequestMessage(HttpMethod.Post, fastTranscriptionEndpointPath);
           
            request.Headers.Add("Ocp-apim-subscription-key", SubscriptionKey);
            request.Headers.Add("Accept", "application/json");


            var content = new MultipartFormDataContent();

            // Add the audio file
            content.Add(new StreamContent(File.OpenRead(audio_filepath)), "audio", "test_call_audio.wav");

            // Add definition
            //var transcriptionDefinition = new StringContent("{\"locales\": [\"en-us\"],\"profanityfiltermode\": \"masked\", \"channels\": [0, 1]}", System.Text.Encoding.UTF8, "application/json");
            var fastTranscriptionOptions = new FastTranscriptionOptions();
            var transcriptionDefinition = JsonContent.Create(fastTranscriptionOptions);
            content.Add(transcriptionDefinition, "definition");

            request.Content = content;

            // Make the API request
            var response = await client.SendAsync(request).ConfigureAwait(false);
            response.EnsureSuccessStatusCode();

            var response_content = await response.Content.ReadAsStringAsync();
            Console.WriteLine(response_content);

            var response_content_json = JsonConvert.SerializeObject(JsonConvert.DeserializeObject(response_content), Formatting.Indented);
            
            //save transcriptions to the transcriptions file location
            await File.WriteAllTextAsync(transcriptions_filepath, response_content_json);
            Console.WriteLine($"Response saved to {transcriptions_filepath}");

        }
    }
}
