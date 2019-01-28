//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Text;
using System.Runtime.InteropServices;

namespace Microsoft.CognitiveServices.Speech.Internal
{
    using SPXHR = System.IntPtr;
    using SPXLUMODELHANDLE = System.IntPtr;

    internal class LanguageUnderstandingModel : SpxExceptionThrower, IDisposable
    {
        internal SPXLUMODELHANDLE luHandle = IntPtr.Zero;
        private bool disposed = false;

        internal LanguageUnderstandingModel(IntPtr luPtr)
        {
            luHandle = luPtr;
        }

        ~LanguageUnderstandingModel()
        {
            Dispose(false);
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (disposed) return;

            if (disposing)
            {
                // dispose managed resources
            }
            // dispose unmanaged resources
            if (luHandle != IntPtr.Zero)
            {
                ThrowIfFail(language_understanding_model__handle_release(luHandle));
                luHandle = IntPtr.Zero;
            }
            disposed = true;
        }

        public static LanguageUnderstandingModel FromEndpoint(string uri)
        {
            SPXLUMODELHANDLE luModel = IntPtr.Zero;
            ThrowIfFail(language_understanding_model_create_from_uri(out luModel, uri));
            return new LanguageUnderstandingModel(luModel);
        }

        public static LanguageUnderstandingModel FromAppId(string appId)
        {
            SPXLUMODELHANDLE luModel = IntPtr.Zero;
            ThrowIfFail(language_understanding_model_create_from_app_id(out luModel, appId));
            return new LanguageUnderstandingModel(luModel);
        }

        public static LanguageUnderstandingModel FromSubscription(string subscriptionKey, string appId, string region)
        {
            SPXLUMODELHANDLE luModel = IntPtr.Zero;
            ThrowIfFail(language_understanding_model_create_from_subscription(out luModel, subscriptionKey, appId, region));
            return new LanguageUnderstandingModel(luModel);
        }

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern bool language_understanding_model_handle_is_valid(SPXLUMODELHANDLE hlumodel);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR language_understanding_model_create_from_uri(out SPXLUMODELHANDLE hlumodel,
            [MarshalAs(UnmanagedType.LPStr)] string uri);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR language_understanding_model_create_from_app_id(out SPXLUMODELHANDLE hlumodel,
            [MarshalAs(UnmanagedType.LPStr)] string appId);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR language_understanding_model_create_from_subscription(out SPXLUMODELHANDLE hlumodel,
            [MarshalAs(UnmanagedType.LPStr)] string subscriptionKey,
            [MarshalAs(UnmanagedType.LPStr)] string appId,
            [MarshalAs(UnmanagedType.LPStr)] string region);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR language_understanding_model__handle_release(SPXLUMODELHANDLE hlumodel);

    }
}
