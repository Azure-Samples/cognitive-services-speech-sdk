//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Runtime.InteropServices;
using System.Text;

namespace Microsoft.CognitiveServices.Speech.Internal
{
    using SPXHR = System.IntPtr;
    using SPXEVENTHANDLE = System.IntPtr;

    internal class SessionEventArgs : EventArgs
    {
        private const Int32 maxUUID = 36;
        private const Int32 maxSessionID = maxUUID + 1;

        internal SessionEventArgs(IntPtr eventPtr) : base(eventPtr)
        {
        }

        public string SessionId
        {
            get
            {
                ThrowIfNull(eventHandle);
                return SpxFactory.GetDataFromHandleUsingDelegate(Recognizer.recognizer_session_event_get_session_id, eventHandle, maxSessionID);
            }
        }
    }
}
