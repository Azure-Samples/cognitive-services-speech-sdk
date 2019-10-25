//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.IO;
using System.Net;
using System.Threading.Tasks;
using System.Net.Http;
using System.Net.Http.Formatting;

namespace BatchClient
{
    class Program
    {
        // Replace with your subscription key
        private const string SubscriptionKey = "key";

        // Update with your service region
        private const string Region = "region";
        private const int Port = 443;

        // recordings and locale
        private const string Locale = "en-US";
        private const string RecordingsBlobUri = "SAS URI";

        //name and description
        private const string Name = "Simple transcription";
        private const string Description = "Simple transcription description";

        private const string speechToTextBasePath = "api/speechtotext/v2.0/";

        static void Main(string[] args)
        {
            TranscribeAsync().Wait();
        }

        static async Task TranscribeAsync()
        {
            Console.WriteLine("Starting transcriptions client...");

            // create the client object and authenticate
            var client = new HttpClient();
            client.Timeout = TimeSpan.FromMinutes(25);
            client.BaseAddress = new UriBuilder(Uri.UriSchemeHttps, $"{Region}.cris.ai", Port).Uri;

            client.DefaultRequestHeaders.Add("Ocp-Apim-Subscription-Key", SubscriptionKey);

            var path = $"{speechToTextBasePath}Transcriptions/";
            var transcriptionDefinition = TranscriptionDefinition.Create(Name, Description, Locale, new Uri(RecordingsBlobUri));

            string res = JsonConvert.SerializeObject(transcriptionDefinition);
            StringContent sc = new StringContent(res);
            sc.Headers.ContentType = JsonMediaTypeFormatter.DefaultMediaType;

            Uri transcriptionLocation = null;

            using (var response = await client.PostAsync(path, sc))
            {
                if (!response.IsSuccessStatusCode)
                {
                    Console.WriteLine("Error {0} starting transcription.", response.StatusCode);
                    return;
                }

                transcriptionLocation = response.Headers.Location;
            }

            Console.WriteLine($"Created transcription at location {transcriptionLocation}.");

            Console.WriteLine("Checking status.");

            bool completed = false;
            Transcription transcription = null;

            // check for the status of our transcriptions periodically
            while (!completed)
            {
                // get all transcriptions for the user
                using (var response = await client.GetAsync(transcriptionLocation.AbsolutePath).ConfigureAwait(false))
                {
                    var contentType = response.Content.Headers.ContentType;

                    if (response.IsSuccessStatusCode && string.Equals(contentType.MediaType, "application/json", StringComparison.OrdinalIgnoreCase))
                    {
                        transcription = await response.Content.ReadAsAsync<Transcription>().ConfigureAwait(false);
                    }
                    else
                    {
                        Console.WriteLine("Error with status {0} getting transcription result", response.StatusCode);
                        continue;
                    }

                }

                // for each transcription in the list we check the status
                switch (transcription.Status)
                {
                    case "Failed":
                        completed = true;
                        Console.WriteLine("Transcription failed. Status: {0}", transcription.StatusMessage);
                        break;
                    case "Succeeded":
                        completed = true;

                        var resultsUri0 = transcription.ResultsUrls["channel_0"];

                        WebClient webClient = new WebClient();

                        var filename = Path.GetTempFileName();
                        webClient.DownloadFile(resultsUri0, filename);
                        var results0 = File.ReadAllText(filename);
                        var resultObject0 = JsonConvert.DeserializeObject<RootObject>(results0);
                        Console.WriteLine(results0);

                        Console.WriteLine("Transcription succeeded. Results: ");
                        Console.WriteLine(results0);
                        break;

                    case "Running":
                        Console.WriteLine("Transcription is still running.");
                        break;

                    case "NotStarted":
                        Console.WriteLine("Transcription has not started.");
                        break;
                }
                
                await Task.Delay(TimeSpan.FromSeconds(5)).ConfigureAwait(false);
            }

            Console.WriteLine("Press any key...");
            Console.ReadKey();
        }
    }

    public sealed class ModelIdentity
    {
        private ModelIdentity(Guid id)
        {
            this.Id = id;
        }

        public Guid Id { get; private set; }

        public static ModelIdentity Create(Guid Id)
        {
            return new ModelIdentity(Id);
        }
    }

    public sealed class Transcription
    {
        [JsonConstructor]
        private Transcription(Guid id, string name, string description, string locale, DateTime createdDateTime, DateTime lastActionDateTime, string status, Uri recordingsUrl, IReadOnlyDictionary<string, string> resultsUrls)
        {
            this.Id = id;
            this.Name = name;
            this.Description = description;
            this.CreatedDateTime = createdDateTime;
            this.LastActionDateTime = lastActionDateTime;
            this.Status = status;
            this.Locale = locale;
            this.RecordingsUrl = recordingsUrl;
            this.ResultsUrls = resultsUrls;
        }

        /// <inheritdoc />
        public string Name { get; set; }

        /// <inheritdoc />
        public string Description { get; set; }

        /// <inheritdoc />
        public string Locale { get; set; }

        /// <inheritdoc />
        public Uri RecordingsUrl { get; set; }

        /// <inheritdoc />
        public IReadOnlyDictionary<string, string> ResultsUrls { get; set; }

        public Guid Id { get; set; }

        /// <inheritdoc />
        public DateTime CreatedDateTime { get; set; }

        /// <inheritdoc />
        public DateTime LastActionDateTime { get; set; }

        /// <inheritdoc />
        public string Status { get; set; }

        public string StatusMessage { get; set; }
    }

    public sealed class TranscriptionDefinition
    {
        private TranscriptionDefinition(string name, string description, string locale, Uri recordingsUrl, IEnumerable<ModelIdentity> models)
        {
            this.Name = name;
            this.Description = description;
            this.RecordingsUrl = recordingsUrl;
            this.Locale = locale;
            this.Models = models;
            this.properties = new Dictionary<string, string>();
            this.properties.Add("PunctuationMode", "DictatedAndAutomatic");
            this.properties.Add("ProfanityFilterMode", "Masked");
            this.properties.Add("AddWordLevelTimestamps", "True");
        }

        /// <inheritdoc />
        public string Name { get; set; }

        /// <inheritdoc />
        public string Description { get; set; }

        /// <inheritdoc />
        public Uri RecordingsUrl { get; set; }

        public string Locale { get; set; }

        public IEnumerable<ModelIdentity> Models { get; set; }

        public IDictionary<string, string> properties { get; set; }

        public static TranscriptionDefinition Create(
            string name,
            string description,
            string locale,
            Uri recordingsUrl)
        {
            return TranscriptionDefinition.Create(name, description, locale, recordingsUrl, new ModelIdentity[0]);
        }

        public static TranscriptionDefinition Create(
            string name,
            string description,
            string locale,
            Uri recordingsUrl,
            IEnumerable<ModelIdentity> models)
        {
            return new TranscriptionDefinition(name, description, locale, recordingsUrl, models);
        }
    }

    public class AudioFileResult
    {
        public string AudioFileName { get; set; }
        public List<SegmentResult> SegmentResults { get; set; }
    }

    public class RootObject
    {
        public List<AudioFileResult> AudioFileResults { get; set; }
    }

    public class NBest
    {
        public double Confidence { get; set; }
        public string Lexical { get; set; }
        public string ITN { get; set; }
        public string MaskedITN { get; set; }
        public string Display { get; set; }
    }

    public class SegmentResult
    {
        public string RecognitionStatus { get; set; }
        public string Offset { get; set; }
        public string Duration { get; set; }
        public List<NBest> NBest { get; set; }
    }
}
