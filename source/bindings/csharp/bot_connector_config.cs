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
    /// Class that defines configurations for speech bot connector
    /// Added in version 1.5.0
    /// </summary>
    public sealed class BotConnectorConfig : SpeechConfig
    {
        internal BotConnectorConfig(IntPtr handle) : base(handle)
        {
        }

        /// <summary>
        /// Creates an instance of the bot connector config with the specified Speech Channel secret key.
        /// </summary>
        /// <param name="secretKey">Speech Channel secret key.</param>
        /// <param name="subscription">Subscription key associated with the bot</param>
        /// <param name="region">The region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).</param>
        /// <returns>A new bot connector config.</returns>
        public static BotConnectorConfig FromSecretKey(string secretKey, string subscription, string region)
        {
            IntPtr botConfigHandle = IntPtr.Zero;
            ThrowIfFail(Internal.BotConnectorConfig.bot_connector_config_from_secret_key(out botConfigHandle, secretKey, subscription, region));
            return new BotConnectorConfig(botConfigHandle);
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
        /// Bot Connection initial silence timeout property.
        /// </summary>
        public string BotInitialSilenceTimeout
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
