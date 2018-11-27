// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import { OutputFormatPropertyName } from "../common.speech/Exports";
import { Contracts } from "./Contracts";
import { OutputFormat, PropertyCollection, PropertyId } from "./Exports";

/**
 * Speech configuration.
 * @class SpeechConfig
 */
export abstract class SpeechConfig {
    /**
     * Creates and initializes an instance.
     * @constructor
     */
    protected constructor() { }

    /**
     * Static instance of SpeechConfig returned by passing subscriptionKey and service region.
     * @member SpeechConfig.fromSubscription
     * @function
     * @public
     * @param {string} subscriptionKey - The subscription key.
     * @param {string} region - The region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).
     * @returns {SpeechConfig} The speech factory
     */
    public static fromSubscription(subscriptionKey: string, region: string): SpeechConfig {
        Contracts.throwIfNullOrWhitespace(subscriptionKey, "subscriptionKey");
        Contracts.throwIfNullOrWhitespace(region, "region");

        const speechImpl: SpeechConfigImpl = new SpeechConfigImpl();
        speechImpl.setProperty(PropertyId.SpeechServiceConnection_Region, region);
        speechImpl.setProperty(PropertyId.SpeechServiceConnection_IntentRegion, region);
        speechImpl.setProperty(PropertyId.SpeechServiceConnection_Key, subscriptionKey);

        return speechImpl;
    }

    /**
     * Creates an instance of the speech factory with specified endpoint and subscription key.
     * This method is intended only for users who use a non-standard service endpoint or paramters.
     * the language setting in uri takes precedence, and the effective language is "de-DE".
     * @member SpeechConfig.fromEndpoint
     * @function
     * @public
     * @param {URL} endpoint - The service endpoint to connect to.
     * @param {string} subscriptionKey - The subscription key.
     * @returns {SpeechConfig} A speech factory instance.
     */
    public static fromEndpoint(endpoint: URL, subscriptionKey: string): SpeechConfig {
        Contracts.throwIfNull(endpoint, "endpoint");
        Contracts.throwIfNullOrWhitespace(subscriptionKey, "subscriptionKey");

        const speechImpl: SpeechConfigImpl = new SpeechConfigImpl();
        speechImpl.setProperty(PropertyId.SpeechServiceConnection_Endpoint, endpoint.href);
        speechImpl.setProperty(PropertyId.SpeechServiceConnection_Key, subscriptionKey);
        return speechImpl;
    }

    /**
     * Creates an instance of the speech factory with specified initial authorization token and region.
     * @member SpeechConfig.fromAuthorizationToken
     * @function
     * @public
     * @param {string} authorizationToken - The initial authorization token.
     * @param {string} region - The region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).
     * @returns {SpeechConfig} A speech factory instance.
     */
    public static fromAuthorizationToken(authorizationToken: string, region: string): SpeechConfig {
        Contracts.throwIfNull(authorizationToken, "authorizationToken");
        Contracts.throwIfNullOrWhitespace(region, "region");

        const speechImpl: SpeechConfigImpl = new SpeechConfigImpl();
        speechImpl.setProperty(PropertyId.SpeechServiceConnection_Region, region);
        speechImpl.setProperty(PropertyId.SpeechServiceConnection_IntentRegion, region);
        speechImpl.authorizationToken = authorizationToken;
        return speechImpl;
    }

    /**
     * Returns the current authorization token.
     * @member SpeechConfig.prototype.authorizationToken
     * @function
     * @public
     */
    public abstract get authorizationToken(): string;

    /**
     * Sets the authorization token.
     * If this is set, subscription key is ignored.
     * User needs to make sure the provided authorization token is valid and not expired.
     * @member SpeechConfig.prototype.authorizationToken
     * @function
     * @public
     * @param {string} value - The authorization token.
     */
    public abstract set authorizationToken(value: string);

    /**
     * Returns the configured language.
     * @member SpeechConfig.prototype.speechRecognitionLanguage
     * @function
     * @public
     */
    public abstract get speechRecognitionLanguage(): string;

    /**
     * Sets the input language.
     * @member SpeechConfig.prototype.speechRecognitionLanguage
     * @function
     * @public
     * @param {string} value - The authorization token.
     */
    public abstract set speechRecognitionLanguage(value: string);

    /**
     * Sets an arbitrary property.
     * @member SpeechConfig.prototype.setProperty
     * @function
     * @public
     * @param {string} name - The name of the property to set.
     * @param {string} value - The new value of the property.
     */
    public abstract setProperty(name: string, value: string): void;

    /**
     * Returns the current value of an arbitrary property.
     * @member SpeechConfig.prototype.getProperty
     * @function
     * @public
     * @param {string} name - The name of the property to query.
     * @param {string} def - The value to return in case the property is not known.
     * @returns {string} The current value, or provided default, of the given property.
     */
    public abstract getProperty(name: string, def?: string): string;

    /**
     * Sets output format.
     * @member SpeechConfig.prototype.outputFormat
     * @function
     * @public
     */
    public abstract set outputFormat(format: OutputFormat);

    /**
     * Gets output format.
     * @member SpeechConfig.prototype.outputFormat
     * @function
     * @public
     * @returns {OutputFormat} Returns the output format.
     */
    public abstract get outputFormat(): OutputFormat;

    /**
     * Sets the endpoint ID of a customized speech model that is used for speech recognition.
     * @member SpeechConfig.prototype.endpointId
     * @function
     * @public
     * @param {string} value - The endpoint ID
     */
    public abstract set endpointId(value: string);

    /**
     * Gets the endpoint ID of a customized speech model that is used for speech recognition.
     * @member SpeechConfig.prototype.endpointId
     * @function
     * @public
     * @return {string} The endpoint ID
     */
    public abstract get endpointId(): string;

    /**
     * Closes the configuration.
     * @member SpeechConfig.prototype.close
     * @function
     * @public
     */
    /* tslint:disable:no-empty */
    public close(): void { }
}

/**
 * @private
 * @class SpeechConfigImpl
 */
// tslint:disable-next-line:max-classes-per-file
export class SpeechConfigImpl extends SpeechConfig {

    private privProperties: PropertyCollection;

    public constructor() {
        super();
        this.privProperties = new PropertyCollection();
        this.speechRecognitionLanguage = "en-US"; // Should we have a default?
        this.outputFormat = OutputFormat.Simple;
    }

    public get properties(): PropertyCollection {
        return this.privProperties;
    }

    public get endPoint(): URL {
        return new URL(this.privProperties.getProperty(PropertyId.SpeechServiceConnection_Endpoint));
    }

    public get subscriptionKey(): string {
        return this.privProperties.getProperty(PropertyId.SpeechServiceConnection_Key);
    }

    public get region(): string {
        return this.privProperties.getProperty(PropertyId.SpeechServiceConnection_Region);
    }

    public get authorizationToken(): string {
        return this.privProperties.getProperty(PropertyId.SpeechServiceAuthorization_Token);
    }

    public set authorizationToken(value: string) {
        this.privProperties.setProperty(PropertyId.SpeechServiceAuthorization_Token, value);
    }

    public get speechRecognitionLanguage(): string {
        return this.privProperties.getProperty(PropertyId.SpeechServiceConnection_RecoLanguage);
    }

    public set speechRecognitionLanguage(value: string) {
        this.privProperties.setProperty(PropertyId.SpeechServiceConnection_RecoLanguage, value);
    }

    public get outputFormat(): OutputFormat {
        return (OutputFormat as any)[this.privProperties.getProperty(OutputFormatPropertyName, undefined)];
    }

    public set outputFormat(value: OutputFormat) {
        this.privProperties.setProperty(OutputFormatPropertyName, OutputFormat[value]);
    }

    public set endpointId(value: string) {
        this.privProperties.setProperty(PropertyId.SpeechServiceConnection_EndpointId, value);
    }

    public get endpointId(): string {
        return this.privProperties.getProperty(PropertyId.SpeechServiceConnection_EndpointId);
    }

    public setProperty(name: string | PropertyId, value: string): void {
        Contracts.throwIfNullOrWhitespace(value, "value");

        this.privProperties.setProperty(name, value);
    }

    public getProperty(name: string | PropertyId, def?: string): string {

        return this.privProperties.getProperty(name, def);
    }

    public clone(): SpeechConfigImpl {
        const ret: SpeechConfigImpl = new SpeechConfigImpl();
        ret.privProperties = this.privProperties.clone();
        return ret;
    }
}
