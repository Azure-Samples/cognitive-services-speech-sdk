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

    public class StorageConnector : IStorageConnector
    {
        private readonly BlobServiceClient blobServiceClient;
        private readonly StorageSharedKeyCredential storageCredential;
        private readonly ILogger<StorageConnector> log;

        public StorageConnector(BlobServiceClient blobServiceClient, StorageSharedKeyCredential storageCredential, ILogger<StorageConnector> log)
        {
            this.blobServiceClient = blobServiceClient ?? throw new ArgumentNullException(nameof(blobServiceClient));
            this.storageCredential = storageCredential ?? throw new ArgumentNullException(nameof(storageCredential));
            this.log = log ?? throw new ArgumentNullException(nameof(log));
        }

        public string GetFileNameFromUri(Uri fileUri)
        {
            if (fileUri == null)
            {
                throw new ArgumentNullException(nameof(fileUri));
            }

            var pathParts = HttpUtility.UrlDecode(fileUri.AbsolutePath).Split('/').ToList();
            var cleanedPathParts = pathParts.SkipWhile(part => string.IsNullOrEmpty(part) || part.Equals("/", StringComparison.OrdinalIgnoreCase));
            return string.Join('/', cleanedPathParts.Skip(1));
        }

        public string GetContainerNameFromUri(Uri fileUri)
        {
            if (fileUri == null)
            {
                throw new ArgumentNullException(nameof(fileUri));
            }

            var pathParts = HttpUtility.UrlDecode(fileUri.AbsolutePath).Split('/').ToList();
            var cleanedPathParts = pathParts.SkipWhile(part => string.IsNullOrEmpty(part) || part.Equals("/", StringComparison.OrdinalIgnoreCase));
            return cleanedPathParts.FirstOrDefault();
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
            if (fileUri == null)
            {
                throw new ArgumentNullException(nameof(fileUri));
            }

            var containerName = this.GetContainerNameFromUri(fileUri);
            var fileName = this.GetFileNameFromUri(fileUri);
            var blobContainerClient = this.blobServiceClient.GetBlobContainerClient(containerName);

            var sasBuilder = new BlobSasBuilder
            {
                BlobContainerName = blobContainerClient.Name,
                BlobName = fileName,
                Resource = "b",
                StartsOn = DateTime.UtcNow.Subtract(TimeSpan.FromMinutes(10)),
                ExpiresOn = DateTime.UtcNow.AddDays(1)
            };
            sasBuilder.SetPermissions(BlobContainerSasPermissions.Read);

            var sasToken = sasBuilder.ToSasQueryParameters(this.storageCredential).ToString();
            return $"{blobContainerClient.GetBlobClient(fileName).Uri}?{sasToken}";
        }

        public async Task<string> WriteTextFileToBlobAsync(string content, string containerName, string fileName)
        {
            this.log.LogInformation($"Writing file {fileName} to container {containerName}.");
            var container = this.blobServiceClient.GetBlobContainerClient(containerName);
            var blockBlobClient = container.GetBlobClient(fileName);

            using var stream = new MemoryStream(Encoding.UTF8.GetBytes(content));
            await blockBlobClient.UploadAsync(stream, overwrite: true).ConfigureAwait(false);
            return blockBlobClient.Uri.AbsoluteUri;
        }

        public async Task MoveFileAsync(string inputContainerName, string inputFileName, string outputContainerName, string outputFileName, bool keepSource)
        {
            this.log.LogInformation($"Start moving file {inputFileName} from container {inputContainerName} to {outputFileName} in container {outputContainerName}.");

            var inputContainerClient = this.blobServiceClient.GetBlobContainerClient(inputContainerName);
            var inputBlockBlobClient = inputContainerClient.GetBlobClient(inputFileName);

            if (!await inputBlockBlobClient.ExistsAsync().ConfigureAwait(false))
            {
                this.log.LogError($"File {inputFileName} does not exist in container {inputContainerName}. Returning.");
                return;
            }

            var outputContainerClient = this.blobServiceClient.GetBlobContainerClient(outputContainerName);
            var outputBlockBlobClient = outputContainerClient.GetBlobClient(outputFileName);

            if (await outputBlockBlobClient.ExistsAsync().ConfigureAwait(false))
            {
                this.log.LogError($"File {outputFileName} already exists in container {outputContainerName}. Returning.");

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
    }
}
