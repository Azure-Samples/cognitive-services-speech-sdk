//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using static Microsoft.CognitiveServices.Speech.Internal.ConversationTranslator;

namespace Microsoft.CognitiveServices.Speech.Transcription
{
    /// <summary>
    /// Defines a payload for the <see cref="ConversationTranslator.ConversationExpiration"/> event.
    /// Added in 1.9.0.
    /// </summary>
    public class ConversationExpirationEventArgs : SessionEventArgs
    {
        /// <summary>
        /// Creates a new instance.
        /// </summary>
        /// <param name="eventHandle">The event handle</param>
        internal ConversationExpirationEventArgs(IntPtr eventHandle) : base(eventHandle)
        {
            this.ExpirationTime = TimeSpan.FromMinutes(
                GetValue<int>(eventHandle, conversation_translator_event_get_expiration_time));
        }

        /// <summary>
        /// How much longer until the conversation expires.
        /// </summary>
        public TimeSpan ExpirationTime { get; }
    }
}
