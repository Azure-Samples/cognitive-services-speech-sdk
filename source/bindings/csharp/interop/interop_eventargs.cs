//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Runtime.InteropServices;

namespace Microsoft.CognitiveServices.Speech.Internal
{
    using SPXHR = System.IntPtr;
    using SPXEVENTHANDLE = System.IntPtr;

    internal static class EventArgs
    {
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern bool recognizer_event_handle_is_valid(SPXEVENTHANDLE eventHandle);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR recognizer_event_handle_release(SPXEVENTHANDLE eventHandle);
    }
}
