// <copyright file="StorageUtilities.cs" company="Microsoft Corporation">
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

    public static class StorageUtilities
    {
        public static async Task<string> CreateSASAsync(string storageConnectionString, Uri fileUri, ILogger log)
        {
            var containerName = GetContainerNameFromUri(fileUri);
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
            var absoluteSasUri = fileUri.AbsoluteUri + sas;

            log.LogInformation($"Created sas url for file {fileUri.AbsoluteUri}");
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
    }
}
