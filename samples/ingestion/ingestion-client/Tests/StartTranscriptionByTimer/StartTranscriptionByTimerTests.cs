// <copyright file="StartTranscriptionByTimerTests.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Tests
{
    using System;
    using System.Collections.Generic;
    using System.Threading;
    using System.Threading.Tasks;

    using Azure.Messaging.ServiceBus;
    using Connector.Enums;

    using Microsoft.Azure.Functions.Worker;
    using Microsoft.Extensions.Azure;
    using Microsoft.Extensions.Logging;
    using Microsoft.Extensions.Options;
    using Microsoft.VisualStudio.TestTools.UnitTesting;
    using Moq;
    using StartTranscriptionByTimer;

    [TestClass]
    public class StartTranscriptionByTimerTests
    {
        private readonly Mock<ILogger<StartTranscriptionByTimer>> mockLogger;

        // private readonly Mock<IStorageConnector> mockStorageConnector;
        private readonly Mock<IAzureClientFactory<ServiceBusClient>> mockServiceBusClientFactory;
        private readonly Mock<ServiceBusClient> mockServiceBusClient;
        private readonly Mock<ServiceBusReceiver> mockServiceBusReceiver;
        private readonly Mock<ServiceBusSender> mockServiceBusSender;
        private readonly IOptions<AppConfig> appConfigOptions;

        private readonly Mock<IStartTranscriptionHelper> mockTranscriptionHelper;

        public StartTranscriptionByTimerTests()
    {
        this.mockLogger = new Mock<ILogger<StartTranscriptionByTimer>>();

        // this.mockStorageConnector = new Mock<IStorageConnector>();
        this.mockServiceBusClientFactory = new Mock<IAzureClientFactory<ServiceBusClient>>();
        this.mockServiceBusClient = new Mock<ServiceBusClient>();
        this.mockServiceBusReceiver = new Mock<ServiceBusReceiver>();
        this.mockServiceBusSender = new Mock<ServiceBusSender>();
        this.mockTranscriptionHelper = new Mock<IStartTranscriptionHelper>();

        var appConfig = new AppConfig
        {
            AzureWebJobsStorage = "UseDevelopmentStorage=true",
            StartTranscriptionServiceBusConnectionString = "Endpoint=sb://test.servicebus.windows.net/;SharedAccessKeyName=RootManageSharedAccessKey;SharedAccessKey=testkey=",
            FetchTranscriptionServiceBusConnectionString = "Endpoint=sb://test.servicebus.windows.net/;SharedAccessKeyName=RootManageSharedAccessKey;SharedAccessKey=testkey=",
            MessagesPerFunctionExecution = 5,
            Locale = "en-US | English (United States)",
        };
        this.appConfigOptions = Options.Create(appConfig);

        this.mockServiceBusClientFactory
            .Setup(f => f.CreateClient(ServiceBusClientName.StartTranscriptionServiceBusClient.ToString()))
            .Returns(this.mockServiceBusClient.Object);

        this.mockServiceBusClientFactory
            .Setup(f => f.CreateClient(ServiceBusClientName.FetchTranscriptionServiceBusClient.ToString()))
            .Returns(this.mockServiceBusClient.Object);

        this.mockServiceBusClient
            .Setup(c => c.CreateReceiver(It.IsAny<string>()))
            .Returns(this.mockServiceBusReceiver.Object);

        this.mockServiceBusClient
            .Setup(c => c.CreateSender(It.IsAny<string>()))
            .Returns(this.mockServiceBusSender.Object);
    }

        [TestMethod]
        public async Task FunctionRunNoMessagesLogsNoMessages()
        {
            // Arrange
            var function = new StartTranscriptionByTimer(
                this.mockLogger.Object,
                this.appConfigOptions,
                this.mockServiceBusClientFactory.Object,
                this.mockTranscriptionHelper.Object);

            var timerInfo = new TimerInfo
            {
                ScheduleStatus = new ScheduleStatus
                {
                    Last = DateTime.UtcNow,
                    Next = DateTime.UtcNow.AddMinutes(5)
                }
            };

            this.mockServiceBusReceiver
                .Setup(r => r.ReceiveMessagesAsync(It.IsAny<int>(), It.IsAny<TimeSpan>(), It.IsAny<CancellationToken>()))
                .ReturnsAsync(new List<ServiceBusReceivedMessage>());

            // Act
            await function.Run(timerInfo);

            // Assert
            this.mockLogger.Verify(
                x => x.Log(
                    LogLevel.Information,
                    It.IsAny<EventId>(),
                    It.Is<It.IsAnyType>((v, t) => v.ToString().Contains("Got no messages in this iteration.")),
                    null,
                    It.IsAny<Func<It.IsAnyType, Exception, string>>()),
                Times.Once);
        }

        [TestMethod]
        public async Task FunctionRunValidMessagesLogsPulledValidMessages()
        {
            // Arrange
            var function = new StartTranscriptionByTimer(
                this.mockLogger.Object,
                this.appConfigOptions,
                this.mockServiceBusClientFactory.Object,
                this.mockTranscriptionHelper.Object);

            var timerInfo = new TimerInfo
            {
                ScheduleStatus = new ScheduleStatus
                {
                    Last = DateTime.UtcNow,
                    Next = DateTime.UtcNow.AddMinutes(5)
                }
            };

            var messages = new List<ServiceBusReceivedMessage>
            {
                ServiceBusModelFactory.ServiceBusReceivedMessage(
                    messageId: "1",
                    lockedUntil: DateTimeOffset.UtcNow.AddMinutes(1),
                    body: new BinaryData("EventType: TranscriptionStarted")),
            };
            this.mockServiceBusReceiver
                .Setup(r => r.ReceiveMessagesAsync(It.IsAny<int>(), It.IsAny<TimeSpan>(), It.IsAny<CancellationToken>()))
                .ReturnsAsync(messages);

            this.mockTranscriptionHelper
                .Setup(t => t.IsValidServiceBusMessage(It.IsAny<ServiceBusReceivedMessage>()))
                .Returns(true);
            this.mockTranscriptionHelper
                .Setup(t => t.StartTranscriptionsAsync(It.IsAny<List<ServiceBusReceivedMessage>>(), It.IsAny<DateTime>()))
                .Returns(Task.CompletedTask);

            // Act
            await function.Run(timerInfo);

            // Assert
            this.mockLogger.Verify(
                x => x.Log(
                    LogLevel.Information,
                    It.IsAny<EventId>(),
                    It.Is<It.IsAnyType>((v, t) => v.ToString().Contains("Pulled 1 valid messages from queue.")),
                    null,
                    It.IsAny<Func<It.IsAnyType, Exception, string>>()),
                Times.Once);
        }

        [TestMethod]
        public async Task FunctionRunInvalidMessagesLogsPulledNoValidMessages()
        {
            // Arrange
            var function = new StartTranscriptionByTimer(
                this.mockLogger.Object,
                this.appConfigOptions,
                this.mockServiceBusClientFactory.Object,
                this.mockTranscriptionHelper.Object);

            var timerInfo = new TimerInfo
            {
                ScheduleStatus = new ScheduleStatus
                {
                    Last = DateTime.UtcNow,
                    Next = DateTime.UtcNow.AddMinutes(5)
                }
            };

            var messages = new List<ServiceBusReceivedMessage>
            {
                ServiceBusModelFactory.ServiceBusReceivedMessage(
                    messageId: "1",
                    lockedUntil: DateTimeOffset.UtcNow.AddMinutes(1),
                    body: new BinaryData("EventType: TranscriptionStarted")),
            };
            this.mockServiceBusReceiver
                .Setup(r => r.ReceiveMessagesAsync(It.IsAny<int>(), It.IsAny<TimeSpan>(), It.IsAny<CancellationToken>()))
                .ReturnsAsync(messages);

            this.mockTranscriptionHelper
                .Setup(t => t.IsValidServiceBusMessage(It.IsAny<ServiceBusReceivedMessage>()))
                .Returns(false);
            this.mockTranscriptionHelper
                .Setup(t => t.StartTranscriptionsAsync(It.IsAny<List<ServiceBusReceivedMessage>>(), It.IsAny<DateTime>()))
                .Returns(Task.CompletedTask);

            // Act
            await function.Run(timerInfo);

            // Assert
            this.mockLogger.Verify(
                x => x.Log(
                    LogLevel.Information,
                    It.IsAny<EventId>(),
                    It.Is<It.IsAnyType>((v, t) => v.ToString().Contains("No valid messages were found in this function execution.")),
                    null,
                    It.IsAny<Func<It.IsAnyType, Exception, string>>()),
                Times.Once);
        }
    }
}