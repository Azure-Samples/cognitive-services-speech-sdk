//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace BatchClient
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Threading.Tasks;
    using Newtonsoft.Json;

    class Program
    {
        // Replace with your subscription key
        private const string SubscriptionKey = "YourSubscriptionKey";

        // Update with your service region
        private const string Region = "YourServiceRegion";

        // recordings and locale
        private const string Locale = "en-US";
        private static Uri RecordingsBlobUri = new Uri("<SAS URI pointing to an audio file stored in Azure Blob Storage>");
        //private static Uri ContentAzureBlobContainer = new Uri("<SAS URI pointing to an container in Azure Blob Storage>");

        // For use of custom trained model:
        private static EntityReference CustomModel = null;
        //private static EntityReference CustomModel =
        //    new EntityReference { Self = new Uri($"https://{Region}.api.cognitive.microsoft.com/speechtotext/v3.0/models/<id of custom model>")};
        private const string DisplayName = "Simple transcription";

#pragma warning disable UseAsyncSuffix // Use Async suffix
        static async Task Main(string[] args)
#pragma warning restore UseAsyncSuffix // Use Async suffix
        {
            Console.WriteLine("Starting transcriptions client...");

            // create the client object and authenticate
            var client = BatchClient.CreateApiV3Client(SubscriptionKey, $"{Region}.api.cognitive.microsoft.com");

            Console.WriteLine("Deleting all existing completed transcriptions.");

            // get all transcriptions for the subscription
            PaginatedTranscriptions paginatedTranscriptions = null;
            do
            {
                if (paginatedTranscriptions == null)
                {
                    paginatedTranscriptions = await client.GetTranscriptionsAsync().ConfigureAwait(false);
                }
                else
                {
                    paginatedTranscriptions = await client.GetTranscriptionsAsync(paginatedTranscriptions.NextLink).ConfigureAwait(false);
                }

                // delete all pre-existing completed transcriptions. If transcriptions are still running or not started, they will not be deleted
                foreach (var transcriptionToDelete in paginatedTranscriptions.Values)
                {
                    // delete a transcription
                    await client.DeleteTranscriptionAsync(transcriptionToDelete.Self).ConfigureAwait(false);
                    Console.WriteLine($"Deleted transcription {transcriptionToDelete.Self}");
                }
            }
            while (paginatedTranscriptions.NextLink != null);

            // <transcriptiondefinition>
            var newTranscription = new Transcription
            {
                DisplayName = DisplayName, 
                Locale = Locale, 
                ContentUrls = new[] { RecordingsBlobUri },
                //ContentContainerUrl = ContentAzureBlobContainer,
                Model = CustomModel,
                Properties = new TranscriptionProperties
                {
                    IsWordLevelTimestampsEnabled = true,
                    TimeToLive = TimeSpan.FromDays(1)
                }
            };

            newTranscription = await client.PostTranscriptionAsync(newTranscription).ConfigureAwait(false);
            Console.WriteLine($"Created transcription {newTranscription.Self}");
            // </transcriptiondefinition>

            // get the transcription Id from the location URI
            var createdTranscriptions = new List<Uri> { newTranscription.Self };

            Console.WriteLine("Checking status.");

            // get the status of our transcriptions periodically and log results
            int completed = 0, running = 0, notStarted = 0;
            while (completed < 1)
            {
                completed = 0; running = 0; notStarted = 0;

                // get all transcriptions for the user
                paginatedTranscriptions = null;
                do
                {
                    // <transcriptionstatus>
                    if (paginatedTranscriptions == null)
                    {
                        paginatedTranscriptions = await client.GetTranscriptionsAsync().ConfigureAwait(false);
                    }
                    else
                    {
                        paginatedTranscriptions = await client.GetTranscriptionsAsync(paginatedTranscriptions.NextLink).ConfigureAwait(false);
                    }

                    // delete all pre-existing completed transcriptions. If transcriptions are still running or not started, they will not be deleted
                    foreach (var transcription in paginatedTranscriptions.Values)
                    {
                        switch (transcription.Status)
                        {
                            case "Failed":
                            case "Succeeded":
                                // we check to see if it was one of the transcriptions we created from this client.
                                if (!createdTranscriptions.Contains(transcription.Self))
                                {
                                    // not created form here, continue
                                    continue;
                                }

                                completed++;

                                // if the transcription was successful, check the results
                                if (transcription.Status == "Succeeded")
                                {
                                    var paginatedfiles = await client.GetTranscriptionFilesAsync(transcription.Links.Files).ConfigureAwait(false);

                                    var resultFile = paginatedfiles.Values.FirstOrDefault(f => f.Kind == ArtifactKind.Transcription);
                                    var result = await client.GetTranscriptionResultAsync(new Uri(resultFile.Links.ContentUrl)).ConfigureAwait(false);
                                    Console.WriteLine("Transcription succeeded. Results: ");
                                    Console.WriteLine(JsonConvert.SerializeObject(result, SpeechJsonContractResolver.WriterSettings));
                                }
                                else
                                {
                                    Console.WriteLine("Transcription failed. Status: {0}", transcription.Properties.Error.Message);
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

                    // for each transcription in the list we check the status
                    Console.WriteLine(string.Format("Transcriptions status: {0} completed, {1} running, {2} not started yet", completed, running, notStarted));
                }
                while (paginatedTranscriptions.NextLink != null);

                // </transcriptionstatus>
                // check again after 1 minute
                await Task.Delay(TimeSpan.FromMinutes(1)).ConfigureAwait(false);
            }

            Console.WriteLine("Press any key...");
            Console.ReadKey();
        }
    }
}
