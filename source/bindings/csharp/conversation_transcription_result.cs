//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using System;
using System.Globalization;
using Microsoft.CognitiveServices.Speech.Internal;

namespace Microsoft.CognitiveServices.Speech.Transcription
{
    /// <summary>
    /// Class that defines the result of conversation transcriber.
    /// Added in version 1.5.0
    /// </summary>
    public sealed class ConversationTranscriptionResult : SpeechRecognitionResult
    {
        internal ConversationTranscriptionResult(IntPtr resultPtr)
            : base(resultPtr)
        {
            UserId = SpxFactory.GetDataFromHandleUsingDelegate(Internal.ConversationTranscriptionResult.conversation_transcription_result_get_user_id, resultHandle, maxCharCount);
            UtteranceId = SpxFactory.GetDataFromHandleUsingDelegate(Internal.ConversationTranscriptionResult.conversation_transcription_result_get_utterance_id, resultHandle, maxCharCount);
        }

        /// <summary>
        /// A string that represents the user id.
        /// </summary>
        public string UserId { get; }

        /// <summary>
        /// A string that represents the utterance. This id is consistence for intermediates and final speech recognition result from one speaker.
        /// </summary>
        public string UtteranceId { get; }

        /// <summary>
        /// Returns a string that represents the conversation transcription result.
        /// </summary>
        /// <returns>A string that represents the conversation transcription result.</returns>
        public override string ToString()
        {
            return string.Format(CultureInfo.InvariantCulture, "ResultId:{0} Reason:{1} UserId:<{2}> UtteranceId:<{3}> Recognized text:<{4}> ",
                ResultId, Reason, UserId, UtteranceId, Text);
        }
    }
}
