// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import { WebsocketConnection } from "../common.browser/Exports";
import { IConnection, IStringDictionary, Storage } from "../common/Exports";
import { PropertyId } from "../sdk/Exports";
import { AuthInfo, IConnectionFactory, RecognizerConfig, WebsocketMessageFormatter } from "./Exports";

const TestHooksParamName: string = "testhooks";
const ConnectionIdHeader: string = "X-ConnectionId";

export class TranslationConnectionFactory implements IConnectionFactory {

    public create = (
        config: RecognizerConfig,
        authInfo: AuthInfo,
        connectionId?: string): IConnection => {

        let endpoint: string = config.parameters.getProperty(PropertyId.SpeechServiceConnection_Endpoint, undefined);
        if (!endpoint) {
            const region: string = config.parameters.getProperty(PropertyId.SpeechServiceConnection_Region, undefined);

            endpoint = this.host(region) + Storage.local.getOrAdd("TranslationRelativeUri", "/speech/translation/cognitiveservices/v1");
        }

        const queryParams: IStringDictionary<string> = {
            from: config.parameters.getProperty(PropertyId.SpeechServiceConnection_RecoLanguage),
            to: config.parameters.getProperty(PropertyId.SpeechServiceConnection_TranslationToLanguages),
        };

        if (this.isDebugModeEnabled) {
            queryParams[TestHooksParamName] = "1";
        }

        const voiceName: string = "voice";
        const featureName: string = "features";

        if (config.parameters.getProperty(PropertyId.SpeechServiceConnection_TranslationVoice, undefined) !== undefined) {
            queryParams[voiceName] = config.parameters.getProperty(PropertyId.SpeechServiceConnection_TranslationVoice);
            queryParams[featureName] = "texttospeech";
        }

        const headers: IStringDictionary<string> = {};
        headers[authInfo.headerName] = authInfo.token;
        headers[ConnectionIdHeader] = connectionId;

        return new WebsocketConnection(endpoint, queryParams, headers, new WebsocketMessageFormatter(), connectionId);
    }

    private host(region: string): string {
        return Storage.local.getOrAdd("Host", "wss://" + region + ".s2s.speech.microsoft.com");
    }

    private get isDebugModeEnabled(): boolean {
        const value = Storage.local.getOrAdd("IsDebugModeEnabled", "false");
        return value.toLowerCase() === "true";
    }
}
