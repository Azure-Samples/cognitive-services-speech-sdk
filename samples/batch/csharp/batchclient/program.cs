//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace BatchClient
{
    using System;
    using System.IO;
    using System.Collections.Generic;
    using System.Linq;
    using System.Threading.Tasks;
    using Newtonsoft.Json;

    public class Program
    {
        private static UserConfig _userConfig;

        // replace with your app service name (check publish on webhook receiver project)
        public const string WebHookAppServiceName = "YourAppServiceName";

        // replace with a secure secret (used for hashing)
        public const string WebHookSecret = "somethingverysecretisbesthere";

        private static Uri WebHookCallbackUrl = new Uri($"https://{WebHookAppServiceName}.azurewebsites.net/api/callback");

        // For use of custom trained model:
        private static EntityReference CustomModel = null;
        //private static EntityReference CustomModel =
        //    new EntityReference { Self = new Uri($"https://{Region}.api.cognitive.microsoft.com/speechtotext/v3.0/models/<id of custom model>")};

        private const string DisplayName = "Simple transcription";

        static void Main(string[] args)
        {
            const string usage = @"USAGE: dotnet run -- [...]

  HELP
    --help                           Show this help and stop.

  CONNECTION
    --key KEY                        Your Azure Speech service resource key. Use the `--key` option.
    --region REGION                  Your Azure Speech service region.Use the `--region` option.
                                     Examples: westus, eastus

  Locale

  INPUT
    --recordingsBlobUris             Input audios SAS URI (if input multiple, please separate them with commas) stored in Azure Blob Storage. Optional.
                                     Examples: https://<storage_account_name>.blob.core.windows.net/<container_name>/<file_name_1>?SAS_TOKEN_1,https://<storage_account_name>.blob.core.windows.net/<container_name>/<file_name_2>?SAS_TOKEN_2
    --recordingsContainerUri         Input audios Azure Blob Storage Container SAS URI. Optional.
                                     Examples: https://<storage_account_name>.blob.core.windows.net/<container_name>?SAS_TOKEN
    --locale                         Specify locale. The locale of recordings.
                                     Examples: en-US, ja-JP
";

            if (args.Contains("--help"))
            {
                Console.WriteLine(usage);
            }
            else
            {
                Program program = new Program();
                program.Initialize(args, usage);

                RunAsync().Wait();
            }
        }

        private void Initialize(string[] args, string usage)
        {
            _userConfig = UserConfig.UserConfigFromArgs(args, usage);
        }

        private static async Task RunAsync()
        {
            // create the client object and authenticate
            using (var client = BatchClient.CreateApiClient(_userConfig.subscriptionKey, $"{_userConfig.region}.api.cognitive.microsoft.com", "2024-11-15"))
            {
                // uncomment next line when using web hooks
                // await SetupWebHookAsync(client).ConfigureAwait(false);

                await TranscribeAsync(client).ConfigureAwait(false);

                // uncomment next line when using web hooks
                // await DeleteAllWebHooksAsync(client).ConfigureAwait(false);
            }
        }

        private async static Task TranscribeAsync(BatchClient client)
        {
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
                Locale = _userConfig.locale,
                ContentUrls = _userConfig.recordingsBlobUris,
                // ContentContainerUrl = _userConfig.contentAzureBlobContainer,
                Model = CustomModel,
                Properties = new TranscriptionProperties
                {
                    TimeToLiveHours = 6,
                    IsWordLevelTimestampsEnabled = true,
                    IsDisplayFormWordLevelTimestampsEnabled = false,

                    // uncomment the following block to enable and configure speaker separation
                    // Diarization = new DiarizationProperties
                    // {
                    //     Enabled = true,
                    //     MaxSpeakers = 5
                    // },

                    // // uncomment the following block to enable and configure language identification prior to transcription. Available modes are "Single" and "Continuous".
                    // LanguageIdentification = new LanguageIdentificationProperties
                    // {
                    //     CandidateLocales = new CultureInfo[] { new CultureInfo("en-US"), new CultureInfo("ja-JP") },
                    //     LanguageIdentificationMode = "Single",
                    // }
                }
            };

            newTranscription = await client.CreateTranscriptionAsync(newTranscription).ConfigureAwait(false);
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

                                    var resultFiles = paginatedfiles.Values.Where(f => f.Kind == ArtifactKind.Transcription);

                                    if (!resultFiles.Any())
                                    {
                                        Console.WriteLine("No transcription results found.");
                                        return;
                                    }

                                    Console.WriteLine("Transcription succeeded. Found {0} result files.", resultFiles.Count());

                                    string resultsDir = "results";
                                    if (!Directory.Exists(resultsDir))
                                    {
                                        Directory.CreateDirectory(resultsDir);
                                    }

                                    foreach (var resultFile in resultFiles)
                                    {
                                        var result = await client.GetTranscriptionResultAsync(new Uri(resultFile.Links.ContentUrl)).ConfigureAwait(false);
                                        string resultJsonStr = JsonConvert.SerializeObject(result, SpeechJsonContractResolver.WriterSettings);

                                        // print the results to console
                                        Console.WriteLine("Transcription of {0} succeeded. Results: ", resultFile.Name);
                                        Console.WriteLine(resultJsonStr);

                                        string sanitizedFileName = string.Concat(resultFile.Name.Split(Path.GetInvalidFileNameChars()));
                                        if (!sanitizedFileName.EndsWith(".json"))
                                        {
                                            sanitizedFileName += ".json";
                                        }

                                        string filePath = Path.Combine(resultsDir, sanitizedFileName);

                                        System.IO.File.WriteAllText(filePath, resultJsonStr);

                                        Console.WriteLine($"Transcription of {resultFile.Name} results written to file: {filePath}");
                                    }
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
                if (completed < 1) {
                    await Task.Delay(TimeSpan.FromMinutes(1)).ConfigureAwait(false);
                }
            }

            Console.WriteLine("Press any key...");
            Console.ReadKey();
        }

        private async static Task SetupWebHookAsync(BatchClient client)
        {
            await DeleteAllWebHooksAsync(client).ConfigureAwait(false);

            var webHook = new WebHook
            {
                DisplayName = "Transcription web hook",
                Events = new Dictionary<WebHookEventKind, bool>
                {
                    [WebHookEventKind.TranscriptionCreation] = true,
                    [WebHookEventKind.TranscriptionProcessing] = true,
                    [WebHookEventKind.TranscriptionCompletion] = true
                },
                WebUrl = WebHookCallbackUrl,
                Properties = new WebHookProperties
                {
                    Secret = WebHookSecret
                }
            };

            webHook = await client.CreateWebHookAsync(webHook);

            Console.WriteLine(string.Format("Created webHook {0}.", webHook.Self));
        }

        private static async Task DeleteAllWebHooksAsync(BatchClient client)
        {
            // get all web hooks for the subscription
            PaginatedWebHooks paginatedWebHooks = null;
            do
            {
                if (paginatedWebHooks == null)
                {
                    paginatedWebHooks = await client.GetWebHooksAsync().ConfigureAwait(false);
                }
                else
                {
                    paginatedWebHooks = await client.GetWebHooksAsync(paginatedWebHooks.NextLink).ConfigureAwait(false);
                }

                // delete all pre-existing web hooks.
                foreach (var webHooksToDelete in paginatedWebHooks.Values)
                {
                    // delete web hook
                    await client.DeleteWebHookAsync(webHooksToDelete.Self).ConfigureAwait(false);
                    Console.WriteLine($"Deleted web hook {webHooksToDelete.Self}");
                }
            }
            while (paginatedWebHooks.NextLink != null);
        }
    }
}
