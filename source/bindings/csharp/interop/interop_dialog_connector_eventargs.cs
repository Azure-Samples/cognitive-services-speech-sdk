//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using System;
using System.Runtime.InteropServices;
using Microsoft.CognitiveServices.Speech.Internal;

namespace Microsoft.CognitiveServices.Speech.Dialog.Internal
{
    using SPXHR = System.IntPtr;
    using SPXEVENTHANDLE = System.IntPtr;
    using SPXACTIVITYHANDLE = System.IntPtr;
    using SPXAUDIOHANDLE = System.IntPtr;

    internal static class ActivityReceivedEventArgs
    {
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        internal static extern bool dialog_connector_activity_received_event_handle_is_valid(SPXEVENTHANDLE eventHandle);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        internal static extern SPXHR dialog_connector_activity_received_event_release(SPXEVENTHANDLE eventHandle);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        internal static extern SPXHR dialog_connector_activity_received_event_get_activity(InteropSafeHandle eventHandle, out SPXACTIVITYHANDLE activityHandle);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        internal static extern bool dialog_connector_activity_received_event_has_audio(InteropSafeHandle eventHandle);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        internal static extern SPXHR dialog_connector_activity_received_event_get_audio(InteropSafeHandle eventHandle, out SPXAUDIOHANDLE audioHandle);
    }
}
