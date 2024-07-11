// <copyright file="StorageConnectorTests.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Tests
{
    using System;
    using System.IO;
    using System.Text;
    using System.Threading;

    using System.Threading.Tasks;

    using Azure;
    using Azure.Storage;

    using Azure.Storage.Blobs;
    using Azure.Storage.Blobs.Models;
    using Azure.Storage.Sas;
    using Connector;

    using Microsoft.Extensions.Logging;
    using Microsoft.VisualStudio.TestTools.UnitTesting;

    using Moq;

    [TestClass]
    public class StorageConnectorTests
    {
        private readonly string testUri = "https://contoso.blob.core.windows.net/testContainer/testfolder/test.wav";
        private Mock<BlobServiceClient> mockBlobServiceClient;
        private Mock<StorageSharedKeyCredential> mockStorageCredential;
        private Mock<ILogger<StorageConnector>> mockLogger;
        private StorageConnector storageConnector;

        [TestInitialize]
        public void Setup()
        {
            this.mockBlobServiceClient = new Mock<BlobServiceClient>();
            this.mockStorageCredential = new Mock<StorageSharedKeyCredential>("accountName", Convert.ToBase64String(Encoding.UTF8.GetBytes("FakeAccountKey")));
            this.mockLogger = new Mock<ILogger<StorageConnector>>();
            this.storageConnector = new StorageConnector(this.mockBlobServiceClient.Object, this.mockStorageCredential.Object, this.mockLogger.Object);
        }

        [TestMethod]
        [ExpectedException(typeof(ArgumentNullException))]
        public void GetFileNameFromUriWithNullUriThrowsArgumentNullException()
        {
            this.storageConnector.GetFileNameFromUri(null);
        }

        [TestMethod]
        public void GetFileNameFromUriWithValidUriReturnsFileName()
        {
            // Arrange
            var uri = new Uri(this.testUri);

            // Act
            var result = this.storageConnector.GetFileNameFromUri(uri);

            // Assert
            Assert.AreEqual("testfolder/test.wav", result);
        }

        [TestMethod]
        [ExpectedException(typeof(ArgumentNullException))]
        public void GetContainerNameFromUriWithNullUriThrowsArgumentNullException()
        {
            this.storageConnector.GetContainerNameFromUri(null);
        }

        [TestMethod]
        public void GetContainerNameFromUriWithValidUriReturnsContainerName()
        {
            // Arrange
            var uri = new Uri(this.testUri);

            // Act
            var result = this.storageConnector.GetContainerNameFromUri(uri);

            // Assert
            Assert.AreEqual("testContainer", result);
        }

        [TestMethod]
        public void CreateSasValidUriReturnsSasUri()
        {
            // Arrange
            var uri = new Uri(this.testUri);
            var containerClient = new Mock<BlobContainerClient>();
            var blobClient = new Mock<BlobClient>();
            var sasBuilder = new Mock<BlobSasBuilder>();
            var containerName = "testContainer";
            var blobName = "testfolder/test.wav";

            this.mockBlobServiceClient
                .Setup(c => c.GetBlobContainerClient(containerName))
                .Returns(containerClient.Object);

            containerClient
                .Setup(c => c.GetBlobClient(blobName))
                .Returns(blobClient.Object);

            blobClient
                .Setup(b => b.Uri)
                .Returns(new Uri($"https://contoso.blob.core.windows.net/{containerName}/{blobName}"));

            // Act
            var result = this.storageConnector.CreateSas(uri);

            // Assert
            Assert.IsTrue(result.Contains(this.testUri + "?", StringComparison.OrdinalIgnoreCase));
        }

        [TestMethod]
        public async Task WriteTextFileToBlobAsyncWithValidInputsReturnsBlobUri()
        {
            // Arrange
            var containerName = "testContainer";
            var fileName = "testfolder/test.wav";
            var content = "test content";
            var mockContainerClient = new Mock<BlobContainerClient>();
            var mockBlobClient = new Mock<BlobClient>();
            var expectedUri = new Uri(this.testUri);

            this.mockBlobServiceClient
                .Setup(c => c.GetBlobContainerClient(containerName))
                .Returns(mockContainerClient.Object);

            mockContainerClient
                .Setup(c => c.GetBlobClient(fileName))
                .Returns(mockBlobClient.Object);

            mockBlobClient
                .Setup(b => b.UploadAsync(It.IsAny<MemoryStream>(), true, default))
                .Returns(Task.FromResult(Response.FromValue(It.IsAny<BlobContentInfo>(), null)));

            mockBlobClient
                .Setup(b => b.Uri)
                .Returns(expectedUri);

            // Act
            var result = await this.storageConnector.WriteTextFileToBlobAsync(content, containerName, fileName);

            // Assert
            Assert.AreEqual(expectedUri.AbsoluteUri, result);
        }

        [TestMethod]
        public async Task MoveFileAsyncWithValidInputsMovesFile()
        {
            // Arrange
            var inputContainerName = "input-container";
            var inputFileName = "input-file.txt";
            var outputContainerName = "output-container";
            var outputFileName = "output-file.txt";
            var mockInputContainerClient = new Mock<BlobContainerClient>();
            var mockInputBlobClient = new Mock<BlobClient>();
            var mockOutputContainerClient = new Mock<BlobContainerClient>();
            var mockOutputBlobClient = new Mock<BlobClient>();

            this.mockBlobServiceClient
                .Setup(c => c.GetBlobContainerClient(inputContainerName))
                .Returns(mockInputContainerClient.Object);

            mockInputContainerClient
                .Setup(c => c.GetBlobClient(inputFileName))
                .Returns(mockInputBlobClient.Object);

            this.mockBlobServiceClient
                .Setup(c => c.GetBlobContainerClient(outputContainerName))
                .Returns(mockOutputContainerClient.Object);

            mockOutputContainerClient
                .Setup(c => c.GetBlobClient(outputFileName))
                .Returns(mockOutputBlobClient.Object);

            mockInputBlobClient
                .Setup(b => b.ExistsAsync(It.IsAny<CancellationToken>()))
                .ReturnsAsync(Response.FromValue<bool>(true, null));

            mockOutputBlobClient
                .Setup(b => b.ExistsAsync(It.IsAny<CancellationToken>()))
                .ReturnsAsync(Response.FromValue<bool>(false, null));

            mockOutputBlobClient
                .Setup(b => b.StartCopyFromUriAsync(null, null, null, null, null, null, It.IsAny<CancellationToken>()))
                .ReturnsAsync(Response.FromValue<CopyFromUriOperation>(null, null));

            mockInputBlobClient
                .Setup(b => b.DeleteIfExistsAsync(DeleteSnapshotsOption.None, null, default))
                .ReturnsAsync(Response.FromValue<bool>(true, null));

            // Act
            await this.storageConnector.MoveFileAsync(inputContainerName, inputFileName, outputContainerName, outputFileName, false);

            // Assert
            mockOutputBlobClient.Verify(b => b.StartCopyFromUriAsync(null, null, null, null, null, null, It.IsAny<CancellationToken>()), Times.Once);
            mockInputBlobClient.Verify(b => b.DeleteIfExistsAsync(DeleteSnapshotsOption.None, null, default), Times.Once);
        }
    }
}