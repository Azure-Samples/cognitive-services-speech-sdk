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
    using Microsoft.Extensions.Logging;
    using Microsoft.WindowsAzure.Storage;
    using Microsoft.WindowsAzure.Storage.Blob;

    public class StorageConnector
    {
        private CloudBlobClient CloudBlobClient;

        public StorageConnector(string storageConnectionString)
        {
            var storageAccount = CloudStorageAccount.Parse(storageConnectionString);
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

        public async Task<string> CreateSASAsync(Uri fileUri)
        {
            var containerName = GetContainerNameFromUri(fileUri);
            var container = CloudBlobClient.GetContainerReference(containerName);

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
            var absoluteSasUri = fileUri.AbsoluteUri + sas;

            return absoluteSasUri;
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
    }
}
