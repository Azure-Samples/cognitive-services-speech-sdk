//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Runtime.InteropServices;

namespace Microsoft.CognitiveServices.Speech.Internal
{
    internal class RecognitionEventArgs : SessionEventArgs
    {
        internal RecognitionEventArgs(IntPtr eventPtr) : base(eventPtr)
        {
        }

        public ulong Offset
        {
            get
            {
                ulong offset = 0;
                ThrowIfNull(eventHandle);
                ThrowIfFail(Recognizer.recognizer_recognition_event_get_offset(eventHandle, ref offset));
                return offset;
            }
        }
    }
}
