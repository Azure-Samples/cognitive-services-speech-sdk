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
        private BlobServiceClient BlobServiceClient;

        private string AccountName;

        private string AccountKey;

        public StorageConnector(string storageConnectionString)
        {
            if (storageConnectionString == null)
            {
                throw new ArgumentNullException(nameof(storageConnectionString));
            }

            AccountName = GetValueFromConnectionString("AccountName", storageConnectionString);
            AccountKey = GetValueFromConnectionString("AccountKey", storageConnectionString);

            BlobServiceClient = new BlobServiceClient(storageConnectionString);
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
            var blob = new BlobClient(new Uri(blobSas));

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

        public string CreateSas(Uri fileUri)
        {
            var containerName = GetContainerNameFromUri(fileUri);
            var fileName = GetFileNameFromUri(fileUri);
            var blobContainerClient = BlobServiceClient.GetBlobContainerClient(containerName);

            var sasBuilder = new BlobSasBuilder()
            {
                BlobContainerName = blobContainerClient.Name,
                BlobName = fileName,
                Resource = "b",
            };

            sasBuilder.StartsOn = DateTime.UtcNow.Subtract(TimeSpan.FromMinutes(10));
            sasBuilder.ExpiresOn = DateTime.UtcNow.AddDays(1);
            sasBuilder.SetPermissions(BlobContainerSasPermissions.Read);

            var sasToken = sasBuilder.ToSasQueryParameters(new StorageSharedKeyCredential(AccountName, AccountKey)).ToString();
            return $"{blobContainerClient.GetBlobClient(fileName).Uri}?{sasToken}";
        }

        public async Task WriteTextFileToBlobAsync(string content, string containerName, string fileName, ILogger log)
        {
            log.LogInformation($"Writing file {fileName} to container {containerName}.");
            var container = BlobServiceClient.GetBlobContainerClient(containerName);
            var blockBlob = container.GetBlobClient(fileName);

            using (var stream = new MemoryStream(Encoding.UTF8.GetBytes(content)))
            {
                await blockBlob.UploadAsync(stream).ConfigureAwait(false);
            }
        }

        public async Task MoveFileAsync(string inputContainerName,  string inputFileName, string outputContainerName, string outputFileName, ILogger log)
        {
            log.LogInformation($"Moving file {inputFileName} from container {inputContainerName} to {outputFileName} in container {outputContainerName}.");
            var inputContainerClient = BlobServiceClient.GetBlobContainerClient(inputContainerName);
            var inputBlockBlobClient = inputContainerClient.GetBlobClient(inputFileName);

            if (!await inputBlockBlobClient.ExistsAsync().ConfigureAwait(false))
            {
                log.LogError($"File {inputFileName} does not exist in container {inputContainerName}. Returning.");
                return;
            }

            var outputContainerClient = BlobServiceClient.GetBlobContainerClient(outputContainerName);
            var outputBlockBlobClient = outputContainerClient.GetBlobClient(outputFileName);

            await outputBlockBlobClient.StartCopyFromUriAsync(inputBlockBlobClient.Uri).ConfigureAwait(false);
            await inputBlockBlobClient.DeleteAsync().ConfigureAwait(false);
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
