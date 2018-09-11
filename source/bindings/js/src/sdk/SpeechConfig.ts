//
// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import { Contracts } from "./Contracts";
import { RecognizerParameterNames } from "./Exports";
import { ISpeechProperties } from "./ISpeechProperties";

/**
 * Speech configuration.
 * @class
 */
export abstract class SpeechConfig {
    /**
     * Creates and initializes an instance.
     * @constructor
     */
    protected constructor() { }

    /**
     * Static instance of SpeechConfig returned by passing subscriptionKey and service region.
     * @member
     * @param subscriptionKey The subscription key.
     * @param region The region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).
     * @returns The speech factory
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
     * the language setting in uri takes precedence, and the effective language is "de-DE".
     * @member
     * @param endpoint The service endpoint to connect to.
     * @param subscriptionKey The subscription key.
     * @returns A speech factory instance.
     */
    public static fromEndpoint(endpoint: URL, subscriptionKey: string): SpeechConfig {
        Contracts.throwIfNull(endpoint, "endpoint");
        Contracts.throwIfNullOrWhitespace(subscriptionKey, "subscriptionKey");

        const speechImpl: SpeechConfigImpl = new SpeechConfigImpl();
        speechImpl.setProperty(RecognizerParameterNames.Endpoint, endpoint.href);
        speechImpl.setProperty(RecognizerParameterNames.SubscriptionKey, subscriptionKey);
        return speechImpl;
    }

    /**
     * Creates an instance of the speech factory with specified initial authorization token and region.
     * @param authorizationToken The initial authorization token.
     * @param region The region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).
     * @returns A speech factory instance.
     */
    public static fromAuthorizationToken(authorizationToken: string, region: string): SpeechConfig {
        Contracts.throwIfNull(authorizationToken, "authorizationToken");
        Contracts.throwIfNullOrWhitespace(region, "region");

        const speechImpl: SpeechConfigImpl = new SpeechConfigImpl();
        speechImpl.setProperty(RecognizerParameterNames.Region, region);
        speechImpl.authorizationToken = authorizationToken;
        return speechImpl;
    }

    /**
     * Returns the subscription key.
     * @property
     */
    public abstract get subscriptionKey(): string;

    /**
     * Returns the current region.
     * @property
     */
    public abstract get region(): string;

    /**
     * Returns the current authorization token.
     * @property
     */
    public abstract get authorizationToken(): string;

    /**
     * Sets the authorization token.
     * If this is set, subscription key is ignored.
     * User needs to make sure the provided authorization token is valid and not expired.
     * @property
     * @param value the authorization token.
     */
    public abstract set authorizationToken(value: string);

    /**
     * Returns the configured language.
     * @property
     */
    public abstract get language(): string;

    /**
     * Sets the input language.
     * @property
     * @param value the authorization token.
     */
    public abstract set language(vale: string);

    /**
     * Sets an arbitrary property.
     * @member
     * @param name - The name of the property to set.
     * @param value - The new value of the property.
     */
    public abstract setProperty(name: string, value: string): void;

    /**
     * Returns the current value of an arbitrary property.
     * @param name - The name of the property to query.
     * @param def - The value to return in case the property is not known.
     * @returns The current value, or provided default, of the given property.
     */
    public abstract getProperty(name: string, def?: string): string;

    /**
     * Closes the configuration.
     * @member
     */
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

    public clone(): SpeechConfigImpl {
        const ret: SpeechConfigImpl = new SpeechConfigImpl();
        ret.speechProperties = this.speechProperties.clone();
        return ret;
    }
}
