//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using Microsoft.CognitiveServices.Speech.Transcription;
using System.Collections.Generic;

namespace Microsoft.CognitiveServices.Speech.RemoteConversation
{
    /// <summary>
    /// RemoteConversationTranscriptionResult for the completed transcriptions.
    /// Added in version 1.13.0
    /// </summary>
    public class RemoteConversationTranscriptionResult
    {
        /// <summary>
        /// Gets an Id representing the operation that can be used to poll for
        /// the status of the long-running operation.
        /// </summary>
        public string Id { get; internal set; }

        /// <summary>
        /// Gets the complete transcriptions for the long-running transcription operation.
        /// </summary>
        public IReadOnlyList<ConversationTranscriptionResult> ConversationTranscriptionResults
        {
            get
            {
                return m_transcriptions;
            }
        }

        internal void AddTranscription(ConversationTranscriptionResult t)
        {
            m_transcriptions.Add(t);
        }

        internal RemoteConversationTranscriptionResult()
        {
            m_transcriptions = new List<ConversationTranscriptionResult>();
        }

        private List<ConversationTranscriptionResult> m_transcriptions = null;
    }
}
