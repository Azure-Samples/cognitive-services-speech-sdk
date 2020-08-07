//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using Microsoft.CognitiveServices.Speech.Test.Internal;
using static Microsoft.CognitiveServices.Speech.Test.Internal.SpxExceptionThrower;

namespace Microsoft.CognitiveServices.Speech.Test
{
    /// <summary>
    /// Diagnostics access for internal objects.
    /// </summary>
    public class Diagnostics
    {
        // Hold the callback to prevent it from being GC'd.
        private static LogMessageCallbackFunctionDelegate callback;

        private static readonly Object lockObj = new object();

        /// <summary>
        /// Sets a callback to receive all internal log lines.
        /// </summary>
        /// <param name="callback">Callback to be invoked</param>
        /// <returns>VOID!</returns>
        public static void SetLogMessageCallback(LogMessageCallbackFunctionDelegate callback)
        {
            lock (lockObj)
            {
                ThrowIfFail(Internal.Diagnostics.diagnostics_logmessage_set_callback(callback));
                Diagnostics.callback = callback;
            }
        }

        /// <summary>
        /// Sets filter criteria for all logging.
        /// </summary>
        /// <param name="filters">';' delimited list of filters.</param>
        /// <returns>VOID!</returns>
        public static void SetLogMessageFilter(string filters)
        {
            ThrowIfFail(Internal.Diagnostics.diagnostics_logmessage_set_filters(filters));
        }
    }
}
