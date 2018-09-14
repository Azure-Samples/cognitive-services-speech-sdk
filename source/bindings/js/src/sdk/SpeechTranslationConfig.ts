//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import { Contracts } from "./Contracts";
import { PropertyCollection, PropertyId } from "./Exports";

/**
 * Speech translation configuration.
 */
export abstract class SpeechTranslationConfig {

    /**
     * Creates an instance of recognizer config.
     */
    protected constructor() {
    }

    /**
     * Static instance of SpeechTranslationConfig returned by passing subscriptionKey and service region.
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
     * @param value the authorization token.
     */
    public abstract set authorizationToken(value: string);

    /**
     * Sets the authorization token.
     * If this is set, subscription key is ignored.
     * User needs to make sure the provided authorization token is valid and not expired.
     * @param value the authorization token.
     */
    public abstract set speechRecognitionLanguage(value: string);

    /**
     * Add a (text) target language to translate into.
     * @param value the language such as de-DE
     */
    public abstract addTargetLanguage(value: string): void;

    /**
     * Add a (text) target language to translate into.
     * @param value the language such as de-DE
     */
    public abstract get targetLanguages(): string[];

    /**
     * Sets voice of the translated language, enable voice synthesis output.
     * @param value
     */
    public abstract set voiceName(value: string);

    /**
     * Sets a named property as value
     * @param name the name of the property
     * @param value the value
     */
    public abstract setProperty(name: string, value: string): void;

    /**
     * Dispose of associated resources.
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
     * @param value the authorization token.
     */
    public set speechRecognitionLanguage(value: string) {
        Contracts.throwIfNullOrWhitespace(value, "value");

        this.properties.setProperty(PropertyId.SpeechServiceConnection_TranslationFromLanguage, value);
    }

    /**
     * Add a (text) target language to translate into.
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
     * @param value the language such as de-DE
     */
    public get targetLanguages(): string[] {

        if (this.speechProperties.hasProperty(PropertyId.SpeechServiceConnection_TranslationToLanguages)) {
            return this.speechProperties.getProperty(PropertyId.SpeechServiceConnection_TranslationToLanguages).split(",");
        } else {
            return [];
        }

    }

    /**
     * Sets voice of the translated language, enable voice synthesis output.
     * @param value
     */
    public set voiceName(value: string) {
        Contracts.throwIfNullOrWhitespace(value, "value");

        this.properties.setProperty(PropertyId.SpeechServiceConnection_TranslationVoice, value);
    }

    public setProperty(name: string, value: string): void {
        this.properties.setProperty(name, value);
    }

    public get properties(): PropertyCollection {
        return this.speechProperties;
    }

    /**
     * Dispose of associated resources.
     */
    public close(): void {
        return;
    }
}
