// <copyright file="StorageConnector.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector
{
    using System;
    using System.IO;
    using System.Linq;
    using System.Threading.Tasks;
    using System.Web;
    using Azure.Storage;
    using Azure.Storage.Sas;
    using Microsoft.Extensions.Logging;
    using Microsoft.WindowsAzure.Storage;
    using Microsoft.WindowsAzure.Storage.Blob;

    public class StorageConnector
    {
        private CloudBlobClient CloudBlobClient;

        private string AccountName;

        private string AccountKey;

        public StorageConnector(string storageConnectionString)
        {
            if (storageConnectionString == null)
            {
                throw new ArgumentNullException(nameof(storageConnectionString));
            }

            var storageAccount = CloudStorageAccount.Parse(storageConnectionString);

            AccountName = GetValueFromConnectionString("AccountName", storageConnectionString);
            AccountKey = GetValueFromConnectionString("AccountKey", storageConnectionString);
            CloudBlobClient = storageAccount.CreateCloudBlobClient();
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

        public string CreateSas(Uri fileUri)
        {
            var containerName = GetContainerNameFromUri(fileUri);
            var fileName = GetFileNameFromUri(fileUri);
            var container = CloudBlobClient.GetContainerReference(containerName);

            var sasBuilder = new BlobSasBuilder()
            {
                BlobContainerName = container.Name,
                BlobName = fileName,
                Resource = "b",
            };

            sasBuilder.StartsOn = DateTime.UtcNow;
            sasBuilder.ExpiresOn = DateTime.UtcNow.AddDays(1);
            sasBuilder.SetPermissions(BlobContainerSasPermissions.Read);

            var sasToken = sasBuilder.ToSasQueryParameters(new StorageSharedKeyCredential(AccountName, AccountKey)).ToString();
            return $"{container.GetBlockBlobReference(fileName).Uri}?{sasToken}";
        }

        public async Task WriteTextFileToBlobAsync(string content, string containerName, string fileName, ILogger log)
        {
            log.LogInformation($"Writing file {fileName} to container {containerName}.");
            var container = CloudBlobClient.GetContainerReference(containerName);
            var blockBlob = container.GetBlockBlobReference(fileName);

            await blockBlob.UploadTextAsync(content).ConfigureAwait(false);
        }

        public async Task MoveFileAsync(string inputContainerName,  string inputFileName, string outputContainerName, string outputFileName, ILogger log)
        {
            log.LogInformation($"Moving file {inputFileName} from container {inputContainerName} to {outputFileName} in container {outputContainerName}.");
            var inputContainer = CloudBlobClient.GetContainerReference(inputContainerName);
            var inputBlockBlob = inputContainer.GetBlockBlobReference(inputFileName);

            if (!await inputBlockBlob.ExistsAsync().ConfigureAwait(false))
            {
                log.LogError($"File {inputFileName} does not exist in container {inputContainerName}. Returning.");
                return;
            }

            var outputContainer = CloudBlobClient.GetContainerReference(outputContainerName);
            var outputBlockBlob = outputContainer.GetBlockBlobReference(outputFileName);

            await outputBlockBlob.StartCopyAsync(inputBlockBlob).ConfigureAwait(false);
            await inputBlockBlob.DeleteAsync().ConfigureAwait(false);
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
