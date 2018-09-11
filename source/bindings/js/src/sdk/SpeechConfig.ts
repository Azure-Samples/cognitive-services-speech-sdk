//
// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import { Contracts } from "./Contracts";
import { RecognizerParameterNames } from "./Exports";
import { ISpeechProperties } from "./ISpeechProperties";

export abstract class SpeechConfig {
    protected constructor() { }

    /**
     * Static instance of SpeechConfig returned by passing subscriptionKey and service region.
     * @param subscriptionKey The subscription key.
     * @param region The region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).
     * @return The speech factory
     */
    public static fromSubscription(subscriptionKey: string, region: string): SpeechConfig {
        Contracts.throwIfNullOrWhitespace(subscriptionKey, "subscriptionKey");
        Contracts.throwIfNullOrWhitespace(region, "region");

        const speechImpl: SpeechConfigImpl = new SpeechConfigImpl();

        speechImpl.setProperty(RecognizerParameterNames.Region, region);
        speechImpl.setProperty(RecognizerParameterNames.SubscriptionKey, subscriptionKey);

        return speechImpl;
    }

    /**
     * Creates an instance of the speech factory with specified endpoint and subscription key.
     * This method is intended only for users who use a non-standard service endpoint or paramters.
     * Note: The query parameters specified in the endpoint URL are not changed, even if they are set by any other APIs.
     * For example, if language is defined in the uri as query parameter "language=de-DE", and also set by CreateSpeechRecognizer("en-US"),
     * the language setting in uri takes precedence, and the effective language is "de-DE".
     * @param endpoint The service endpoint to connect to.
     * @param subscriptionKey The subscription key.
     * @return A speech factory instance.
     */
    public static fromEndpoint(endpoint: URL, subscriptionKey: string): SpeechConfig {
        Contracts.throwIfNull(endpoint, "endpoint");
        Contracts.throwIfNullOrWhitespace(subscriptionKey, "subscriptionKey");
        const speechImpl: SpeechConfigImpl = new SpeechConfigImpl();

        speechImpl.setProperty(RecognizerParameterNames.Endpoint, endpoint.href);
        speechImpl.setProperty(RecognizerParameterNames.SubscriptionKey, subscriptionKey);
        return speechImpl;
    }

    public abstract get subscriptionKey(): string;

    public abstract get region(): string;

    public abstract get authorizationToken(): string;

    /**
     * Sets the authorization token.
     * If this is set, subscription key is ignored.
     * User needs to make sure the provided authorization token is valid and not expired.
     * @param value the authorization token.
     */
    public abstract set authorizationToken(value: string);

    public abstract get language(): string;
    /**
     * Sets the input language.
     * @param value the authorization token.
     */
    public abstract set language(vale: string);

    public abstract setProperty(name: string, value: string): void;

    public abstract getProperty(name: string, def?: string): string;

    /* tslint:disable:no-empty */
    public close(): void { }
}

// tslint:disable-next-line:max-classes-per-file
export class SpeechConfigImpl extends SpeechConfig {

    private speechProperties: ISpeechProperties;

    public constructor() {
        super();
        this.speechProperties = new ISpeechProperties();
        this.language = "en-US"; // Should we have a default?
    }

    public get parameters(): ISpeechProperties {
        return this.speechProperties;
    }

    public get endPoint(): URL {
        return new URL(this.speechProperties.get(RecognizerParameterNames.Endpoint));
    }

    public get subscriptionKey(): string {
        return this.speechProperties.get(RecognizerParameterNames.SubscriptionKey);
    }

    public get region(): string {
        return this.speechProperties.get(RecognizerParameterNames.Region);
    }

    public get authorizationToken(): string {
        return this.speechProperties.get(RecognizerParameterNames.AuthorizationToken);
    }

    public set authorizationToken(value: string) {
        this.speechProperties.set(RecognizerParameterNames.AuthorizationToken, value);
    }

    public get language(): string {
        return this.speechProperties.get(RecognizerParameterNames.SpeechRecognitionLanguage);
    }

    public set language(value: string) {
        this.speechProperties.set(RecognizerParameterNames.SpeechRecognitionLanguage, value);
        this.speechProperties.set(RecognizerParameterNames.TranslationFromLanguage, value);
    }

    public setAuthorizationToken(value: string): void {
        Contracts.throwIfNullOrWhitespace(value, "value");

        this.speechProperties.set(RecognizerParameterNames.AuthorizationToken, value);
    }

    public has(key: string): boolean {
        return this.speechProperties.has(key);
    }

    public setProperty(name: string, value: string): void {
        Contracts.throwIfNullOrWhitespace(name, "name");

        this.speechProperties.set(name, value);
    }

    public getProperty(name: string, def?: string): string {
        Contracts.throwIfNullOrWhitespace(name, "name");

        return this.speechProperties.get(name, def);
    }

}
