// <copyright file="JsonResult.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace RealtimeTranscription
{
    using System.Collections.Generic;
    using Connector;

    public sealed class JsonResult
    {
        public JsonResult(string recognitionStatus, long offset, long duration, string speakerId, List<NBest> nbest)
        {
            this.RecognitionStatus = recognitionStatus;
            this.Offset = offset;
            this.Duration = duration;
            this.SpeakerId = speakerId;
            this.NBest = nbest;
        }

        public string RecognitionStatus { get; }

        public long Offset { get; }

        public long Duration { get; }

        public string SpeakerId { get; }

        public List<NBest> NBest { get; }
    }
}
