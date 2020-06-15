//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.Remoting.Messaging;
using System.Threading.Tasks;

namespace BatchClient
{
    class Program
    {
        // Replace with your subscription key
        private const string SubscriptionKey = "YourSubscriptionKey";

        // Update with your service region
        private const string Region = "YourServiceRegion";

        // recordings and locale
        private const string Locale = "en-US";
        private static Uri RecordingsBlobUri = new Uri("<SAS URI pointing to an audio file stored in Azure Blob Storage>");
        //private static Uri ContentContainer = new Uri("<SAS URI pointing to an container in Azure Blob Storage>");
        
        // For use of custom trained model:
        private static EntityReference CustomModel = null;
        //private static EntityReference CustomModel =
        //    new EntityReference { Self = new Uri($"https://{Region}.api.cognitive.microsoft.com/speechtotext/v3.0/models/<id of custom model>")};
        private const string Name = "Simple transcription";

        static async Task Main(string[] args)
        {
            await TranscribeAsync().ConfigureAwait(false);
        }

        static async Task TranscribeAsync()
        {
            Console.WriteLine("Starting transcriptions client...");

            // create the client object and authenticate
            var client = BatchClient.CreateApiV3Client(SubscriptionKey, $"{Region}.api.cognitive.microsoft.com");

            Console.WriteLine("Deleting all existing completed transcriptions.");
            
            // get all transcriptions for the subscription
            var paginatedTranscriptions = await client.GetTranscriptionsAsync().ConfigureAwait(false);
            do
            {
                // delete all pre-existing completed transcriptions. If transcriptions are still running or not started, they will not be deleted
                foreach (var _ in paginatedTranscriptions.Values)
                {
                    // delete a transcription
                    await client.DeleteTranscriptionAsync(_.Self).ConfigureAwait(false);
                }

                paginatedTranscriptions = await client.GetTranscriptionsAsync().ConfigureAwait(false);
            }
            while (paginatedTranscriptions.NextLink != null);

            var transcription = new Transcription
            {
                Name = Name, 
                Locale = Locale, 
                ContentUrls = new[] { RecordingsBlobUri },
                //ContentContainerUrl = ContentContainer,
                Model = CustomModel,
                Properties = new TranscriptionProperties
                    {
                        IsWordLevelTimestampsEnabled = true,
                        TimeToLive = TimeSpan.FromDays(1)
                    }
            };

            transcription = await client.PostTranscriptionAsync(transcription).ConfigureAwait(false);

            // get the transcription Id from the location URI
            var createdTranscriptions = new List<Uri>();
            createdTranscriptions.Add(transcription.Self);

            Console.WriteLine($"Created transcription {transcription.Self}.");

            Console.WriteLine("Checking status.");

            // check for the status of our transcriptions periodically
            int completed = 0, running = 0, notStarted = 0;
            while (completed < 1)
            {
                completed = 0; running = 0; notStarted = 0;

                // <batchstatus>
                // get all transcriptions for the user
                paginatedTranscriptions = await client.GetTranscriptionsAsync().ConfigureAwait(false);
                do
                {
                    // delete all pre-existing completed transcriptions. If transcriptions are still running or not started, they will not be deleted
                    foreach (var _ in paginatedTranscriptions.Values)
                    {
                        switch (_.Status)
                        {
                            case "Failed":
                            case "Succeeded":
                                // we check to see if it was one of the transcriptions we created from this client.
                                if (!createdTranscriptions.Contains(_.Self))
                                {
                                    // not created form here, continue
                                    continue;
                                }
                                completed++;

                                // if the transcription was successful, check the results
                                if (_.Status == "Succeeded")
                                {
                                    var paginatedfiles = await client.GetTranscriptionFilesAsync(_.Links.Files).ConfigureAwait(false);

                                    var resultFile = paginatedfiles.Values.FirstOrDefault(f => f.Kind == ArtifactKind.Transcription);
                                    var result = await client.GetTranscriptionResultAsync(new Uri(resultFile.Links.ContentUrl)).ConfigureAwait(false);
                                    Console.WriteLine("Transcription succeeded. Results: ");
                                    Console.WriteLine(JsonConvert.SerializeObject(result, SpeechJsonContractResolver.WriterSettings));
                                }
                                else
                                {
                                    Console.WriteLine("Transcription failed. Status: {0}", _.Properties.Error.Message);
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
                    await Task.Delay(TimeSpan.FromMinutes(1)).ConfigureAwait(false);

                    paginatedTranscriptions = await client.GetTranscriptionsAsync().ConfigureAwait(false);
                }
                while (paginatedTranscriptions.NextLink != null);

                // for each transcription in the list we check the status
            }

            Console.WriteLine("Press any key...");
            Console.ReadKey();
        }
    }
}
