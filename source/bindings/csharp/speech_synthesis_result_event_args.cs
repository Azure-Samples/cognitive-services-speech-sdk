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
    /// Added in version 1.4.0
    /// </summary>
    public class SpeechSynthesisEventArgs : System.EventArgs, IDisposable
    {
        internal SpeechSynthesisEventArgs(IntPtr eventHandlePtr)
        {
            eventHandle = new InteropSafeHandle(eventHandlePtr, Internal.Synthesizer.synthesizer_event_handle_release);

            IntPtr result = IntPtr.Zero;
            ThrowIfFail(Internal.Synthesizer.synthesizer_synthesis_event_get_result(eventHandlePtr, out result));
            Result = new SpeechSynthesisResult(result);
        }

        /// <summary>
        /// Specifies the synthesis result.
        /// </summary>
        public SpeechSynthesisResult Result { get; }

        private InteropSafeHandle eventHandle;

        /// <summary>
        /// Dispose of associated resources.
        /// </summary>
        public void Dispose()
        {
            eventHandle.Dispose();
            Result.Dispose();
            GC.SuppressFinalize(this);
        }
    }
}
