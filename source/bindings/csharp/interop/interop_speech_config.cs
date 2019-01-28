//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Text;
using System.Runtime.InteropServices;
using System.Globalization;

namespace Microsoft.CognitiveServices.Speech.Internal
{
    using SPXHR = System.IntPtr;
    using SPXSPEECHCONFIGHANDLE = System.IntPtr;
    using SPXPROPERTYBAGHANDLE = System.IntPtr;

    internal class SpeechConfig: SpxExceptionThrower, IDisposable
    {
        protected PropertyCollection progBag = null;
        internal SPXSPEECHCONFIGHANDLE configHandle = IntPtr.Zero;
        protected bool disposed = false;

        internal SpeechConfig(IntPtr configPtr)
        {
            configHandle = configPtr;
            SPXPROPERTYBAGHANDLE hpropbag = IntPtr.Zero;
            ThrowIfFail(speech_config_get_property_bag(configHandle, out hpropbag));
            progBag = new PropertyCollection(hpropbag);
        }

        ~SpeechConfig()
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
                if (progBag != null)
                {
                    progBag.Dispose();
                    progBag = null;
                }
            }
            // dispose unmanaged resources
            if (configHandle != IntPtr.Zero)
            {
                LogErrorIfFail(speech_config_release(configHandle));
                configHandle = IntPtr.Zero;
            }
            disposed = true;
        }

        public static SpeechConfig FromSubscription(string subscriptionKey, string region)
        {
            SPXSPEECHCONFIGHANDLE speechConfigHandle = IntPtr.Zero;
            ThrowIfFail(speech_config_from_subscription(out speechConfigHandle, subscriptionKey, region));
            return new SpeechConfig(speechConfigHandle);
        }

        public static SpeechConfig FromAuthorizationToken(string authorizationToken, string region)
        {
            SPXSPEECHCONFIGHANDLE speechConfigHandle = IntPtr.Zero;
            ThrowIfFail(speech_config_from_authorization_token(out speechConfigHandle, authorizationToken, region));
            return new SpeechConfig(speechConfigHandle);
        }

        public static SpeechConfig FromEndpoint(string endpoint, string subscriptionKey)
        {
            SPXSPEECHCONFIGHANDLE speechConfigHandle = IntPtr.Zero;
            ThrowIfFail(speech_config_from_endpoint(out speechConfigHandle, endpoint, subscriptionKey));
            return new SpeechConfig(speechConfigHandle);
        }

        public string SpeechRecognitionLanguage
        {
            get
            {
                return progBag.GetProperty(PropertyId.SpeechServiceConnection_RecoLanguage);
            }
            set
            {
                progBag.SetProperty(PropertyId.SpeechServiceConnection_RecoLanguage, value);
            }
        }

        public string EndpointId
        {
            get
            {
                return progBag.GetProperty(PropertyId.SpeechServiceConnection_EndpointId);
            }
            set
            {
                progBag.SetProperty(PropertyId.SpeechServiceConnection_EndpointId, value);
            }
        }

        public string AuthorizationToken
        {
            get
            {
                return progBag.GetProperty(PropertyId.SpeechServiceAuthorization_Token);
            }
            set
            {
                progBag.SetProperty(PropertyId.SpeechServiceAuthorization_Token, value);
            }

        }

        public string SubscriptionKey
        {
            get
            {
                return progBag.GetProperty(PropertyId.SpeechServiceConnection_Key);
            }
        }

        public string Region
        {
            get
            {
                return progBag.GetProperty(PropertyId.SpeechServiceConnection_Region);
            }
        }

        public string GetProperty(string name)
        {
            return progBag.GetProperty(name);
        }

        public string GetProperty(PropertyId id)
        {
            return progBag.GetProperty(id);
        }

        public void SetProperty(string name, string value)
        {
            progBag.SetProperty(name, value);
        }

        public void SetProperty(PropertyId id, string value)
        {
            progBag.SetProperty(id, value);
        }

        public void SetProxy(string proxyHostName, UInt32 proxyPort, string proxyUserName, string proxyPassword)
        {
            progBag.SetProperty(PropertyId.SpeechServiceConnection_ProxyHostName, proxyHostName);
            progBag.SetProperty(PropertyId.SpeechServiceConnection_ProxyPort, proxyPort.ToString(CultureInfo.CurrentCulture));
            progBag.SetProperty(PropertyId.SpeechServiceConnection_ProxyUserName, proxyUserName);
            progBag.SetProperty(PropertyId.SpeechServiceConnection_ProxyPassword, proxyPassword);
        }

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern bool speech_config_is_handle_valid(SPXSPEECHCONFIGHANDLE hconfig);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public static extern SPXHR speech_config_from_subscription(out SPXSPEECHCONFIGHANDLE hconfig,
            [MarshalAs(UnmanagedType.LPStr)] string subscriptionKey,
            [MarshalAs(UnmanagedType.LPStr)] string region);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public static extern SPXHR speech_config_from_authorization_token(out SPXSPEECHCONFIGHANDLE hconfig,
            [MarshalAs(UnmanagedType.LPStr)] string authToken,
            [MarshalAs(UnmanagedType.LPStr)] string region);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public static extern SPXHR speech_config_from_endpoint(out SPXSPEECHCONFIGHANDLE hconfig,
            [MarshalAs(UnmanagedType.LPStr)] string endpoint,
            [MarshalAs(UnmanagedType.LPStr)] string subscription);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR speech_config_release(SPXSPEECHCONFIGHANDLE hconfig);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR speech_config_get_property_bag(SPXSPEECHCONFIGHANDLE hconfig, out SPXPROPERTYBAGHANDLE hpropbag);

    }
}
