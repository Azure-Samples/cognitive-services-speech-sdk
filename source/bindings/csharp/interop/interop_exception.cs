//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Globalization;
using System.Runtime.InteropServices;

namespace Microsoft.CognitiveServices.Speech.Internal
{
    using SPXHR = System.IntPtr;
    internal class SpxExceptionThrower
    {
        internal static void ThrowIfFail(SPXHR hr)
        {
            if (hr != IntPtr.Zero)
            {
                int error = (int)SpxError.GetErrorCode(hr);
                string message = String.Format(CultureInfo.CurrentCulture, "Exception with an error code: 0x{0}", error.ToString("X", CultureInfo.CurrentCulture).ToLower(CultureInfo.CurrentCulture));
                SpxError.Release(hr);
                throw new ApplicationException(message);
            }
        }

        internal static void ThrowIfNull(object item)
        {
            if (item == null)
            {
                string message = "NullReferenceException";
                throw new ApplicationException(message);
            }
        }

        internal static void ThrowIfNull(IntPtr item)
        {
            if (item == null)
            {
                string message = "NullReferenceException";
                throw new ApplicationException(message);
            }
        }

        internal static void ThrowIfFail(bool item)
        {
            if (item == false)
            {
                string message = "Exception with an error code: false condition";
                throw new ApplicationException(message);
            }
        }
        
        internal static void ThrowIfFail(bool item, int error)
        {
            if (item == false)
            {
                string message = String.Format(CultureInfo.CurrentCulture, "Exception with an error code: {0}", error.ToString("X", CultureInfo.CurrentCulture).ToLower(CultureInfo.CurrentCulture));
                throw new ApplicationException(message);
            }
        }

        internal static void LogErrorIfFail(SPXHR hr)
        {
            if (hr != IntPtr.Zero)
            {
                int error = (int)SpxError.GetErrorCode(hr);
                string message = String.Format(CultureInfo.CurrentCulture, "Failed with an error code: {0}", error.ToString("X", CultureInfo.CurrentCulture).ToLower(CultureInfo.CurrentCulture));
                SpxError.Release(hr);
                System.Diagnostics.Trace.WriteLine(message);
            }
        }

        internal static void LogError(SPXHR hr)
        {
            if (hr != IntPtr.Zero)
            {
                int error = (int)SpxError.GetErrorCode(hr);
                string message = String.Format(CultureInfo.CurrentCulture, "Failed with an error code: {0}", error.ToString("X", CultureInfo.CurrentCulture).ToLower(CultureInfo.CurrentCulture));
                SpxError.Release(hr);
                System.Diagnostics.Trace.WriteLine(message);
            }
        }

        internal static void LogError(string message)
        {
            System.Diagnostics.Trace.WriteLine(message);
        }
    }
}
