// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import { WebsocketConnection } from "../common.browser/Exports";
import { OutputFormatPropertyName } from "../common.speech/Exports";
import { IConnection, IStringDictionary, Storage } from "../common/Exports";
import { OutputFormat, PropertyId } from "../sdk/Exports";
import { AuthInfo, IConnectionFactory, RecognitionMode, RecognizerConfig, WebsocketMessageFormatter } from "./Exports";
import { QueryParameterNames } from "./QueryParameterNames";

export class SpeechConnectionFactory implements IConnectionFactory {

    public create = (
        config: RecognizerConfig,
        authInfo: AuthInfo,
        connectionId?: string): IConnection => {

        let endpoint: string = config.parameters.getProperty(PropertyId.SpeechServiceConnection_Endpoint, undefined);

        const queryParams: IStringDictionary<string> = {};

        const endpointId: string = config.parameters.getProperty(PropertyId.SpeechServiceConnection_EndpointId, undefined);
        const language: string = config.parameters.getProperty(PropertyId.SpeechServiceConnection_RecoLanguage, undefined);

        if (endpointId) {
            if (!endpoint || endpoint.search(QueryParameterNames.DeploymentIdParamName) === -1) {
                queryParams[QueryParameterNames.DeploymentIdParamName] = endpointId;
            }
        } else if (language) {
            if (!endpoint || endpoint.search(QueryParameterNames.LanguageParamName) === -1) {
                queryParams[QueryParameterNames.LanguageParamName] = language;
            }
        }

        if (!endpoint || endpoint.search(QueryParameterNames.FormatParamName) === -1) {
            queryParams[QueryParameterNames.FormatParamName] = config.parameters.getProperty(OutputFormatPropertyName, OutputFormat[OutputFormat.Simple]).toLowerCase();
        }

        if (this.isDebugModeEnabled) {
            queryParams[QueryParameterNames.TestHooksParamName] = "1";
        }

        if (!endpoint) {
            const region: string = config.parameters.getProperty(PropertyId.SpeechServiceConnection_Region, undefined);

            switch (config.recognitionMode) {
                case RecognitionMode.Conversation:
                    endpoint = this.host(region) + this.conversationRelativeUri;
                    break;
                case RecognitionMode.Dictation:
                    endpoint = this.host(region) + this.dictationRelativeUri;
                    break;
                default:
                    endpoint = this.host(region) + this.interactiveRelativeUri; // default is interactive
                    break;
            }
        }

        const headers: IStringDictionary<string> = {};
        headers[authInfo.headerName] = authInfo.token;
        headers[QueryParameterNames.ConnectionIdHeader] = connectionId;

        return new WebsocketConnection(endpoint, queryParams, headers, new WebsocketMessageFormatter(), connectionId);
    }

    private host(region: string): string {
        return Storage.local.getOrAdd("Host", "wss://" + region + ".stt.speech.microsoft.com");
    }

    private get interactiveRelativeUri(): string {
        return Storage.local.getOrAdd("InteractiveRelativeUri", "/speech/recognition/interactive/cognitiveservices/v1");
    }

    private get conversationRelativeUri(): string {
        return Storage.local.getOrAdd("ConversationRelativeUri", "/speech/recognition/conversation/cognitiveservices/v1");
    }

    private get dictationRelativeUri(): string {
        return Storage.local.getOrAdd("DictationRelativeUri", "/speech/recognition/dictation/cognitiveservices/v1");
    }

    private get isDebugModeEnabled(): boolean {
        const value = Storage.local.getOrAdd("IsDebugModeEnabled", "false");
        return value.toLowerCase() === "true";
    }
}
