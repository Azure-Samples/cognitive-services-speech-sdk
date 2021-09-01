// <copyright file="AudioFileInfo.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector.Serializable.TranscriptionStartedServiceBusMessage
{
    public class AudioFileInfo
    {
        public AudioFileInfo(string fileUrl, int retryCount)
        {
            FileUrl = fileUrl;
            RetryCount = retryCount;
        }

        public string FileUrl { get; set; }

        public int RetryCount { get; set; }
    }
}
