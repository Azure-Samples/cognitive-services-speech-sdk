// <copyright file="TranscriptionStartedMessage.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector
{
    using System;
    using System.Collections.Generic;
    using Connector.Serializable.TranscriptionStartedServiceBusMessage;
    using Newtonsoft.Json;

    public class TranscriptionStartedMessage
    {
        public TranscriptionStartedMessage(
            string transcriptionLocation,
            string jobName,
            string locale,
            bool usesCustomModel,
            IEnumerable<AudioFileInfo> audioFileInfos,
            int pollingCounter,
            int failedExecutionCounter)
        {
            this.TranscriptionLocation = transcriptionLocation;
            this.JobName = jobName;
            this.Locale = locale;
            this.UsesCustomModel = usesCustomModel;
            this.AudioFileInfos = audioFileInfos;
            this.PollingCounter = pollingCounter;
            this.FailedExecutionCounter = failedExecutionCounter;
        }

        public string TranscriptionLocation { get; set; }

        public string JobName { get; set; }

        public string Locale { get; set; }

        public bool UsesCustomModel { get; set; }

        public IEnumerable<AudioFileInfo> AudioFileInfos { get; set; }

        // Gets incremented when Transcription job is retried because it is still running, not started, or the request timed out / got throttled
        public int PollingCounter { get; set; }

        // Gets incremented when Transcription job is retried because of unexpected exception
        public int FailedExecutionCounter { get; set; }

        public static TranscriptionStartedMessage DeserializeMessage(string serviceBusMessage)
        {
            if (serviceBusMessage == null)
            {
                throw new ArgumentNullException(nameof(serviceBusMessage));
            }

            return JsonConvert.DeserializeObject<TranscriptionStartedMessage>(serviceBusMessage);
        }

        public string CreateMessageString()
        {
            return JsonConvert.SerializeObject(this);
        }
    }
}
