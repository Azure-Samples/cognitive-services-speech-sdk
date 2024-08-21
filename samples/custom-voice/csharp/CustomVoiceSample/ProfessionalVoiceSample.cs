//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using Azure.Storage;
using Azure.Storage.Blobs;
using Azure.Storage.Sas;
using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Audio;

public class ProfessionalVoiceSample
{
    // Update your key and region here.
    private const string subscriptionKey = "YourSubscriptionKey";
    private const string region = "YourServiceRegion. E.g., 'eastus'";

    private const string projectId = "professional-voice-project-1";
    private const string consentId = "professional-voice-consent-1";
    private const string trainingSetId = "professional-voice-training-set-1";
    private const string modelId = "professional-voice-model-1";
    private static Guid endpointId = Guid.NewGuid();

    // Below are parameters for blob storage. You need to replace them with your own blob storage account information.
    private const string blobAccountName = "YourBlobAccountName. E.g., 'voicetest'.";
    private const string blobAccountKey = "YourBlobAccountKey";
    private const string containerName = "YourContainerName.";
    private const string consentblobPrefix = "YourBlobPrefix. E.g., consents";
    private const string trainingDataBlobPrefix = "YourTrainingDataBlobPrefix. E.g., professionalvoice/trainingset";

    private static CustomVoiceClient client = new CustomVoiceClient(region, subscriptionKey);

    public static async Task ProfessionalVoiceTestAsync()
    {
        try
        {
            // Step 1: Create a project
            var project = await CreateProjectAsync().ConfigureAwait(false);

            // Step 2: Create a consent
            var consent = await CreateConsentAsync().ConfigureAwait(false);

            // Step 3: Create a training set and upload training data
            var trainingSet = await UploadTrainingSetAsync().ConfigureAwait(false);

            // Step 4: Create a model
            var model = await CreateModelAsync().ConfigureAwait(false);

            // Step 5: deploy model
            var endpoint = await DeployModel().ConfigureAwait(false);

            // Step 6: speak test
            await ProfessionalVoiceSpeechSynthesisToWaveFileAsync(model.VoiceName, endpoint.Id);
        }
        catch (Exception ex)
        {
            Console.WriteLine($"Error: {ex.Message}");
        }
        finally
        {
            // Step 5: clean up
            Console.WriteLine("Uncomment below and Clean up resources if you don't need them.");

            // await client.DeleteEndpointAsync(endpointId).ConfigureAwait(false);
            // await client.DeleteConsentAsync(consentId).ConfigureAwait(false);
            // await client.DeleteModelAsync(modelId).ConfigureAwait(false);
            // await client.DeleteTrainingSetAsync(trainingSetId).ConfigureAwait(false);
            // await client.DeleteProjectAsync(projectId).ConfigureAwait(false);
        }
    }

    private static async Task<Project> CreateProjectAsync()
    {
        var project = await client.CreateProjectAsync(
            projectId,
            ProjectKind.ProfessionalVoice,
            "Test project for professional voice").ConfigureAwait(false);
        Console.WriteLine($"Project created. project id: {project.Id}");

        return project;
    }

    private static async Task<Consent> CreateConsentAsync()
    {
        //   Step 1: Upload a consent audio file to blob storage
        var audioUrl = await UploadSingleFileIntoBlobAsync(
            "TestData/VoiceTalentVerbalStatement.wav",
            blobAccountName,
            blobAccountKey,
            containerName,
            consentblobPrefix).ConfigureAwait(false);

        //   Step 2: Create a consent
        var consent = await client.CreateConsentAsync(
            consentId,
            projectId,
            "Sample Voice Actor",
            "Contoso",
            "en-US",
            audioUrl).ConfigureAwait(false);
        Console.WriteLine($"Consent created. consent id: {consent.Id}");

        return consent;
    }

    private static async Task<TrainingSet> UploadTrainingSetAsync()
    {
        // You can find sample script and audio file here.
        // https://github.com/Azure-Samples/Cognitive-Speech-TTS/blob/master/CustomVoice/Sample%20Data/Individual%20utterances%20%2B%20matching%20script/SampleScript.txt
        // https://github.com/Azure-Samples/Cognitive-Speech-TTS/blob/master/CustomVoice/Sample%20Data/Individual%20utterances%20%2B%20matching%20script/SampleAudios.zip
        //Pleae unzip audio file. Put both audio and script file in foler below.
        var localTrainingDataFolder = "YourLocalTrainingDataFolder. E.g., C:\\trainingset";

        //   Step 3.1: Create a training set
        var trainingSet = await client.CreateTrainingSetAsync(
            trainingSetId,
            projectId,
            "Test training set",
            "en-US").ConfigureAwait(false);
        Console.WriteLine($"Training set created. training set id: {trainingSet.Id}");

        //   Step 3.2: Upload training data to blob storage
        var trainingDataUrl = await UploadFolderIntoBlobAsync(
            localTrainingDataFolder,
            blobAccountName,
            blobAccountKey,
            containerName,
            trainingDataBlobPrefix).ConfigureAwait(false);

        //   Step 3.3: Add training data to the training set
        var audios = new AzureBlobContentSource()
        {
            ContainerUrl = trainingDataUrl,
            Prefix = trainingDataBlobPrefix,
            Extensions = new List<string> { ".wav" }
        };

        var scripts = new AzureBlobContentSource()
        {
            ContainerUrl = trainingDataUrl,
            Prefix = trainingDataBlobPrefix,
            Extensions = new List<string> { ".txt" }
        };

        await client.UploadDataThroughAzureBlobAsync(trainingSetId, DatasetKind.AudioAndScript, audios, scripts).ConfigureAwait(false);

        Console.Write("Uploading data into training set. It takes around 5 minutes to 1 hour depend on data size.");

        do
        {
            await Task.Delay(1000 * 10).ConfigureAwait(false);
            trainingSet = await client.GetTrainingSetAsync(trainingSetId).ConfigureAwait(false);
            Console.Write(".");
        }
        while (trainingSet.Status != Status.Succeeded && trainingSet.Status != Status.Failed);

        if (trainingSet.Status == Status.Failed)
        {
            throw new InvalidOperationException("Training set upload failed.");
        }

        return trainingSet;
    }

    private static async Task<Model> CreateModelAsync()
    {
        var model = await client.CreateModelAsync(
            modelId,
            projectId,
            "Test model",
            consentId,
            trainingSetId,
            "SampleVoiceNeural",
            RecipeKind.Default,
            locale: null,
            properties: null).ConfigureAwait(false);

        Console.WriteLine();
        Console.WriteLine($"Model created. model id: {model.Id}");

        Console.Write("Model is training in server. It takes around 24 hours.");
        while (model.Status != Status.Succeeded && model.Status != Status.Failed)
        {
            Console.Write(".");
            await Task.Delay(1000 * 300).ConfigureAwait(false);
            model = await client.GetModelAsync(modelId).ConfigureAwait(false);
        }

        if (model.Status == Status.Failed)
        {
            throw new InvalidOperationException("Model training failed.");
        }

        return model;
    }

    private static async Task<Endpoint> DeployModel()
    {
        var endpoint = await client.CreateEndpointAsync(
            endpointId,
            projectId,
            "Test endpoint",
            modelId).ConfigureAwait(false);

        Console.WriteLine();
        Console.WriteLine($"Start deploying model. endpoint id: {endpoint.Id}");
        Console.Write("Deploying model. It takes around 1 to 5 minutes.");
        while (endpoint.Status != Status.Succeeded && endpoint.Status != Status.Failed)
        {
            Console.Write(".");
            await Task.Delay(1000 * 10).ConfigureAwait(false);
            endpoint = await client.GetEndpointAsync(endpointId).ConfigureAwait(false);
        }

        if (endpoint.Status == Status.Failed)
        {
            throw new InvalidOperationException("Model deployment failed.");
        }

        return endpoint;
    }

    // blobPrefix is the folder name in the blob container
    private static async Task<Uri> UploadSingleFileIntoBlobAsync(
        string localFilePath,
        string blobAccountName,
        string blobAccountKey,
        string containerName,
        string blobPrefix)
    {
        // Put your storage account name and key here
        var storageSharedKeyCredential = new StorageSharedKeyCredential(blobAccountName, blobAccountKey);
        var blobServiceClient = new BlobServiceClient(new Uri($"https://{blobAccountName}.blob.core.windows.net"), storageSharedKeyCredential);
        var containerClient = blobServiceClient.GetBlobContainerClient(containerName);
        var blobName = $"{blobPrefix}/{Path.GetFileName(localFilePath)}";
        var blobClient = containerClient.GetBlobClient(blobName);
        using var fileStream = File.OpenRead(localFilePath);
        await blobClient.UploadAsync(fileStream, true).ConfigureAwait(false);
        
        // Generate a SAS token for the blob
        var sasUri = GenerateBlobSasTokenUrl(blobAccountName, blobAccountKey, containerName, blobName);
        return sasUri;
    }

    // Upload local folder to blob container
    private static async Task<Uri> UploadFolderIntoBlobAsync(
        string localFolderPath,
        string blobAccountName,
        string blobAccountKey,
        string containerName,
        string blobPrefix)
    {
        var storageSharedKeyCredential = new StorageSharedKeyCredential(blobAccountName, blobAccountKey);
        var blobServiceClient = new BlobServiceClient(new Uri($"https://{blobAccountName}.blob.core.windows.net"), storageSharedKeyCredential);
        var containerClient = blobServiceClient.GetBlobContainerClient(containerName);

        if (!Directory.Exists(localFolderPath))
        {
            throw new ArgumentException($"Local folder {localFolderPath} does not exist.");
        }

        var directory = new DirectoryInfo(localFolderPath);
        foreach (var file in directory.GetFiles())
        {
            var blobName = $"{blobPrefix}/{file.Name}";
            var blobClient = containerClient.GetBlobClient(blobName);
            using var fileStream = File.OpenRead(file.FullName);
            await blobClient.UploadAsync(fileStream, true).ConfigureAwait(false);
        }

        // Generate a SAS token for the blob
        var sasUri = GenerateContainerSasTokenUrl(blobAccountName, blobAccountKey, containerName);
        return sasUri;
    }

    private static Uri GenerateBlobSasTokenUrl(string accountName, string accountKey, string containerName, string blobPrefix)
    {
        var staorageSharedKeyCredential = new StorageSharedKeyCredential(accountName, accountKey);
        var blobServiceClient = new BlobServiceClient(new Uri($"https://{accountName}.blob.core.windows.net"), staorageSharedKeyCredential);
        var containerClient = blobServiceClient.GetBlobContainerClient(containerName);
        var sasBuilder = new BlobSasBuilder
        {
            BlobContainerName = containerName,
            BlobName = blobPrefix,
            Resource = "b",
            StartsOn = DateTimeOffset.UtcNow,
            ExpiresOn = DateTimeOffset.UtcNow.AddHours(3)
        };
        sasBuilder.SetPermissions(BlobSasPermissions.Read);
        var sasToken = sasBuilder.ToSasQueryParameters(staorageSharedKeyCredential);
        var sasUri = new Uri($"{containerClient.Uri}/{blobPrefix}?{sasToken}");
        return sasUri;
    }

    private static Uri GenerateContainerSasTokenUrl(string accountName, string accountKey, string containerName)
    {
        var staorageSharedKeyCredential = new StorageSharedKeyCredential(accountName, accountKey);
        var blobServiceClient = new BlobServiceClient(new Uri($"https://{accountName}.blob.core.windows.net"), staorageSharedKeyCredential);
        var containerClient = blobServiceClient.GetBlobContainerClient(containerName);
        var sasBuilder = new BlobSasBuilder
        {
            BlobContainerName = containerName,
            Resource = "c",
            StartsOn = DateTimeOffset.UtcNow,
            ExpiresOn = DateTimeOffset.UtcNow.AddHours(3)
        };
        sasBuilder.SetPermissions(BlobContainerSasPermissions.Read | BlobContainerSasPermissions.List);
        var sasToken = sasBuilder.ToSasQueryParameters(staorageSharedKeyCredential);
        var sasUri = new Uri($"{containerClient.Uri}?{sasToken}");
        return sasUri;
    }

    private static async Task ProfessionalVoiceSpeechSynthesisToWaveFileAsync(
        string voiceName,
        Guid endpointId)
    {
        var text = "This is a professional voice test!";
        var outputFilePath = "professionalvoice_output.mp3";

        var speechConfig = SpeechConfig.FromSubscription(subscriptionKey, region);
        speechConfig.EndpointId = endpointId.ToString();
        speechConfig.SpeechSynthesisVoiceName = voiceName;
        speechConfig.SetSpeechSynthesisOutputFormat(SpeechSynthesisOutputFormat.Audio24Khz160KBitRateMonoMp3);
        using var audioConfig = AudioConfig.FromWavFileOutput(outputFilePath);
        using var synthesizer = new SpeechSynthesizer(speechConfig, audioConfig);

        using var result = await synthesizer.SpeakTextAsync(text).ConfigureAwait(false);

        if (result.Reason == ResultReason.SynthesizingAudioCompleted)
        {
            Console.WriteLine($"Speech synthesis succeeded. The audio was saved to {outputFilePath}");
        }
        else if (result.Reason == ResultReason.Canceled)
        {
            var cancellation = SpeechSynthesisCancellationDetails.FromResult(result);
            Console.WriteLine($"Speech synthesis canceled: Reason={cancellation.Reason}");
            if (cancellation.Reason == CancellationReason.Error)
            {
                Console.WriteLine($"Result id: {result.ResultId}");
                Console.WriteLine($"Error details: {cancellation.ErrorDetails}");
            }
        }
    }
}
