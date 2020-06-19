//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Runtime.InteropServices;
using static Microsoft.CognitiveServices.Speech.Internal.SpxExceptionThrower;

namespace Microsoft.CognitiveServices.Speech.Internal
{
    using SPXHR = IntPtr;
    using SPXHANDLE = IntPtr;
    using SPXCONVERSATIONTRANSLATORHANDLE = IntPtr;
    using SPXPROPERTYBAGHANDLE = IntPtr;
    using SPXEVENTHANDLE = IntPtr;
    using SPXPARTICIPANTHANDLE = IntPtr;
    using SPXRESULTHANDLE = IntPtr;

    /// <summary>
    /// P/Invokes for the conversation translator
    /// </summary>
    internal static class ConversationTranslator
    {
        /// <summary>
        /// An invalid handle
        /// </summary>
        public static readonly SPXHANDLE SPXHANDLE_INVALID = (IntPtr)(-1);

        /// <summary>
        /// Delegate to read a string value from native code. This will return the length if you pass
        /// <see cref="Utf8StringHandle.Null"/> as the native string handle. Otherwise it copy the
        /// native string into the passed in native string handle.
        /// </summary>
        /// <typeparam name="THandle">The handle type.</typeparam>
        /// <param name="handle">The native handle.</param>
        /// <param name="nativeString">The native string handle.</param>
        /// <param name="length">The length of the native string handle. If native string handle is
        /// null, this will be set to the native string length. Otherwise this will be used to determine
        /// the length of the passed in native string handle.</param>
        /// <returns>An SPXHR code indicating success, or failure.</returns>
        public delegate IntPtr NativeReadString<THandle>(THandle handle, Utf8StringHandle nativeString, ref uint length);

        /// <summary>
        /// Delegate to read a value from native code
        /// </summary>
        /// <typeparam name="THandle">The type of the handle.</typeparam>
        /// <typeparam name="TValue">The type of the value to read.</typeparam>
        /// <param name="handle">The native handle.</param>
        /// <param name="value">The value to set.</param>
        /// <returns>SPXHR code</returns>
        public delegate IntPtr NativeReadValue<THandle, TValue>(THandle handle, out TValue value);

        /// <summary>
        /// Helper method to read a UTF8 string from native code
        /// </summary>
        /// <typeparam name="THandle">The native handle type.</typeparam>
        /// <param name="handle">The native handle.</param>
        /// <param name="nativeMethod">The native method to read</param>
        /// <returns>The string value, or null. Throws an exception on errors</returns>
        public static string GetString<THandle>(THandle handle, NativeReadString<THandle> nativeMethod)
        {
            // get the length of the native string first
            uint length = 0;
            ThrowIfFail(nativeMethod(handle, Utf8StringHandle.Null, ref length));

            if (length < 0)
            {
                return null;
            }
            else if (length == 0)
            {
                return string.Empty;
            }

            // allocate enough native memory and read the string
            using (var stringHandle = new Utf8StringHandle(length))
            {
                length = stringHandle.Length;
                ThrowIfFail(nativeMethod(handle, stringHandle, ref length));
                return stringHandle.ToString();
            }
        }

        /// <summary>
        /// Helper method to read a value from native code
        /// </summary>
        /// <typeparam name="TValue">The value type to read</typeparam>
        /// <param name="handle">The native handle</param>
        /// <param name="method">The method to read from native code</param>
        /// <returns>The value or throws an exception on failure</returns>
        public static TValue GetValue<TValue>(InteropSafeHandle handle, NativeReadValue<InteropSafeHandle, TValue> method)
        {
            TValue value;
            ThrowIfFail(method(handle, out value));
            return value;
        }

        /// <summary>
        /// Helper method to read a value from native code
        /// </summary>
        /// <typeparam name="TValue">The value type to read</typeparam>
        /// <param name="handle">The native handle</param>
        /// <param name="method">The method to read from native code</param>
        /// <returns>The value or throws an exception on failure</returns>
        public static TValue GetValue<TValue>(IntPtr handle, NativeReadValue<IntPtr, TValue> method)
        {
            TValue value;
            ThrowIfFail(method(handle, out value));
            return value;
        }

        #region ConversationTranslator P/Invoke

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR conversation_translator_create_from_config(out SPXCONVERSATIONTRANSLATORHANDLE conversationTranslatorHandle, InteropSafeHandle audioinputHandle);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR conversation_translator_get_property_bag(InteropSafeHandle conversationTranslatorHandle, out SPXPROPERTYBAGHANDLE propertyBagHandle);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR conversation_translator_join(InteropSafeHandle conversationTranslatorHandle, InteropSafeHandle conversationHandle, Utf8StringHandle nickname);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR conversation_translator_join_with_id(InteropSafeHandle conversationTranslatorHandle, Utf8StringHandle conversationid, Utf8StringHandle nickname, Utf8StringHandle lang);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR conversation_translator_start_transcribing(InteropSafeHandle conversationTranslatorHandle);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR conversation_translator_stop_transcribing(InteropSafeHandle conversationTranslatorHandle);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR conversation_translator_send_text_message(InteropSafeHandle conversationTranslatorHandle, Utf8StringHandle message);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR conversation_translator_set_authorization_token(InteropSafeHandle conversationTranslatorHandle, Utf8StringHandle authToken, Utf8StringHandle region);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR conversation_translator_leave(InteropSafeHandle conversationTranslatorHandle);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern bool conversation_translator_handle_is_valid(SPXCONVERSATIONTRANSLATORHANDLE handle);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR conversation_translator_handle_release(SPXHANDLE handle);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR conversation_translator_session_started_set_callback(InteropSafeHandle conversationTranslatorHandle, CallbackFunctionDelegate callback, IntPtr context);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR conversation_translator_session_stopped_set_callback(InteropSafeHandle conversationTranslatorHandle, CallbackFunctionDelegate callback, IntPtr context);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR conversation_translator_canceled_set_callback(InteropSafeHandle conversationTranslatorHandle, CallbackFunctionDelegate callback, IntPtr context);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR conversation_translator_participants_changed_set_callback(InteropSafeHandle conversationTranslatorHandle, CallbackFunctionDelegate callback, IntPtr context);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR conversation_translator_conversation_expiration_set_callback(InteropSafeHandle conversationTranslatorHandle, CallbackFunctionDelegate callback, IntPtr context);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR conversation_translator_transcribing_set_callback(InteropSafeHandle conversationTranslatorHandle, CallbackFunctionDelegate callback, IntPtr context);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR conversation_translator_transcribed_set_callback(InteropSafeHandle conversationTranslatorHandle, CallbackFunctionDelegate callback, IntPtr context);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR conversation_translator_text_message_recevied_set_callback(InteropSafeHandle conversationTranslatorHandle, CallbackFunctionDelegate callback, IntPtr context);

        #endregion

        #region ConversationTranslator events P/Invoke

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern bool conversation_translator_event_handle_is_valid(SPXEVENTHANDLE handle);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR conversation_translator_event_handle_release(SPXEVENTHANDLE handle);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR conversation_translator_event_get_expiration_time(SPXEVENTHANDLE eventHandle, out int expirationInMinutes);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR conversation_translator_event_get_participant_changed_reason(SPXEVENTHANDLE eventHandle, out Transcription.ParticipantChangedReason reason);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR conversation_translator_event_get_participant_changed_at_index(SPXEVENTHANDLE eventHandle, int index, out SPXPARTICIPANTHANDLE participantHandle);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR conversation_translator_result_get_original_lang(SPXRESULTHANDLE resultHandle, Utf8StringHandle stringhandle, ref uint length);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR conversation_translator_participant_get_avatar(InteropSafeHandle eventHandle, Utf8StringHandle stringhandle, ref uint length);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR conversation_translator_participant_get_displayname(InteropSafeHandle eventHandle, Utf8StringHandle stringhandle, ref uint length);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR conversation_translator_participant_get_id(InteropSafeHandle eventHandle, Utf8StringHandle stringhandle, ref uint length);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR conversation_translator_participant_get_is_muted(InteropSafeHandle eventHandle, out bool muted);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR conversation_translator_participant_get_is_host(InteropSafeHandle eventHandle, out bool isHost);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR conversation_translator_participant_get_is_using_tts(InteropSafeHandle eventHandle, out bool tts);

        #endregion
    }
}
