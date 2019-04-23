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

    internal static class BotConnectorConfig
    {
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public static extern SPXHR bot_connector_config_from_secret_key(out SPXCONNECTORCONFIGHANDLE config,
            [MarshalAs(UnmanagedType.LPStr)] string secret_key,
            [MarshalAs(UnmanagedType.LPStr)] string subscription,
            [MarshalAs(UnmanagedType.LPStr)] string region);
    }
}
