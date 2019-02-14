//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Runtime.InteropServices;

namespace Microsoft.CognitiveServices.Speech.Internal
{
    using SPXHR = System.IntPtr;
    using SPXRESULTHANDLE = System.IntPtr;

    internal static class CancellationDetails
    {
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR result_get_reason_canceled(InteropSafeHandle result, out CancellationReason reason);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR result_get_canceled_error_code(InteropSafeHandle result, out CancellationErrorCode errorCode);
    }
}
