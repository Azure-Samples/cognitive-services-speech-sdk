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
    using SPXHANDLE = System.IntPtr;
    using SPXSIMODELHANDLE = System.IntPtr;
    using SPXSVMODELHANDLE = System.IntPtr;
    using SPXSPEAKERIDHANDLE = System.IntPtr;
    using SPXRESULTHANDLE = System.IntPtr;
    using SPXPROPERTYBAGHANDLE = System.IntPtr;
    using SPXVOICEPROFILEHANDLE = System.IntPtr;
    using SPXVOICEPROFILECLIENTHANDLE = System.IntPtr;
    using Speech.Internal;

    internal static class SpeakerRecognition
    {
        /// <summary>
        /// An invalid handle
        /// </summary>
        public static readonly SPXHANDLE SPXHANDLE_INVALID = (IntPtr)(-1);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR speaker_recognizer_identify(InteropSafeHandle SpeakerRecognizerHandle, InteropSafeHandle hsimodelHandle, out SPXRESULTHANDLE result);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR speaker_recognizer_verify(InteropSafeHandle speakerRecognizerHandle, InteropSafeHandle hsvmodelHandle, out SPXRESULTHANDLE result);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR speaker_verification_model_release_handle(SPXSVMODELHANDLE svModelHandle);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR speaker_identification_model_release_handle(SPXSIMODELHANDLE siModelHandle);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR speaker_verification_model_create(out SPXSVMODELHANDLE svmodel, InteropSafeHandle profileHandle);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR speaker_identification_model_add_profile(InteropSafeHandle simodelHandle, InteropSafeHandle profileHandle);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR speaker_identification_model_create(out SPXSIMODELHANDLE simodel);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR speaker_recognizer_get_property_bag(InteropSafeHandle speakerRecognizerHandle, out SPXPROPERTYBAGHANDLE propertyBagHandle);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR reset_voice_profile(InteropSafeHandle speakerRecognizerHandle, InteropSafeHandle profileHandle, out SPXRESULTHANDLE result);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR delete_voice_profile(InteropSafeHandle speakerRecognizerHandle, InteropSafeHandle profileHandle, out SPXRESULTHANDLE result);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR voice_profile_client_get_property_bag(SPXSPEAKERIDHANDLE speakerRecognizerHandle, out SPXPROPERTYBAGHANDLE propertyBagHandle);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR create_voice_profile(InteropSafeHandle speakerRecognizerHandle, Int32 voiceProfileType, Utf8StringHandle locale, out SPXVOICEPROFILEHANDLE profileHandle);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR enroll_voice_profile(InteropSafeHandle speakerRecognizerHandle, InteropSafeHandle profileHandle, InteropSafeHandle audioConfigHandle, out SPXRESULTHANDLE result);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR voice_profile_get_id(InteropSafeHandle profileHandle, Utf8StringHandle stringhandle, ref uint length);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR speaker_recognizer_release_handle(SPXSPEAKERIDHANDLE speakerRecognizerHandle);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR voice_profile_release_handle(SPXVOICEPROFILEHANDLE profileHandle);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR voice_profile_client_release_handle(SPXVOICEPROFILECLIENTHANDLE voiceProfileClientHandle);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR create_voice_profile_from_id_and_type(out SPXVOICEPROFILEHANDLE profileHandle, Utf8StringHandle id, Int32 voiceProfileType);
    }
}



