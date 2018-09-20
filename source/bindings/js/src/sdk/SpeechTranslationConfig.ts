//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

import {
    OutputFormatPropertyName,
} from "../common.speech/Exports";
import {
    Contracts,
} from "./Contracts";
import {
    OutputFormat,
    PropertyCollection,
    PropertyId,
    SpeechConfig,
} from "./Exports";

/**
 * Speech translation configuration.
 * @class SpeechTranslationConfig
 */
export abstract class SpeechTranslationConfig extends SpeechConfig {

    /**
     * Creates an instance of recognizer config.
     */
    protected constructor() {
        super();
    }

    /**
     * Static instance of SpeechTranslationConfig returned by passing subscriptionKey and service region.
     * @member SpeechTranslationConfig.fromSubscription
     * @param subscriptionKey The subscription key.
     * @param region The region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).
     * @return The speech config
     */
    public static fromSubscription(subscriptionKey: string, region: string): SpeechTranslationConfig {
        Contracts.throwIfNullOrWhitespace(subscriptionKey, "subscriptionKey");
        Contracts.throwIfNullOrWhitespace(region, "region");

        const ret: SpeechTranslationConfigImpl = new SpeechTranslationConfigImpl();
        ret.properties.setProperty(PropertyId.SpeechServiceConnection_Key, subscriptionKey);
        ret.properties.setProperty(PropertyId.SpeechServiceConnection_Region, region);
        return ret;
    }

    /**
     * Static instance of SpeechTranslationConfig returned by passing authorization token and service region.
     * Note: The caller needs to ensure that the authorization token is valid. Before the authorization token
     * expipres, the caller needs to refresh it by setting the property `AuthorizationToken` with a new valid token.
     * Otherwise, all the recognizers created by this SpeechTranslationConfig instance will encounter errors during recognition.
     * @member SpeechTranslationConfig.fromAuthorizationToken
     * @param authorizationToken The authorization token.
     * @param region The region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).
     * @return The speech config
     */
    public static fromAuthorizationToken(authorizationToken: string, region: string): SpeechTranslationConfig {
        Contracts.throwIfNullOrWhitespace(authorizationToken, "authorizationToken");
        Contracts.throwIfNullOrWhitespace(region, "region");

        const ret: SpeechTranslationConfigImpl = new SpeechTranslationConfigImpl();
        ret.properties.setProperty(PropertyId.SpeechServiceAuthorization_Token, authorizationToken);
        ret.properties.setProperty(PropertyId.SpeechServiceConnection_Region, region);
        return ret;
    }

    /**
     * Creates an instance of the speech config with specified endpoint and subscription key.
     * This method is intended only for users who use a non-standard service endpoint or paramters.
     * Note: The query properties specified in the endpoint URL are not changed, even if they are set by any other APIs.
     * For example, if language is defined in the uri as query parameter "language=de-DE", and also set by CreateSpeechRecognizer("en-US"),
     * the language setting in uri takes precedence, and the effective language is "de-DE".
     * Only the properties that are not specified in the endpoint URL can be set by other APIs.
     * @member SpeechTranslationConfig.fromEndpoint
     * @param endpoint The service endpoint to connect to.
     * @param subscriptionKey The subscription key.
     * @return A speech config instance.
     */
    public static fromEndpoint(endpoint: URL, subscriptionKey: string): SpeechTranslationConfig {
        Contracts.throwIfNull(endpoint, "endpoint");
        Contracts.throwIfNullOrWhitespace(subscriptionKey, "subscriptionKey");

        const ret: SpeechTranslationConfigImpl = new SpeechTranslationConfigImpl();
        ret.properties.setProperty(PropertyId.SpeechServiceConnection_Endpoint, endpoint.href);
        ret.properties.setProperty(PropertyId.SpeechServiceConnection_Key, subscriptionKey);
        return ret;
    }

    /**
     * Sets the authorization token.
     * If this is set, subscription key is ignored.
     * User needs to make sure the provided authorization token is valid and not expired.
     * @member SpeechTranslationConfig.prototype.authorizationToken
     * @param value the authorization token.
     */
    public abstract set authorizationToken(value: string);

    /**
     * Sets the authorization token.
     * If this is set, subscription key is ignored.
     * User needs to make sure the provided authorization token is valid and not expired.
     * @member SpeechTranslationConfig.prototype.speechRecognitionLanguage
     * @param value the authorization token.
     */
    public abstract set speechRecognitionLanguage(value: string);

    /**
     * Add a (text) target language to translate into.
     * @member SpeechTranslationConfig.prototype.addTargetLanguage
     * @param value the language such as de-DE
     */
    public abstract addTargetLanguage(value: string): void;

    /**
     * Add a (text) target language to translate into.
     * @member SpeechTranslationConfig.prototype.targetLanguages
     * @param value the language such as de-DE
     */
    public abstract get targetLanguages(): string[];

    /**
     * Returns the selected voice name.
     * @member SpeechTranslationConfig.prototype.voiceName
     * @returns The voice name.
     */
    public abstract get voiceName(): string;

    /**
     * Sets voice of the translated language, enable voice synthesis output.
     * @member SpeechTranslationConfig.prototype.voiceName
     * @param value
     */
    public abstract set voiceName(value: string);

    /**
     * Sets a named property as value
     * @member SpeechTranslationConfig.prototype.setProperty
     * @param name the name of the property
     * @param value the value
     */
    public abstract setProperty(name: string, value: string): void;

    /**
     * Dispose of associated resources.
     * @member SpeechTranslationConfig.prototype.close
     */
    public abstract close(): void;
}

// tslint:disable-next-line:max-classes-per-file
export class SpeechTranslationConfigImpl extends SpeechTranslationConfig {
    private speechProperties: PropertyCollection;

    public constructor() {
        super();
        this.speechProperties = new PropertyCollection();

    }
    /**
     * Sets the authorization token.
     * If this is set, subscription key is ignored.
     * User needs to make sure the provided authorization token is valid and not expired.
     * @member SpeechTranslationConfigImpl.prototype.authorizationToken
     * @param value the authorization token.
     */
    public set authorizationToken(value: string) {
        Contracts.throwIfNullOrWhitespace(value, "value");

        this.properties.setProperty(PropertyId.SpeechServiceAuthorization_Token, value);
    }

    /**
     * Sets the authorization token.
     * If this is set, subscription key is ignored.
     * User needs to make sure the provided authorization token is valid and not expired.
     * @member SpeechTranslationConfigImpl.prototype.speechRecognitionLanguage
     * @param value the authorization token.
     */
    public set speechRecognitionLanguage(value: string) {
        Contracts.throwIfNullOrWhitespace(value, "value");

        this.properties.setProperty(PropertyId.SpeechServiceConnection_RecoLanguage, value);
    }

    /**
     * @member SpeechTranslationConfigImpl.prototype.subscriptionKey
     */
    public get subscriptionKey(): string {
        return this.speechProperties.getProperty(PropertyId[PropertyId.SpeechServiceConnection_Key]);
    }

    /**
     * @member SpeechTranslationConfigImpl.prototype.outputFormat
     */
    public get outputFormat(): OutputFormat {
        return (OutputFormat as any)[this.speechProperties.getProperty(OutputFormatPropertyName, OutputFormat[OutputFormat.Simple])];
    }

    /**
     * @member SpeechTranslationConfigImpl.prototype.outputFormat
     */
    public set outputFormat(value: OutputFormat) {
        this.speechProperties.setProperty(OutputFormatPropertyName, OutputFormat[value]);
    }

    /**
     * @member SpeechTranslationConfigImpl.prototype.endpointId
     */
    public set endpointId(value: string) {
        this.speechProperties.setProperty(PropertyId.SpeechServiceConnection_Endpoint, value);
    }

    /**
     * @member SpeechTranslationConfigImpl.prototype.endpointId
     */
    public get endpointId(): string {
        return this.speechProperties.getProperty(PropertyId.SpeechServiceConnection_EndpointId);
    }
    /**
     * Add a (text) target language to translate into.
     * @member SpeechTranslationConfigImpl.prototype.addTargetLanguage
     * @param value the language such as de-DE
     */
    public addTargetLanguage(value: string): void {
        Contracts.throwIfNullOrWhitespace(value, "value");

        const languages: string[] = this.targetLanguages;
        languages.push(value);
        this.properties.setProperty(PropertyId.SpeechServiceConnection_TranslationToLanguages, languages.join(","));
    }

    /**
     * Add a (text) target language to translate into.
     * @member SpeechTranslationConfigImpl.prototype.targetLanguages
     * @param value the language such as de-DE
     */
    public get targetLanguages(): string[] {

        if (this.speechProperties.getProperty(PropertyId.SpeechServiceConnection_TranslationToLanguages, undefined) !== undefined) {
            return this.speechProperties.getProperty(PropertyId.SpeechServiceConnection_TranslationToLanguages).split(",");
        } else {
            return [];
        }

    }

    /**
     * @member SpeechTranslationConfigImpl.prototype.voiceName
     */
    public get voiceName(): string {
        return this.getProperty(PropertyId[PropertyId.SpeechServiceConnection_TranslationVoice]);
    }

    /**
     * Sets voice of the translated language, enable voice synthesis output.
     * @member SpeechTranslationConfigImpl.prototype.voiceName
     * @param value
     */
    public set voiceName(value: string) {
        Contracts.throwIfNullOrWhitespace(value, "value");

        this.properties.setProperty(PropertyId.SpeechServiceConnection_TranslationVoice, value);
    }

    /**
     * Provides the region.
     * @member SpeechTranslationConfigImpl.prototype.region
     * @returns The region.
     */
    public get region(): string {
        return this.speechProperties.getProperty(PropertyId.SpeechServiceConnection_Region);
    }

    /**
     * Allows for setting arbitrary properties.
     * @member SpeechTranslationConfigImpl.prototype.setProperty
     * @param name - The name of the property.
     * @param value - The value of the property.
     */
    public setProperty(name: string, value: string): void {
        this.properties.setProperty(name, value);
    }

    /**
     * Allows for retrieving arbitrary property values.
     * @member SpeechTranslationConfigImpl.prototype.getProperty
     * @param name - The name of the property.
     * @param def - The default value of the property in case it is not set.
     * @returns The value of the property.
     */
    public getProperty(name: string, def?: string): string {
        return this.speechProperties.getProperty(name, def);
    }

    /**
     * Provides access to custom properties.
     * @member SpeechTranslationConfigImpl.prototype.properties
     * @returns The properties.
     */
    public get properties(): PropertyCollection {
        return this.speechProperties;
    }

    /**
     * Dispose of associated resources.
     * @member SpeechTranslationConfigImpl.prototype.close
     */
    public close(): void {
        return;
    }
}
