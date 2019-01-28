//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Runtime.InteropServices;

namespace Microsoft.CognitiveServices.Speech.Internal
{
    using SPXEVENTHANDLE = System.IntPtr;
    using SPXRESULTHANDLE = System.IntPtr;

    internal class SpeechRecognitionCanceledEventArgs : SpeechRecognitionEventArgs
    {
        private CancellationDetails cancellationDetails = null;

        public SpeechRecognitionCanceledEventArgs(IntPtr eventPtr) : base(eventPtr)
        {
            cancellationDetails = CancellationDetails.FromResult(Result);
        }

        ~SpeechRecognitionCanceledEventArgs()
        {
            Dispose(false);
        }

        protected override void Dispose(bool disposing)
        {
            if (disposed) return;

            if (disposing)
            {
                // dispose managed resources
                if (cancellationDetails != null)
                {
                    cancellationDetails.Dispose();
                    cancellationDetails = null;
                }
            }
            // dispose unmanaged resources

            base.Dispose(disposing);
        }

        public CancellationDetails CancellationDetails
        {
            get
            {
                return cancellationDetails;
            }
        }
    }
}
