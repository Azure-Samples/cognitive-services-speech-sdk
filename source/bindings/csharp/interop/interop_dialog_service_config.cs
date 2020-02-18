//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using System;
using System.Runtime.InteropServices;

namespace Microsoft.CognitiveServices.Speech.Dialog.Internal
{
    using SPXHR = System.IntPtr;
    using SPXCONNECTORCONFIGHANDLE = System.IntPtr;
    using Speech.Internal;

    internal static class DialogServiceConfig
    {
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public static extern SPXHR bot_framework_config_from_subscription(out SPXCONNECTORCONFIGHANDLE config,
            [MarshalAs(UnmanagedType.LPStr)] string subscription,
            [MarshalAs(UnmanagedType.LPStr)] string region,
            [MarshalAs(UnmanagedType.LPStr)] string botId);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public static extern SPXHR bot_framework_config_from_authorization_token(out SPXCONNECTORCONFIGHANDLE config,
            [MarshalAs(UnmanagedType.LPStr)] string auth_token,
            [MarshalAs(UnmanagedType.LPStr)] string region);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public static extern SPXHR custom_commands_config_from_subscription(out SPXCONNECTORCONFIGHANDLE config,
            [MarshalAs(UnmanagedType.LPStr)] string app_id,
            [MarshalAs(UnmanagedType.LPStr)] string subscription,
            [MarshalAs(UnmanagedType.LPStr)] string region);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public static extern SPXHR custom_commands_config_from_authorization_token(out SPXCONNECTORCONFIGHANDLE config,
            [MarshalAs(UnmanagedType.LPStr)] string app_id,
            [MarshalAs(UnmanagedType.LPStr)] string auth_token,
            [MarshalAs(UnmanagedType.LPStr)] string region);
    }
}
