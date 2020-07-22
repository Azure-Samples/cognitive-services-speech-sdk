// <copyright file="StorageUtilities.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector
{
    using System;
    using System.IO;
    using System.Linq;
    using System.Text;
    using System.Threading.Tasks;
    using Microsoft.Extensions.Logging;
    using Microsoft.WindowsAzure.Storage;
    using Microsoft.WindowsAzure.Storage.Blob;
    using Newtonsoft.Json;

    public static class StorageUtilities
    {
        public static (string, string, string) CreateSAS(string storageConnectionString, string filePath, ILogger log)
        {
            var audioContainerPath = GetContainerPathFromSAS(filePath, log);

            var storageAccount = CloudStorageAccount.Parse(storageConnectionString);
            var blobClient = storageAccount.CreateCloudBlobClient();

            var container = blobClient.GetContainerReference(audioContainerPath.Split('/').FirstOrDefault());

            var containerPermissions = new BlobContainerPermissions
            {
                PublicAccess = BlobContainerPublicAccessType.Blob
            };

            container.SetPermissionsAsync(containerPermissions);
            containerPermissions.SharedAccessPolicies.Add("mypolicy", new SharedAccessBlobPolicy()
            {
                SharedAccessStartTime = DateTime.UtcNow,
                SharedAccessExpiryTime = DateTime.UtcNow.AddDays(100),
                Permissions = SharedAccessBlobPermissions.Read | SharedAccessBlobPermissions.Write
            });

#pragma warning disable CA5377 // Use Container Level Access Policy
            var sas = container.GetSharedAccessSignature(containerPermissions.SharedAccessPolicies.FirstOrDefault().Value);
#pragma warning restore CA5377 // Use Container Level Access Policy
            log.LogInformation("SAS URI: " + sas);
            var sasBlobClient = new CloudBlobClient(storageAccount.BlobEndpoint);

            var blob = sasBlobClient.GetBlobReferenceFromServerAsync(new Uri(filePath + sas));

            log.LogInformation("Absolute sas uri: " + blob.Result.Uri.AbsoluteUri + sas);
            log.LogInformation("Blob container name is: " + blob.Result.Container.Name);
            log.LogInformation("Blob name is: " + blob.Result.Name);
            return (blob.Result.Uri.AbsoluteUri + sas, blob.Result.Container.Name, blob.Result.Name);
        }

        public static void WriteTextFileToBlob(string storageConnectionString, string content, string containerName, string fileName, ILogger log)
        {
            log.LogInformation($"Writing file {fileName} to container {containerName}.");

            var storageAccount = CloudStorageAccount.Parse(storageConnectionString);
            var fileClient = storageAccount.CreateCloudBlobClient();
            var container = fileClient.GetContainerReference(containerName);
            var blockBlob = container.GetBlockBlobReference(fileName);
            blockBlob.UploadTextAsync(content);
        }

        public static async Task<AudioDetails> WriteTranscriptionAcknowledgementToBlob(string storageConnectionString, byte[] audioBytes, string locale, string containerName, string fileName, ILogger log)
        {
            if (fileName == null)
            {
                throw new ArgumentNullException(nameof(fileName));
            }

            fileName = fileName.Replace("%20", " ", StringComparison.OrdinalIgnoreCase);

            var createdTime = DateTime.UtcNow;
            var receipt = "TranscriptionID:" + fileName + " filed at " + createdTime;

            var ext = Path.GetExtension(fileName);
            log.LogInformation("Determined file name extension:" + ext);

            (TimeSpan duration, int channels, double estimatedCost) = GetAudioDetailsFromBytes(audioBytes, ext, log);

            if (duration != TimeSpan.Zero && channels != 0 && estimatedCost != 0d)
            {
                receipt += "\nAudio length: " + duration.ToString();
                receipt += "\nNumber of channels: " + channels;
                receipt += "\nApproximate cost: $" + estimatedCost;
            }
            else
            {
                receipt += "\nUnable to parse file header.";
            }

            receipt += "\nLocale: " + locale;

            // file storage
            var storageAccount = CloudStorageAccount.Parse(storageConnectionString);
            var fileClient = storageAccount.CreateCloudBlobClient();

            // Get a reference to the file share we created previously.
            var container = fileClient.GetContainerReference(containerName);
            log.LogInformation("Got container reference for container: " + containerName);

            var fileNameWithoutExtension = fileName.Replace(ext, string.Empty, StringComparison.OrdinalIgnoreCase);
            var blockBlob = container.GetBlockBlobReference(fileNameWithoutExtension + "_receipt.txt");

            await blockBlob.UploadTextAsync(receipt).ConfigureAwait(false);

            return new AudioDetails(createdTime, duration, channels, estimatedCost, locale);
        }

        public static (TimeSpan, int, double) GetAudioDetailsFromBytes(byte[] fileBytes, string fileNameExtension, ILogger log)
        {
            var duration = TimeSpan.Zero;
            var channels = 0;
            var estimatedCost = 0d;

            try
            {
                duration = AudioFileProcessor.GetDuration(fileBytes, fileNameExtension);

                channels = AudioFileProcessor.GetNumberOfChannels(fileBytes, fileNameExtension);

                estimatedCost = AudioFileProcessor.GetPayment(duration, channels);
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

            return (duration, channels, estimatedCost);
        }

        public static string GetContainerPathFromSAS(string sas, ILogger log)
        {
            return GetContainerPathFromSAS(new Uri(sas), log);
        }

        public static string GetContainerPathFromSAS(Uri uri, ILogger log)
        {
            if (uri == null)
            {
                throw new ArgumentNullException(nameof(uri));
            }

            string filePath = string.Empty;
            int count = 0;

            var path = uri.AbsolutePath.Split('/').SkipLast(1).Skip(1);
            foreach (string partPath in path)
            {
                count++;
                filePath += partPath;
                if (count < path.Count())
                {
                    filePath += "/";
                }
            }

            log.LogInformation("Path to container: " + filePath);
            return filePath;
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
