//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Transcription;

namespace Microsoft.CognitiveServices.Speech.RemoteConversation
{
    internal class RemoteConversationTranscriptionResultImpl : ConversationTranscriptionResult
    {
        public RemoteConversationTranscriptionResultImpl(
            string text,
            string resultId,
            string userId,
            ResultReason reason,
            long offsetInTicks,
            long durationInticks,
            PropertyCollection collection
            )
            : base(IntPtr.Zero)
        {
            Text = text;
            ResultId = resultId;
            UserId = userId;
            Reason = reason;
            OffsetInTicks = offsetInTicks;
            Duration = TimeSpan.FromTicks(durationInticks);
            Properties = collection;
        }

        public override string UserId { get; }

        public override string UtteranceId { get; }

        public string toString()
        {
            return "ResultId:" + this.ResultId +
                    " Status:" + this.Reason +
                    " UserId:" + this.UserId +
                    " Recognized text:<" + this.Text + ">.";
        }

        public override string ResultId { get; }

        public override ResultReason Reason { get; }

        public override string Text { get; }

        public override TimeSpan Duration { get; }

        public override long OffsetInTicks { get; }

        public override PropertyCollection Properties { get; }
    }
}
