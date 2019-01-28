//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Runtime.InteropServices;

namespace Microsoft.CognitiveServices.Speech.Internal
{
    using SPXHR = System.IntPtr;
    using SPXASYNCHANDLE = System.IntPtr;
    using SPXRECOHANDLE = System.IntPtr;
    using SPXRESULTHANDLE = System.IntPtr;
    using SPXPROPERTYBAGHANDLE = System.IntPtr;
    using SPXKEYWORDHANDLE = System.IntPtr;
    using SPXEVENTHANDLE = System.IntPtr;

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void CallbackFunctionDelegate(SPXRECOHANDLE hreco, SPXEVENTHANDLE hevent, IntPtr context);

    internal class Recognizer : SpxExceptionThrower, IDisposable
    {
        internal SPXRECOHANDLE recoHandle = IntPtr.Zero;
        private SPXASYNCHANDLE asyncStartContinuousHandle = IntPtr.Zero;
        private SPXASYNCHANDLE asyncStopContinuousHandle = IntPtr.Zero;
        private SPXASYNCHANDLE asyncStartKeywordHandle = IntPtr.Zero;
        private SPXASYNCHANDLE asyncStopKeywordHandle = IntPtr.Zero;
        protected bool disposed = false;

        internal Recognizer(IntPtr recoPtr)
        {
            recoHandle = recoPtr;
            SPXPROPERTYBAGHANDLE propertyHandle = IntPtr.Zero;
            ThrowIfFail(recognizer_get_property_bag(recoHandle, out propertyHandle));
            Properties = new PropertyCollection(propertyHandle);
        }

        ~Recognizer()
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
                if (Properties != null)
                {
                    Properties.Dispose();
                }
            }
            // dispose unmanaged resources
            if (recoHandle != IntPtr.Zero)
            {
                LogErrorIfFail(recognizer_handle_release(recoHandle));
                recoHandle = IntPtr.Zero;
            }
            disposed = true;
        }

        public virtual bool IsEnabled()
        {
            bool enabled = false;
            ThrowIfFail(recognizer_is_enabled(recoHandle, out enabled));
            return enabled;
        }

        public virtual void Enable()
        {
            ThrowIfFail(recognizer_enable(recoHandle));
        }

        public virtual void Disable()
        {
            ThrowIfFail(recognizer_disable(recoHandle));
        }

        public SPXRESULTHANDLE RecognizeOnce()
        {
            SPXRESULTHANDLE hresult = IntPtr.Zero;
            ThrowIfFail(recognizer_recognize_once(recoHandle, out hresult));
            return hresult;
        }

        public void StartContinuousRecognition()
        {
            if (asyncStartContinuousHandle != IntPtr.Zero)
            {
                ThrowIfFail(recognizer_async_handle_release(asyncStartContinuousHandle));
            }
            ThrowIfFail(recognizer_start_continuous_recognition_async(recoHandle, out asyncStartContinuousHandle));
            ThrowIfFail(recognizer_start_continuous_recognition_async_wait_for(asyncStartContinuousHandle, UInt32.MaxValue));
            ThrowIfFail(recognizer_async_handle_release(asyncStartContinuousHandle));
            asyncStartContinuousHandle = IntPtr.Zero;
        }

        public void StopContinuousRecognition()
        {
            if (asyncStopContinuousHandle != IntPtr.Zero)
            {
                ThrowIfFail(recognizer_async_handle_release(asyncStopContinuousHandle));
            }
            ThrowIfFail(recognizer_stop_continuous_recognition_async(recoHandle, out asyncStopContinuousHandle));
            ThrowIfFail(recognizer_stop_continuous_recognition_async_wait_for(asyncStopContinuousHandle, UInt32.MaxValue));
            ThrowIfFail(recognizer_async_handle_release(asyncStopContinuousHandle));
            asyncStopContinuousHandle = IntPtr.Zero;
        }

        public void StartKeywordRecognition(KeywordRecognitionModel model)
        {
            if (asyncStartKeywordHandle != IntPtr.Zero)
            {
                ThrowIfFail(recognizer_async_handle_release(asyncStartKeywordHandle));
            }
            ThrowIfFail(recognizer_start_keyword_recognition_async(recoHandle, model.keywordHandle, out asyncStartKeywordHandle));
            GC.KeepAlive(model);
            ThrowIfFail(recognizer_start_keyword_recognition_async_wait_for(asyncStartKeywordHandle, UInt32.MaxValue));
            ThrowIfFail(recognizer_async_handle_release(asyncStartKeywordHandle));
            asyncStartKeywordHandle = IntPtr.Zero;
        }

        public void StopKeywordRecognition()
        {
            if (asyncStopKeywordHandle != IntPtr.Zero)
            {
                ThrowIfFail(recognizer_async_handle_release(asyncStopKeywordHandle));
            }
            ThrowIfFail(recognizer_stop_keyword_recognition_async(recoHandle, out asyncStopKeywordHandle));
            ThrowIfFail(recognizer_stop_keyword_recognition_async_wait_for(asyncStopKeywordHandle, UInt32.MaxValue));
            ThrowIfFail(recognizer_async_handle_release(asyncStopKeywordHandle));
            asyncStopKeywordHandle = IntPtr.Zero;
        }

        public void SetRecognizingCallback(CallbackFunctionDelegate callbackDelegate, IntPtr gcHandlePtr)
        {
            ThrowIfFail(recognizer_recognizing_set_callback(recoHandle, callbackDelegate, gcHandlePtr));
        }

        public void SetRecognizedCallback(CallbackFunctionDelegate callbackDelegate, IntPtr gcHandlePtr)
        {
            ThrowIfFail(recognizer_recognized_set_callback(recoHandle, callbackDelegate, gcHandlePtr));
        }

        public void SetCanceledCallback(CallbackFunctionDelegate callbackDelegate, IntPtr gcHandlePtr)
        {
            ThrowIfFail(recognizer_canceled_set_callback(recoHandle, callbackDelegate, gcHandlePtr));
        }

        public void SetSpeechStartDetectedCallback(CallbackFunctionDelegate callbackDelegate, IntPtr gcHandlePtr)
        {
            ThrowIfFail(recognizer_speech_start_detected_set_callback(recoHandle, callbackDelegate, gcHandlePtr));
        }

        public void SetSpeechEndDetectedCallback(CallbackFunctionDelegate callbackDelegate, IntPtr gcHandlePtr)
        {
            ThrowIfFail(recognizer_speech_end_detected_set_callback(recoHandle, callbackDelegate, gcHandlePtr));
        }

        public void SetSessionStartedCallback(CallbackFunctionDelegate callbackDelegate, IntPtr gcHandlePtr)
        {
            ThrowIfFail(recognizer_session_started_set_callback(recoHandle, callbackDelegate, gcHandlePtr));
        }

        public void SetSessionStoppedCallback(CallbackFunctionDelegate callbackDelegate, IntPtr gcHandlePtr)
        {
            ThrowIfFail(recognizer_session_stopped_set_callback(recoHandle, callbackDelegate, gcHandlePtr));
        }

        public PropertyCollection Properties { get; } = null;        

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR recognizer_get_property_bag(SPXRECOHANDLE hreco, out SPXPROPERTYBAGHANDLE hpropbag);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern bool recognizer_handle_is_valid(SPXRECOHANDLE hreco);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR recognizer_handle_release(SPXRECOHANDLE hreco);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR recognizer_enable(SPXRECOHANDLE hreco);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR recognizer_disable(SPXRECOHANDLE hreco);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR recognizer_is_enabled(SPXRECOHANDLE hreco, out bool pfEnabled);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR recognizer_recognize_once(SPXRECOHANDLE hreco, out SPXRESULTHANDLE phresult);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR recognizer_recognize_once_async(SPXRECOHANDLE hreco, out SPXASYNCHANDLE phasync);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR recognizer_recognize_once_async_wait_for(SPXASYNCHANDLE hasync, UInt32 milliseconds, out SPXRESULTHANDLE phresult);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR recognizer_start_continuous_recognition(SPXRECOHANDLE hreco);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR recognizer_start_continuous_recognition_async(SPXRECOHANDLE hreco, out SPXASYNCHANDLE phasync);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR recognizer_start_continuous_recognition_async_wait_for(SPXASYNCHANDLE hasync, UInt32 milliseconds);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR recognizer_stop_continuous_recognition(SPXRECOHANDLE hreco);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR recognizer_stop_continuous_recognition_async(SPXRECOHANDLE hreco, out SPXASYNCHANDLE phasync);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR recognizer_stop_continuous_recognition_async_wait_for(SPXASYNCHANDLE hasync, UInt32 milliseconds);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern bool recognizer_async_handle_is_valid(SPXASYNCHANDLE hasync);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR recognizer_async_handle_release(SPXASYNCHANDLE hasync);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR recognizer_start_keyword_recognition(SPXRECOHANDLE hreco, SPXKEYWORDHANDLE hkeyword);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR recognizer_start_keyword_recognition_async(SPXRECOHANDLE hreco, SPXKEYWORDHANDLE hkeyword, out SPXASYNCHANDLE phasync);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR recognizer_start_keyword_recognition_async_wait_for(SPXASYNCHANDLE hasync, UInt32 milliseconds);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR recognizer_stop_keyword_recognition(SPXRECOHANDLE hreco);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR recognizer_stop_keyword_recognition_async(SPXRECOHANDLE hreco, out SPXASYNCHANDLE phasync);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR recognizer_stop_keyword_recognition_async_wait_for(SPXASYNCHANDLE hasync, UInt32 milliseconds);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR recognizer_recognition_event_get_offset(SPXEVENTHANDLE hevent, ref UInt64 offset);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public static extern SPXHR recognizer_session_event_get_session_id(SPXEVENTHANDLE hevent, IntPtr sessionIdStr, UInt32 sessionId);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR recognizer_recognition_event_get_result(SPXEVENTHANDLE hevent, out SPXRESULTHANDLE phresult);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern bool recognizer_event_handle_is_valid(SPXEVENTHANDLE hevent);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR recognizer_session_started_set_callback(SPXRECOHANDLE hreco, CallbackFunctionDelegate pCallback, IntPtr pvContext);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR recognizer_session_stopped_set_callback(SPXRECOHANDLE hreco, CallbackFunctionDelegate pCallback, IntPtr pvContext);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR recognizer_speech_start_detected_set_callback(SPXRECOHANDLE hreco, CallbackFunctionDelegate pCallback, IntPtr pvContext);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR recognizer_speech_end_detected_set_callback(SPXRECOHANDLE hreco, CallbackFunctionDelegate pCallback, IntPtr pvContext);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR recognizer_recognizing_set_callback(SPXRECOHANDLE hreco, CallbackFunctionDelegate pCallback, IntPtr pvContext);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR recognizer_recognized_set_callback(SPXRECOHANDLE hreco, CallbackFunctionDelegate pCallback, IntPtr pvContext);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR recognizer_canceled_set_callback(SPXRECOHANDLE hreco, CallbackFunctionDelegate pCallback, IntPtr pvContext);

    }
}
