//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
using System;
using Microsoft.CognitiveServices.Speech.Internal;
using static Microsoft.CognitiveServices.Speech.Internal.SpxExceptionThrower;

namespace Microsoft.CognitiveServices.Speech.Dialog
{
    /// <summary>
    /// Class that defines configurations for dialog service connector
    /// Added in version 1.5.0
    /// </summary>
    public sealed class DialogServiceConfig : SpeechConfig
    {
        internal DialogServiceConfig(IntPtr handle) : base(handle)
        {
        }

        /// <summary>
        /// Creates an instance of the dialog service config with the specified Speech Channel Bot secret key.
        /// </summary>
        /// <param name="secretKey">Speech Channel Bot secret key.</param>
        /// <param name="subscription">Subscription key associated with the bot</param>
        /// <param name="region">The region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).</param>
        /// <returns>A new dialog service config.</returns>
        public static DialogServiceConfig FromBotSecret(string secretKey, string subscription, string region)
        {
            IntPtr configHandle = IntPtr.Zero;
            ThrowIfFail(Internal.DialogServiceConfig.dialog_service_config_from_bot_secret(out configHandle, secretKey, subscription, region));
            return new DialogServiceConfig(configHandle);
        }

        /// <summary>
        /// Creates an instance of the dialog service config with the specified Task Dialog Application id.
        /// </summary>
        /// <param name="applicationId">Task dialog application id.</param>
        /// <param name="subscription">Subscription key associated with the bot</param>
        /// <param name="region">The region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).</param>
        /// <returns>A new dialog service config.</returns>
        public static DialogServiceConfig FromTaskDialogAppId(string applicationId, string subscription, string region)
        {
            IntPtr configHandle = IntPtr.Zero;
            ThrowIfFail(Internal.DialogServiceConfig.dialog_service_config_from_task_dialog_app_id(out configHandle, applicationId, subscription, region));
            return new DialogServiceConfig(configHandle);
        }

        /// <summary>
        /// Speech Channel secret key.
        /// </summary>
        public string SecretKey
        {
            get
            {
                return progBag.GetProperty(PropertyId.Conversation_Secret_Key);
            }
            set
            {
                progBag.SetProperty(PropertyId.Conversation_Secret_Key, value);
            }
        }

        /// <summary>
        /// Task Dialog Application Id
        /// </summary>
        public string TaskDialogAppId
        {
            get
            {
                return progBag.GetProperty(PropertyId.Conversation_TaskDialogAppId);
            }
            set
            {
                progBag.SetProperty(PropertyId.Conversation_TaskDialogAppId, value);
            }
        }

        /// <summary>
        /// Connection initial silence timeout property.
        /// </summary>
        public string InitialSilenceTimeout
        {
            get
            {
                return progBag.GetProperty(PropertyId.Conversation_Initial_Silence_Timeout);
            }
            set
            {
                progBag.SetProperty(PropertyId.Conversation_Initial_Silence_Timeout, value);
            }
        }

        /// <summary>
        /// The format of the audio that is returned during text to speech.
        /// </summary>
        public string TextToSpeechAudioFormat
        {
            get
            {
                return progBag.GetProperty(PropertyId.SpeechServiceConnection_SynthOutputFormat);
            }
            set
            {
                progBag.SetProperty(PropertyId.SpeechServiceConnection_SynthOutputFormat, value);
            }
        }
    }
}
