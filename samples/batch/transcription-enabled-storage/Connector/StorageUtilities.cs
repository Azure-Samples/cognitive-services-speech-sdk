// <copyright file="StorageUtilities.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector
{
    using System;
    using System.Collections.Generic;
    using System.IO;
    using System.Linq;
    using System.Threading.Tasks;
    using Microsoft.Extensions.Logging;
    using Microsoft.WindowsAzure.Storage;
    using Microsoft.WindowsAzure.Storage.Blob;

    public static class StorageUtilities
    {
        public static async Task<string> CreateSASAsync(string storageConnectionString, string filePath, ILogger log)
        {
            var containerName = GetContainerNameFromPath(filePath);
            var storageAccount = CloudStorageAccount.Parse(storageConnectionString);
            var blobClient = storageAccount.CreateCloudBlobClient();
            var container = blobClient.GetContainerReference(containerName);

            var containerPermissions = new BlobContainerPermissions
            {
                PublicAccess = BlobContainerPublicAccessType.Blob
            };

            await container.SetPermissionsAsync(containerPermissions).ConfigureAwait(false);
            containerPermissions.SharedAccessPolicies.Add("mypolicy", new SharedAccessBlobPolicy()
            {
                SharedAccessStartTime = DateTime.UtcNow,
                SharedAccessExpiryTime = DateTime.UtcNow.AddDays(1),
                Permissions = SharedAccessBlobPermissions.Read
            });

            var sas = container.GetSharedAccessSignature(containerPermissions.SharedAccessPolicies.FirstOrDefault().Value);
            var absoluteSasUri = filePath + sas;

            log.LogInformation($"Created sas url for file {filePath}");
            return absoluteSasUri;
        }

        public static async Task WriteTextFileToBlobAsync(string storageConnectionString, string content, string containerName, string fileName, ILogger log)
        {
            log.LogInformation($"Writing file {fileName} to container {containerName}.");

            var storageAccount = CloudStorageAccount.Parse(storageConnectionString);
            var fileClient = storageAccount.CreateCloudBlobClient();
            var container = fileClient.GetContainerReference(containerName);
            var blockBlob = container.GetBlockBlobReference(fileName);

            await blockBlob.UploadTextAsync(content).ConfigureAwait(false);
        }

        public static AudioDetails GetAudioDetails(byte[] audioBytes, string fileName, string locale, DateTime transcriptionCreationTime, bool isCustomModel, bool sentimentAnalysisAdded, bool entityRedactionAdded, ILogger log)
        {
            var fileNameExtension = Path.GetExtension(fileName);

            var duration = TimeSpan.Zero;
            var channels = 0;

            try
            {
                duration = AudioFileProcessor.GetDuration(audioBytes, fileNameExtension);
                channels = AudioFileProcessor.GetNumberOfChannels(audioBytes, fileNameExtension);
            }
            catch (Exception e)
            {
                if (e is InvalidDataException || e is FormatException)
                {
                    log.LogInformation("Failed to parse audio file. Exception: " + e.Message);
                }
                else
                {
                    throw;
                }
            }

            var estimatedCost = AudioFileProcessor.GetCostEstimation(duration, channels, isCustomModel, sentimentAnalysisAdded, entityRedactionAdded);
            return new AudioDetails(fileName, transcriptionCreationTime, duration, channels, estimatedCost, locale);
        }

        public static async Task WriteTranscriptionReceiptToStorageAsync(string storageConnectionString, string containerName, AudioDetails audioDetails, ILogger log)
        {
            if (audioDetails == null)
            {
                throw new ArgumentNullException(nameof(audioDetails));
            }

            var receipt = $"Transcription job \"{audioDetails.FileName}\" filed at {audioDetails.CreatedTime}.";
            if (audioDetails.EstimatedCost != 0d || audioDetails.AudioLength.TotalSeconds != 0d)
            {
                receipt += $"\nEstimated cost: {Math.Round(audioDetails.EstimatedCost, 4)} €";
                receipt += $"\nAudio length: {audioDetails.AudioLength}";
            }
            else
            {
                receipt += "\nUnable to parse file header.";
            }

            receipt += $"\nLocale: {audioDetails.Locale}";

            // file storage
            var storageAccount = CloudStorageAccount.Parse(storageConnectionString);
            var fileClient = storageAccount.CreateCloudBlobClient();

            // Get a reference to the file share we created previously.
            var container = fileClient.GetContainerReference(containerName);
            log.LogInformation("Got container reference for container: " + containerName);

            var fileNameWithoutExtension = GetFileNameWithoutExtension(audioDetails.FileName);
            var blockBlob = container.GetBlockBlobReference(fileNameWithoutExtension + "_receipt.txt");

            await blockBlob.UploadTextAsync(receipt).ConfigureAwait(false);
        }

        public static string GetFileNameFromPath(string filePath)
        {
            if (filePath == null)
            {
                throw new ArgumentNullException(nameof(filePath));
            }

            var pathParts = new Uri(filePath).AbsolutePath.Split('/').ToList();
            var cleanedPathParts = pathParts.SkipWhile(part => string.IsNullOrEmpty(part) || part.Equals("/", StringComparison.OrdinalIgnoreCase));

            var fileName = string.Join('/', cleanedPathParts.Skip(1));
            return fileName;
        }

        public static string GetFileNameWithoutExtension(string fileName)
        {
            if (fileName == null)
            {
                throw new ArgumentNullException(nameof(fileName));
            }

            var fileExtension = Path.GetExtension(fileName);

            return fileName.Substring(0, fileName.Length - fileExtension.Length);
        }

        public static string GetContainerNameFromPath(string filePath)
        {
            if (filePath == null)
            {
                throw new ArgumentNullException(nameof(filePath));
            }

            var pathParts = new Uri(filePath).AbsolutePath.Split('/').ToList();
            var cleanedPathParts = pathParts.SkipWhile(part => string.IsNullOrEmpty(part) || part.Equals("/", StringComparison.OrdinalIgnoreCase));

            var containerName = cleanedPathParts.FirstOrDefault();
            return containerName;
        }

        public static bool IsItAudio(string fileName)
        {
            string[] mediaExtensions =
            {
                ".WAV", ".MID", ".MIDI", ".WMA", ".MP3", ".OGG", ".RMA"
            };

            return mediaExtensions.Contains(Path.GetExtension(fileName).ToUpperInvariant());
        }

        public static async Task<byte[]> DownloadFileFromSAS(string blobSas)
        {
            var blob = new CloudBlockBlob(new Uri(blobSas));

            byte[] data;

            using (var memoryStream = new MemoryStream())
            {
                await blob.DownloadToStreamAsync(memoryStream).ConfigureAwait(false);
                data = new byte[memoryStream.Length];
                memoryStream.Position = 0;
                memoryStream.Read(data, 0, data.Length);
            }

            return data;
        }
    }
}
