//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using System;
using System.Globalization;
using Microsoft.CognitiveServices.Speech.Internal;
using static Microsoft.CognitiveServices.Speech.Internal.SpxExceptionThrower;

namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// Define payload of speech synthesis events.
    /// Added in version 1.7.0
    /// </summary>
    public class SpeechSynthesisWordBoundaryEventArgs : System.EventArgs, IDisposable
    {
        internal SpeechSynthesisWordBoundaryEventArgs(IntPtr eventHandlePtr)
        {
            eventHandle = new InteropSafeHandle(eventHandlePtr, Internal.Synthesizer.synthesizer_event_handle_release);

            ThrowIfFail(Internal.Synthesizer.synthesizer_word_boundary_event_get_values(eventHandlePtr, out audioOffset, out textOffset, out wordLength));
        }

        /// <summary>
        /// Specifies current word's binary offset in output audio, by ticks (100ns).
        /// </summary>
        public UInt64 AudioOffset
        {
            get
            {
                return audioOffset;
            }
        }

        /// <summary>
        /// Specifies current word's text offset in input text, by characters.
        /// </summary>
        public UInt32 TextOffset
        {
            get
            {
                return textOffset;
            }
        }

        /// <summary>
        /// Specifies current word's length, by characters.
        /// </summary>
        public UInt32 WordLength
        {
            get
            {
                return wordLength;
            }
        }

        private InteropSafeHandle eventHandle;

        private UInt64 audioOffset;

        private UInt32 textOffset;

        private UInt32 wordLength;

        /// <summary>
        /// Dispose of associated resources.
        /// </summary>
        public void Dispose()
        {
            eventHandle.Dispose();
        }
    }
}
