// <copyright file="IStorageConnector.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector
{
    using System;
    using System.Threading.Tasks;

    public interface IStorageConnector
    {
        string GetFileNameFromUri(Uri fileUri);

        string GetContainerNameFromUri(Uri fileUri);

        Task<byte[]> DownloadFileFromContainer(string containerName, string blobName);

        string CreateSas(Uri fileUri);

        Task<string> WriteTextFileToBlobAsync(string content, string containerName, string fileName);

        Task MoveFileAsync(string inputContainerName, string inputFileName, string outputContainerName, string outputFileName, bool keepSource);
    }
}
