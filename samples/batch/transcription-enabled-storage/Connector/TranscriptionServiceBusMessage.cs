// <copyright file="TranscriptionServiceBusMessage.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector
{
    using System;
    using System.Globalization;

    public class TranscriptionServiceBusMessage
    {
        public TranscriptionServiceBusMessage(
            Subscription subscription,
            string transcriptionLocation,
            string blobName,
            string audioFileUrl,
            string createdTime,
            string audioLength,
            int channels,
            double estimatedCost,
            string locale,
            int retryCount,
            int reenqueueingTimeInSeconds)
        {
            Subscription = subscription;
            TranscriptionLocation = transcriptionLocation;
            BlobName = blobName;
            AudioFileUrl = audioFileUrl;
            CreatedTime = createdTime;
            AudioLength = audioLength;
            Channels = channels;
            EstimatedCost = estimatedCost;
            Locale = locale;
            RetryCount = retryCount;
            ReenqueueingTimeInSeconds = reenqueueingTimeInSeconds;
        }

        public string TranscriptionLocation { get; }

        public string BlobName { get; }

        public string AudioFileUrl { get; }

        public int RetryCount { get; }

        public int ReenqueueingTimeInSeconds { get; }

        public Subscription Subscription { get; }

        public string CreatedTime { get; set; }

        public string AudioLength { get; set; }

        public int Channels { get; set; }

        public double EstimatedCost { get; set; }

        public string Locale { get; set; }

        public static TranscriptionServiceBusMessage ParseMessageFromString(string serviceBusMessage)
        {
            if (serviceBusMessage == null)
            {
                throw new ArgumentNullException(nameof(serviceBusMessage));
            }

            var messageElements = serviceBusMessage.Split("\n");
            return new TranscriptionServiceBusMessage(
                new Subscription(
                    messageElements[0],
                    new Uri(messageElements[1])),
                messageElements[2],
                messageElements[3],
                messageElements[4],
                messageElements[5],
                messageElements[6],
                int.Parse(messageElements[7], CultureInfo.InvariantCulture),
                double.Parse(messageElements[8], CultureInfo.InvariantCulture),
                messageElements[9],
                int.Parse(messageElements[10], CultureInfo.InvariantCulture),
                int.Parse(messageElements[11], CultureInfo.InvariantCulture));
        }

        public string CreateMessageString()
        {
            return $"{Subscription.SubscriptionKey}" +
                $"\n{Subscription.LocationUri.AbsoluteUri}" +
                $"\n{TranscriptionLocation}" +
                $"\n{BlobName}" +
                $"\n{AudioFileUrl}" +
                $"\n{CreatedTime}" +
                $"\n{AudioLength}" +
                $"\n{Channels}" +
                $"\n{EstimatedCost}" +
                $"\n{Locale}" +
                $"\n{RetryCount.ToString(CultureInfo.InvariantCulture)}" +
                $"\n{ReenqueueingTimeInSeconds.ToString(CultureInfo.InvariantCulture)}";
        }

        public TranscriptionServiceBusMessage RetryMessage()
        {
            return new TranscriptionServiceBusMessage(
                Subscription,
                TranscriptionLocation,
                BlobName,
                AudioFileUrl,
                CreatedTime,
                AudioLength,
                Channels,
                EstimatedCost,
                Locale,
                RetryCount + 1,
                ReenqueueingTimeInSeconds);
        }
    }
}
