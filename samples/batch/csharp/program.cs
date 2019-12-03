//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Threading.Tasks;

namespace BatchClient
{
    class Program
    {
        // <batchdefinition>
        // Replace with your subscription key
        private const string SubscriptionKey = "YourSubscriptionKey";

        // Update with your service region
        private const string Region = "YourServiceRegion";
        private const int Port = 443;

        // recordings and locale
        private const string Locale = "en-US";
        private const string RecordingsBlobUri = "<SAS URI pointing to an audio file stored in Azure Blob Storage>";

        // For usage of baseline models, no acoustic and language model needs to be specified.
        private static Guid[] modelList = new Guid[0];

        // For use of specific acoustic and language models:
        // - comment the previous line
        // - uncomment the next lines to create an array containing the guids of your required model(s)
        // private static Guid AdaptedAcousticId = new Guid("<id of the custom acoustic model>");
        // private static Guid AdaptedLanguageId = new Guid("<id of the custom language model>");
        // private static Guid[] modelList = new[] { AdaptedAcousticId, AdaptedLanguageId };

        //name and description
        private const string Name = "Simple transcription";
        private const string Description = "Simple transcription description";
        // </batchdefinition>

        static void Main(string[] args)
            {
                TranscribeAsync().Wait();
            }

        static async Task TranscribeAsync()
        {
            Console.WriteLine("Starting transcriptions client...");

            // create the client object and authenticate
            var client = BatchClient.CreateApiV2Client(SubscriptionKey, $"{Region}.cris.ai", Port);

            // get all transcriptions for the subscription
            var transcriptions = await client.GetTranscriptionsAsync().ConfigureAwait(false);

            Console.WriteLine("Deleting all existing completed transcriptions.");
            // delete all pre-existing completed transcriptions. If transcriptions are still running or not started, they will not be deleted
            foreach (var item in transcriptions)
            {
                // delete a transcription
                await client.DeleteTranscriptionAsync(item.Id).ConfigureAwait(false);
            }

            var transcriptionLocation = await client.PostTranscriptionAsync(Name, Description, Locale, new Uri(RecordingsBlobUri), modelList).ConfigureAwait(false);
            var thisTranscriptionGuid = new Guid(transcriptionLocation.ToString().Split('/').LastOrDefault());

            // get the transcription Id from the location URI
            var createdTranscriptions = new List<Guid>();
            createdTranscriptions.Add(thisTranscriptionGuid);

            Console.WriteLine($"Created transcription with id {thisTranscriptionGuid}.");

            Console.WriteLine("Checking status.");

            // check for the status of our transcriptions periodically
            int completed = 0, running = 0, notStarted = 0;
            while (completed < 1)
            {
                // <batchstatus>
                // get all transcriptions for the user
                transcriptions = await client.GetTranscriptionsAsync().ConfigureAwait(false);

                completed = 0; running = 0; notStarted = 0;
                // for each transcription in the list we check the status
                foreach (var transcription in transcriptions)
                {
                    switch (transcription.Status)
                    {
                        case "Failed":
                        case "Succeeded":
                            // we check to see if it was one of the transcriptions we created from this client.
                            if (!createdTranscriptions.Contains(transcription.Id))
                            {
                                // not created form here, continue
                                continue;
                            }
                            completed++;

                            // if the transcription was successful, check the results
                            if (transcription.Status == "Succeeded")
                            {
                                var resultsUri0 = transcription.ResultsUrls["channel_0"];

                                WebClient webClient = new WebClient();

                                var filename = Path.GetTempFileName();
                                webClient.DownloadFile(resultsUri0, filename);
                                var results0 = File.ReadAllText(filename);
                                var resultObject0 = JsonConvert.DeserializeObject<RootObject>(results0);
                                Console.WriteLine(results0);

                                Console.WriteLine("Transcription succeeded. Results: ");
                                Console.WriteLine(results0);
                            }
                            else
                            {
                                Console.WriteLine("Transcription failed. Status: {0}", transcription.StatusMessage);
                            }
                            break;

                        case "Running":
                            running++;
                            break;

                        case "NotStarted":
                            notStarted++;
                            break;
                    }
                }
                // </batchstatus>

                Console.WriteLine(string.Format("Transcriptions status: {0} completed, {1} running, {2} not started yet", completed, running, notStarted));
                // check again after 5 seconds (can also be 1, 2, 5 min depending on usage).
                await Task.Delay(TimeSpan.FromSeconds(5)).ConfigureAwait(false);
            }

            Console.WriteLine("Press any key...");
            Console.ReadKey();
        }
    }
}
