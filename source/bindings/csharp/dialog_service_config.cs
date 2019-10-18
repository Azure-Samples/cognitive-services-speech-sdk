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
    /// Class that defines base configurations for dialog service connector
    /// Added in version 1.5.0
    /// </summary>
    public abstract class DialogServiceConfig
    {
        private SpeechConfig config;
        internal InteropSafeHandle configHandle
        {
            get
            {
                return this.config.configHandle;
            }
        }

        internal DialogServiceConfig(IntPtr handle)
        {
            this.config = new SpeechConfig(handle);
        }

        /// <summary>
        /// Sets the property by name.
        /// </summary>
        /// <param name="name">Name of the property</param>
        /// <param name="value">Value of the property</param>
        public void SetProperty(string name, string value)
        {
            this.config.SetProperty(name, value);
        }

        /// <summary>
        /// Sets the property by propertyId
        /// </summary>
        /// <param name="id">PropertyId of the property</param>
        /// <param name="value">Value of the property</param>
        public void SetProperty(PropertyId id, string value)
        {
            this.config.SetProperty(id, value);
        }

        /// <summary>
        /// Gets the property by name.
        /// </summary>
        /// <param name="name">Name of the property</param>
        /// <returns>Value of the property</returns>
        public string GetProperty(string name)
        {
            return this.config.GetProperty(name);
        }

        /// <summary>
        /// Gets the property by propertyId.
        /// </summary>
        /// <param name="id">PropertyId of the property</param>
        /// <returns>Value of the property</returns>
        public string GetProperty(PropertyId id)
        {
            return this.config.GetProperty(id);
        }

        /// <summary>
        /// Sets a property value that will be passed to service using the specified channel.
        /// </summary>
        /// <param name="name">The property name.</param>
        /// <param name="value">The property value.</param>
        /// <param name="channel">The channel used to pass the specified property to service.</param>
        public void SetServiceProperty(string name, string value, ServicePropertyChannel channel)
        {
            this.config.SetServiceProperty(name, value, channel);
        }

        /// <summary>
        /// Sets proxy configuration.
        ///
        /// Note: Proxy functionality is not available on macOS. This function will have no effect on this platform.
        /// </summary>
        /// <param name="proxyHostName">The host name of the proxy server, without the protocol scheme (http://)</param>
        /// <param name="proxyPort">The port number of the proxy server.</param>
        /// <param name="proxyUserName">The user name of the proxy server.</param>
        /// <param name="proxyPassword">The password of the proxy server.</param>
        public void SetProxy(string proxyHostName, int proxyPort, string proxyUserName, string proxyPassword)
        {
            this.config.SetProxy(proxyHostName, proxyPort, proxyUserName, proxyPassword);
        }

        /// <summary>
        /// Sets proxy configuration.
        /// </summary>
        /// <param name="proxyHostName">The host name of the proxy server.</param>
        /// <param name="proxyPort">The port number of the proxy server.</param>
        public void SetProxy(string proxyHostName, int proxyPort)
        {
            this.config.SetProxy(proxyHostName, proxyPort);
        }

        /// <summary>
        /// Specifies the name of the language to be used in BCP-47 format
        /// </summary>
        public string Language
        {
            get
            {
                return this.config.SpeechRecognitionLanguage;
            }
            set
            {
                this.config.SpeechRecognitionLanguage = value;
            }
        }
    }

    /// <summary>
    /// Class that defines configurations for the dialog service connector object for using a Bot Framework backend.
    /// </summary>
    public sealed class BotFrameworkConfig: DialogServiceConfig
    {
        private BotFrameworkConfig(IntPtr handle): base(handle)
        {
        }

        /// <summary>
        /// Creates an instance of the bot framework config with the specified subscription and region.
        /// </summary>
        /// <param name="subscription">Subscription key associated with the bot</param>
        /// <param name="region">The region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).</param>
        /// <returns>A new bot framework config.</returns>
        public static BotFrameworkConfig FromSubscription(string subscription, string region)
        {
            IntPtr configHandle = IntPtr.Zero;
            ThrowIfFail(Internal.DialogServiceConfig.bot_framework_config_from_subscription(out configHandle, subscription, region));
            return new BotFrameworkConfig(configHandle);
        }

        /// <summary>
        /// Creates an instance of the bot framework config with the specified authorization token and region.
        /// Note: The caller needs to ensure that the authorization token is valid. Before the authorization token
        /// expires, the caller needs to refresh it by calling this setter with a new valid token.
        /// As configuration values are copied when creating a new recognizer, the new token value will not apply to recognizers that have already been created.
        /// For recognizers that have been created before, you need to set authorization token of the corresponding recognizer
        /// to refresh the token. Otherwise, the recognizers will encounter errors during recognition.
        /// </summary>
        /// <param name="authorizationToken">The authorization token associated with the bot</param>
        /// <param name="region">The region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).</param>
        /// <returns>A new bot framework config.</returns>
        public static BotFrameworkConfig FromAuthorizationToken(string authorizationToken, string region)
        {
            IntPtr configHandle = IntPtr.Zero;
            ThrowIfFail(Internal.DialogServiceConfig.bot_framework_config_from_authorization_token(out configHandle, authorizationToken, region));
            return new BotFrameworkConfig(configHandle);
        }
    }

    /// <summary>
    /// Class that defines configurations for the dialog service connector object for using a CustomCommands backend.
    /// </summary>
    public sealed class CustomCommandsConfig: DialogServiceConfig
    {
        private CustomCommandsConfig(IntPtr handle): base(handle)
        {
        }

        /// <summary>
        /// Creates an instance of the dialog service config with the specified Custom Commands application id, subscription and region.
        /// </summary>
        /// <param name="applicationId">Custom Commands application id.</param>
        /// <param name="subscription">Subscription key associated with the application.</param>
        /// <param name="region">The region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).</param>
        /// <returns>A new Custom Commands config.</returns>
        public static CustomCommandsConfig FromSubscription(string applicationId, string subscription, string region)
        {
            IntPtr configHandle = IntPtr.Zero;
            ThrowIfFail(Internal.DialogServiceConfig.custom_commands_config_from_subscription(out configHandle, applicationId, subscription, region));
            return new CustomCommandsConfig(configHandle);
        }

        /// <summary>
        /// Creates an instance of the dialog service config with the specified Custom Commands application id, authorization token and region.
        /// Note: The caller needs to ensure that the authorization token is valid. Before the authorization token
        /// expires, the caller needs to refresh it by calling this setter with a new valid token.
        /// As configuration values are copied when creating a new recognizer, the new token value will not apply to recognizers that have already been created.
        /// For recognizers that have been created before, you need to set authorization token of the corresponding recognizer
        /// to refresh the token. Otherwise, the recognizers will encounter errors during recognition.
        /// </summary>
        /// <param name="applicationId">Custom Commands application id.</param>
        /// <param name="authorizationToken">The authorization token associated with the application.</param>
        /// <param name="region">The region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).</param>
        /// <returns>A new Custom Commands config.</returns>
        public static CustomCommandsConfig FromAuthorizationToken(string applicationId, string authorizationToken, string region)
        {
            IntPtr configHandle = IntPtr.Zero;
            ThrowIfFail(Internal.DialogServiceConfig.custom_commands_config_from_authorization_token(out configHandle, applicationId, authorizationToken, region));
            return new CustomCommandsConfig(configHandle);
        }

        /// <summary>
        /// Custom Commands application identifier.
        /// </summary>
        public string ApplicationId
        {
            get
            {
                return this.GetProperty(PropertyId.Conversation_ApplicationId);
            }
            set
            {
                this.SetProperty(PropertyId.Conversation_ApplicationId, value);
            }
        }
    }
}