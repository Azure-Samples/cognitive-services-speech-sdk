//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Collections.Generic;
using System.Globalization;

namespace Microsoft.CognitiveServices.Speech.Translation
{
    /// <summary>
    /// Define payload of translation intermediate/final result events.
    /// </summary>
    public sealed class TranslationTextResultEventArgs : System.EventArgs
    {
        internal TranslationTextResultEventArgs(Microsoft.CognitiveServices.Speech.Internal.TranslationTextResultEventArgs e)
        {
            eventArgImpl = e;
            Result = new TranslationTextResult(e.GetResult());
            SessionId = e.SessionId;
        }

        /// <summary>
        /// Specifies the recognition result.
        /// </summary>
        public TranslationTextResult Result { get; }

        /// <summary>
        /// Specifies the session identifier.
        /// </summary>
        public string SessionId { get; }

        /// <summary>
        /// Returns a string that represents the speech recognition result event.
        /// </summary>
        /// <returns>A string that represents the speech recognition result event.</returns>
        public override string ToString()
        {
            return string.Format(CultureInfo.InvariantCulture,"SessionId:{0} Result:{1}.", SessionId, Result.ToString());
        }

        // Hold the reference
        Microsoft.CognitiveServices.Speech.Internal.TranslationTextResultEventArgs eventArgImpl;
    }
}
