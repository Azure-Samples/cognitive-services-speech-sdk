//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Globalization;
using Microsoft.CognitiveServices.Speech.Internal;
using static Microsoft.CognitiveServices.Speech.Internal.ConversationTranslator;

namespace Microsoft.CognitiveServices.Speech.Transcription
{
    /// <summary>
    /// Defines a conversation translation result.
    /// Added in 1.9.0.
    /// </summary>
    public class ConversationTranslationResult : Translation.TranslationRecognitionResult
    {
        /// <summary>
        /// Creates a new instance.
        /// </summary>
        /// <param name="resultPtr">The result handle.</param>
        internal ConversationTranslationResult(IntPtr resultPtr) : base(resultPtr)
        {
            ParticipantId = SpxFactory.GetDataFromHandleUsingDelegate(
                Internal.ConversationTranscriptionResult.conversation_transcription_result_get_user_id,
                resultHandle,
                maxCharCount);
            OriginalLang = GetString(resultPtr, conversation_translator_result_get_original_lang);
        }

        /// <summary>
        /// The unique identifier for the participant this result is for.
        /// </summary>
        public string ParticipantId { get; }

        /// <summary>
        /// The original language this result was in.
        /// </summary>
        public string OriginalLang { get; }

        /// <summary>
        /// Returns a string that represents the conversation translation result.
        /// </summary>
        /// <returns>A string representation.</returns>
        public override string ToString()
        {
            return $"ParticipantId: {ParticipantId}, OriginalLang: {OriginalLang}, {base.ToString()}";
        }
    }
}
