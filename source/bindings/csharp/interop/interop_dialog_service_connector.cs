//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using System;
using System.Text;
using System.Runtime.InteropServices;

namespace Microsoft.CognitiveServices.Speech.Dialog.Internal
{
    using SPXHR = System.IntPtr;
    using SPXCONNECTORHANDLE = System.IntPtr;
    using SPXKEYWORDHANDLE = System.IntPtr;
    using SPXEVENTHANDLE = System.IntPtr;
    using SPXACTIVITYHANDLE = System.IntPtr;
    using SPXRESULTHANDLE = System.IntPtr;
    using SPXPROPERTYBAGHANDLE = System.IntPtr;
    using Speech.Internal;

    internal static class DialogServiceConnector
    {
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR dialog_service_connector_get_property_bag(InteropSafeHandle connectorHandle, out SPXPROPERTYBAGHANDLE propertyBag);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR dialog_service_connector_handle_release(SPXCONNECTORHANDLE connectorHandle);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR dialog_service_connector_connect(InteropSafeHandle connectorHandle);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR dialog_service_connector_disconnect(InteropSafeHandle connectorHandle);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention, CharSet = CharSet.Ansi)]
        public static extern SPXHR dialog_service_connector_send_activity(InteropSafeHandle connectorHandle, SPXACTIVITYHANDLE activityHandle, StringBuilder interactionId);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR dialog_service_connector_start_keyword_recognition(InteropSafeHandle connectorHandle, InteropSafeHandle keywordHandle);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR dialog_service_connector_stop_keyword_recognition(InteropSafeHandle connectorHandle);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR dialog_service_connector_listen_once(InteropSafeHandle connectorHandle, out SPXRESULTHANDLE result);


        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR dialog_service_connector_session_started_set_callback(InteropSafeHandle connectorHandle, CallbackFunctionDelegate callback, IntPtr context);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR dialog_service_connector_session_stopped_set_callback(InteropSafeHandle connectorHandle, CallbackFunctionDelegate callback, IntPtr context);


        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR dialog_service_connector_recognized_set_callback(InteropSafeHandle connectorHandle, CallbackFunctionDelegate callback, IntPtr context);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR dialog_service_connector_recognizing_set_callback(InteropSafeHandle connectorHandle, CallbackFunctionDelegate callback, IntPtr context);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR dialog_service_connector_canceled_set_callback(InteropSafeHandle connectorHandle, CallbackFunctionDelegate callback, IntPtr context);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR dialog_service_connector_activity_received_set_callback(InteropSafeHandle connectorHandle, CallbackFunctionDelegate callback, IntPtr context);

    }
}



