// <copyright file="EndToEndTests.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Tests
{
    using System;
    using System.Collections.Generic;
    using System.IO;
    using System.Linq;
    using System.Threading.Tasks;
    using Connector;
    using Connector.Serializable.Language.Conversations;
    using Connector.Serializable.TranscriptionStartedServiceBusMessage;

    using FetchTranscription;

    using Microsoft.Extensions.Logging;
    using Microsoft.Extensions.Options;
    using Microsoft.VisualStudio.TestTools.UnitTesting;
    using Moq;
    using Newtonsoft.Json;

    [TestClass]
    public class EndToEndTests
    {
        private readonly Mock<ILogger> logger;

        private readonly IOptions<AppConfig> appConfigOptions;

        public EndToEndTests()
        {
            this.logger = new Mock<ILogger>();
            var appConfig = new AppConfig
            {
                AzureWebJobsStorage = "UseDevelopmentStorage=true",
                StartTranscriptionServiceBusConnectionString = "Endpoint=sb://test.servicebus.windows.net/;SharedAccessKeyName=RootManageSharedAccessKey;SharedAccessKey=testkey=",
                FetchTranscriptionServiceBusConnectionString = "Endpoint=sb://test.servicebus.windows.net/;SharedAccessKeyName=RootManageSharedAccessKey;SharedAccessKey=testkey=",
                TextAnalyticsKey = "sometestkey",
                TextAnalyticsEndpoint = "https://sometestendpoint",
            };
            this.appConfigOptions = Options.Create(appConfig);
        }

        [TestMethod]
        [TestCategory(TestCategories.EndToEndTest)]
        public async Task AnalyzeConversationTestAsync()
        {
            Console.WriteLine(System.Text.Json.JsonSerializer.Serialize(new ConversationSummarizationOptions
            {
                Aspects = new HashSet<Aspect> { Aspect.Issue, Aspect.Resolution, Aspect.ChapterTitle, Aspect.Narrative },
                Stratergy = new RoleAssignmentStratergy
                {
                    Key = RoleAssignmentMappingKey.Channel,
                    Mapping = new Dictionary<int, Role> { { 0, Role.Agent }, { 1, Role.Customer } },
                    FallbackRole = Role.None,
                }
            }));

            var provider = new AnalyzeConversationsProvider("en-US", this.appConfigOptions.Value.TextAnalyticsKey, this.appConfigOptions.Value.TextAnalyticsEndpoint, this.logger.Object, this.appConfigOptions);
            var body = File.ReadAllText(@"TestFiles/summarizationInputSample.json");
            var transcription = JsonConvert.DeserializeObject<SpeechTranscript>(body);

            var speechTranscriptMapping = new Dictionary<AudioFileInfo, SpeechTranscript>
            {
                { new AudioFileInfo("someUrl", 0, null, "someFile"), transcription }
            };

            var errors = await provider.SubmitTranscriptionAnalyticsJobsAsync(speechTranscriptMapping).ConfigureAwait(false);
            Console.WriteLine("Submit");
            Assert.AreEqual(0, errors.Count());

            while ((await provider.GetTranscriptionAnalyticsJobStatusAsync(speechTranscriptMapping.Keys).ConfigureAwait(false)) == Connector.Enums.TranscriptionAnalyticsJobStatus.Running)
            {
                await Task.Delay(TimeSpan.FromSeconds(10)).ConfigureAwait(false);
                Console.WriteLine($"[{DateTime.Now}]jobs are running...");
            }

            Console.WriteLine($"[{DateTime.Now}]jobs done.");

            var err = await provider.AddTranscriptionAnalyticsResultsToTranscriptsAsync(speechTranscriptMapping);
            Console.WriteLine($"annotation result: {JsonConvert.SerializeObject(transcription)}");
            Assert.AreEqual(0, err.Count());
            Assert.AreEqual(4, transcription.ConversationAnalyticsResults.AnalyzeConversationSummarizationResults.Conversations.First().Summaries.Count());
        }
    }
}
