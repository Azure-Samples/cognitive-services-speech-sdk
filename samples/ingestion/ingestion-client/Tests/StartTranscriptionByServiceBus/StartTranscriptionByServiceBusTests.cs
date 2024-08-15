// <copyright file="StartTranscriptionByServiceBusTests.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Tests
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Threading.Tasks;

    using Azure.Messaging.ServiceBus;

    using Microsoft.Extensions.Logging;
    using Microsoft.VisualStudio.TestTools.UnitTesting;

    using Moq;
    using StartTranscription;

    using StartTranscriptionByTimer;

    [TestClass]
    public class StartTranscriptionByServiceBusTests
    {
        private readonly Mock<ILogger<StartTranscriptionByServiceBus>> mockLogger;
        private readonly Mock<IStartTranscriptionHelper> mockTranscriptionHelper;
        private readonly StartTranscriptionByServiceBus startTranscriptionByServiceBus;

        public StartTranscriptionByServiceBusTests()
        {
            this.mockLogger = new Mock<ILogger<StartTranscriptionByServiceBus>>();
            this.mockTranscriptionHelper = new Mock<IStartTranscriptionHelper>();
            this.startTranscriptionByServiceBus = new StartTranscriptionByServiceBus(
                this.mockLogger.Object,
                this.mockTranscriptionHelper.Object);
        }

        [TestMethod]
        public async Task FunctionRunValidMessageCallsStartTranscriptionAsync()
        {
            // Arrange
            var messages = new List<ServiceBusReceivedMessage>
            {
                ServiceBusModelFactory.ServiceBusReceivedMessage(
                    messageId: "1",
                    subject: "TestSubject",
                    sequenceNumber: 123,
                    lockedUntil: DateTimeOffset.UtcNow.AddMinutes(1),
                    body: new BinaryData("EventType: TranscriptionStarted")),
            };

            this.mockTranscriptionHelper.Setup(h => h.IsValidServiceBusMessage(It.IsAny<ServiceBusReceivedMessage>()))
                                        .Returns(true);

            // Act
            await this.startTranscriptionByServiceBus.Run(messages.First());

            // Assert
            this.mockLogger.Verify(
                logger => logger.Log(
                It.Is<LogLevel>(logLevel => logLevel == LogLevel.Information),
                It.IsAny<EventId>(),
                It.Is<It.IsAnyType>((v, t) => v.ToString().Contains("C# Isolated ServiceBus queue trigger function processed message: TestSubject")),
                It.IsAny<Exception>(),
                It.Is<Func<It.IsAnyType, Exception, string>>((v, t) => true)),
                Times.Once);

            this.mockLogger.Verify(
                logger => logger.Log(
                It.Is<LogLevel>(logLevel => logLevel == LogLevel.Information),
                It.IsAny<EventId>(),
                It.Is<It.IsAnyType>((v, t) => v.ToString().Contains("Received message: SequenceNumber:123 Body:EventType: TranscriptionStarted")),
                It.IsAny<Exception>(),
                It.Is<Func<It.IsAnyType, Exception, string>>((v, t) => true)),
                Times.Once);

            this.mockTranscriptionHelper.Verify(helper => helper.StartTranscriptionAsync(It.IsAny<ServiceBusReceivedMessage>()), Times.Once);
        }

        [TestMethod]
        public async Task FunctionRunInvalidMessageLogsInvalidMessageAndDoesNotCallTranscription()
        {
            // Arrange
            var messages = new List<ServiceBusReceivedMessage>
            {
                ServiceBusModelFactory.ServiceBusReceivedMessage(
                    messageId: "1",
                    subject: "TestSubject",
                    sequenceNumber: 123,
                    lockedUntil: DateTimeOffset.UtcNow.AddMinutes(1),
                    body: new BinaryData("EventType: TranscriptionStarted")),
            };

            this.mockTranscriptionHelper.Setup(h => h.IsValidServiceBusMessage(It.IsAny<ServiceBusReceivedMessage>()))
                                        .Returns(false);

            // Act
            await this.startTranscriptionByServiceBus.Run(messages.First());

            // Assert
            this.mockLogger.Verify(
                logger => logger.Log(
                It.Is<LogLevel>(logLevel => logLevel == LogLevel.Information),
                It.IsAny<EventId>(),
                It.Is<It.IsAnyType>((v, t) => v.ToString().Contains("C# Isolated ServiceBus queue trigger function processed message: TestSubject")),
                It.IsAny<Exception>(),
                It.Is<Func<It.IsAnyType, Exception, string>>((v, t) => true)),
                Times.Once);

            this.mockLogger.Verify(
                logger => logger.Log(
                It.Is<LogLevel>(logLevel => logLevel == LogLevel.Information),
                It.IsAny<EventId>(),
                It.Is<It.IsAnyType>((v, t) => v.ToString().Contains("Received message: SequenceNumber:123 Body:EventType: TranscriptionStarted")),
                It.IsAny<Exception>(),
                It.Is<Func<It.IsAnyType, Exception, string>>((v, t) => true)),
                Times.Once);

            this.mockLogger.Verify(
                logger => logger.Log(
                It.Is<LogLevel>(logLevel => logLevel == LogLevel.Information),
                It.IsAny<EventId>(),
                It.Is<It.IsAnyType>((v, t) => v.ToString().Contains("Service bus message is invalid.")),
                It.IsAny<Exception>(),
                It.Is<Func<It.IsAnyType, Exception, string>>((v, t) => true)),
                Times.Once);

            this.mockTranscriptionHelper.Verify(helper => helper.StartTranscriptionAsync(It.IsAny<ServiceBusReceivedMessage>()), Times.Never);
        }

        [TestMethod]
        public async Task FunctionRunNullMessageThrowsArgumentNullException()
        {
            // Arrange
            ServiceBusReceivedMessage message = null;

            // Act & Assert
            await Assert.ThrowsExceptionAsync<ArgumentNullException>(() => this.startTranscriptionByServiceBus.Run(message));
        }
    }
}