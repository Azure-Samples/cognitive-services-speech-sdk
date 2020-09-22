// <copyright file="PostTranscriptionServiceBusMessage.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector
{
    using System;
    using Newtonsoft.Json;

    public class PostTranscriptionServiceBusMessage
    {
        public PostTranscriptionServiceBusMessage(
            Subscription subscription,
            string transcriptionLocation,
            string jobName,
            string createdTime,
            string locale,
            bool usesCustomModel,
            bool addSentimentAnalysis,
            bool addEntityRedaction,
            int retryCount,
            int reenqueueingTimeInSeconds,
            int initialDelayInSeconds)
        {
            Subscription = subscription;
            TranscriptionLocation = transcriptionLocation;
            JobName = jobName;
            CreatedTime = createdTime;
            Locale = locale;
            UsesCustomModel = usesCustomModel;
            AddSentimentAnalysis = addSentimentAnalysis;
            AddEntityRedaction = addEntityRedaction;
            RetryCount = retryCount;
            ReenqueueingTimeInSeconds = reenqueueingTimeInSeconds;
            InitialDelayInSeconds = initialDelayInSeconds;
        }

        public string TranscriptionLocation { get; set; }

        public string JobName { get; set; }

        public int RetryCount { get; set; }

        public Subscription Subscription { get; set; }

        public string CreatedTime { get; set; }

        public string Locale { get; set; }

        public bool UsesCustomModel { get; set; }

        public bool AddSentimentAnalysis { get; set; }

        public bool AddEntityRedaction { get; set; }

        public int ReenqueueingTimeInSeconds { get; set; }

        public int InitialDelayInSeconds { get; set; }

        public static PostTranscriptionServiceBusMessage DeserializeMessage(string serviceBusMessage)
        {
            if (serviceBusMessage == null)
            {
                throw new ArgumentNullException(nameof(serviceBusMessage));
            }

            return JsonConvert.DeserializeObject<PostTranscriptionServiceBusMessage>(serviceBusMessage);
        }

        public string CreateMessageString()
        {
            return JsonConvert.SerializeObject(this);
        }

        public PostTranscriptionServiceBusMessage RetryMessage()
        {
            return new PostTranscriptionServiceBusMessage(
                Subscription,
                TranscriptionLocation,
                JobName,
                CreatedTime,
                Locale,
                UsesCustomModel,
                AddSentimentAnalysis,
                AddEntityRedaction,
                RetryCount + 1,
                ReenqueueingTimeInSeconds,
                InitialDelayInSeconds);
        }
    }
}
