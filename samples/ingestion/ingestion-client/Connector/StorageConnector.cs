// <copyright file="StorageConnector.cs" company="Microsoft Corporation">
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
    using System.Web;
    using Azure.Storage;
    using Azure.Storage.Blobs;
    using Azure.Storage.Sas;
    using Microsoft.Extensions.Logging;

    public class StorageConnector
    {
        private readonly BlobServiceClient blobServiceClient;

        private readonly string accountName;

        private readonly string accountKey;

        public StorageConnector(string storageConnectionString)
        {
            if (storageConnectionString == null)
            {
                throw new ArgumentNullException(nameof(storageConnectionString));
            }

            this.accountName = GetValueFromConnectionString("AccountName", storageConnectionString);
            this.accountKey = GetValueFromConnectionString("AccountKey", storageConnectionString);

            this.blobServiceClient = new BlobServiceClient(storageConnectionString);
        }

        public static string GetFileNameFromUri(Uri fileUri)
        {
            if (fileUri == null)
            {
                throw new ArgumentNullException(nameof(fileUri));
            }

            var pathParts = HttpUtility.UrlDecode(fileUri.AbsolutePath).Split('/').ToList();
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

        public static string GetContainerNameFromUri(Uri fileUri)
        {
            if (fileUri == null)
            {
                throw new ArgumentNullException(nameof(fileUri));
            }

            var pathParts = HttpUtility.UrlDecode(fileUri.AbsolutePath).Split('/').ToList();
            var cleanedPathParts = pathParts.SkipWhile(part => string.IsNullOrEmpty(part) || part.Equals("/", StringComparison.OrdinalIgnoreCase));

            var containerName = cleanedPathParts.FirstOrDefault();
            return containerName;
        }

        public static async Task<byte[]> DownloadFileFromSAS(string blobSas)
        {
            var blobClientOptions = new BlobClientOptions()
            {
                Retry =
                {
                    MaxRetries = 3,
                    Delay = TimeSpan.FromSeconds(5),
                    Mode = Azure.Core.RetryMode.Fixed
                }
            };

            var blob = new BlobClient(new Uri(blobSas), blobClientOptions);

            byte[] data;

            using (var memoryStream = new MemoryStream())
            {
                await blob.DownloadToAsync(memoryStream).ConfigureAwait(false);
                data = new byte[memoryStream.Length];
                memoryStream.Position = 0;
                memoryStream.Read(data, 0, data.Length);
            }

            return data;
        }

        public async Task<byte[]> DownloadFileFromContainer(string containerName, string blobName)
        {
            var containerClient = this.blobServiceClient.GetBlobContainerClient(containerName);

            var blobClient = containerClient.GetBlobClient(blobName);

            using var memoryStream = new MemoryStream();
            await blobClient.DownloadToAsync(memoryStream).ConfigureAwait(false);
            return memoryStream.ToArray();
        }

        public string CreateSas(Uri fileUri)
        {
            var containerName = GetContainerNameFromUri(fileUri);
            var fileName = GetFileNameFromUri(fileUri);
            var blobContainerClient = this.blobServiceClient.GetBlobContainerClient(containerName);

            var sasBuilder = new BlobSasBuilder()
            {
                BlobContainerName = blobContainerClient.Name,
                BlobName = fileName,
                Resource = "b",
            };

            sasBuilder.StartsOn = DateTime.UtcNow.Subtract(TimeSpan.FromMinutes(10));
            sasBuilder.ExpiresOn = DateTime.UtcNow.AddDays(1);
            sasBuilder.SetPermissions(BlobContainerSasPermissions.Read);

            var sasToken = sasBuilder.ToSasQueryParameters(new StorageSharedKeyCredential(this.accountName, this.accountKey)).ToString();
            return $"{blobContainerClient.GetBlobClient(fileName).Uri}?{sasToken}";
        }

        public async Task WriteTextFileToBlobAsync(string content, string containerName, string fileName, ILogger log)
        {
            log.LogInformation($"Writing file {fileName} to container {containerName}.");
            var container = this.blobServiceClient.GetBlobContainerClient(containerName);
            var blockBlobClient = container.GetBlobClient(fileName);

            using var stream = new MemoryStream(Encoding.UTF8.GetBytes(content));
            await blockBlobClient.UploadAsync(stream, overwrite: true).ConfigureAwait(false);
        }

        public async Task MoveFileAsync(string inputContainerName,  string inputFileName, string outputContainerName, string outputFileName, bool keepSource, ILogger log)
        {
            log.LogInformation($"Start moving file {inputFileName} from container {inputContainerName} to {outputFileName} in container {outputContainerName}.");

            var inputContainerClient = this.blobServiceClient.GetBlobContainerClient(inputContainerName);
            var inputBlockBlobClient = inputContainerClient.GetBlobClient(inputFileName);

            if (!await inputBlockBlobClient.ExistsAsync().ConfigureAwait(false))
            {
                log.LogError($"File {inputFileName} does not exist in container {inputContainerName}. Returning.");
                return;
            }

            var outputContainerClient = this.blobServiceClient.GetBlobContainerClient(outputContainerName);
            var outputBlockBlobClient = outputContainerClient.GetBlobClient(outputFileName);

            if (await outputBlockBlobClient.ExistsAsync().ConfigureAwait(false))
            {
                log.LogError($"File {outputFileName} already exists in container {outputContainerName}. Returning.");

                if (!keepSource)
                {
                    await inputBlockBlobClient.DeleteIfExistsAsync().ConfigureAwait(false);
                }

                return;
            }

            await outputBlockBlobClient.StartCopyFromUriAsync(inputBlockBlobClient.Uri).ConfigureAwait(false);

            if (!keepSource)
            {
                await inputBlockBlobClient.DeleteIfExistsAsync().ConfigureAwait(false);
            }
        }

        private static string GetValueFromConnectionString(string key, string connectionString)
        {
            var split = connectionString.Split(';');

            foreach (var subConnectionString in split)
            {
                if (subConnectionString.StartsWith(key, StringComparison.OrdinalIgnoreCase))
                {
                    return subConnectionString.Substring(key.Length + 1);
                }
            }

            return string.Empty;
        }
    }
}
