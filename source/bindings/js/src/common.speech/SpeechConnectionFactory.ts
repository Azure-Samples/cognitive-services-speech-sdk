//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
import { WebsocketConnection } from "../common.browser/Exports";
import { OutputFormatPropertyName } from "../common.speech/Exports";
import {
    IConnection,
    IStringDictionary,
    Storage,
} from "../common/Exports";
import { OutputFormat, PropertyId } from "../sdk/Exports";
import {
    AuthInfo,
    IConnectionFactory,
    RecognitionMode,
    RecognizerConfig,
    WebsocketMessageFormatter,
} from "./Exports";
import { QueryParameterNames } from "./QueryParameterNames";

export class SpeechConnectionFactory implements IConnectionFactory {

    public Create = (
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

        if (this.IsDebugModeEnabled) {
            queryParams[QueryParameterNames.TestHooksParamName] = "1";
        }

        if (!endpoint) {
            const region: string = config.parameters.getProperty(PropertyId.SpeechServiceConnection_Region, undefined);

            switch (config.RecognitionMode) {
                case RecognitionMode.Conversation:
                    endpoint = this.Host(region) + this.ConversationRelativeUri;
                    break;
                case RecognitionMode.Dictation:
                    endpoint = this.Host(region) + this.DictationRelativeUri;
                    break;
                default:
                    endpoint = this.Host(region) + this.InteractiveRelativeUri; // default is interactive
                    break;
            }
        }

        const headers: IStringDictionary<string> = {};
        headers[authInfo.HeaderName] = authInfo.Token;
        headers[QueryParameterNames.ConnectionIdHeader] = connectionId;

        return new WebsocketConnection(endpoint, queryParams, headers, new WebsocketMessageFormatter(), connectionId);
    }

    private Host(region: string): string {
        return Storage.Local.GetOrAdd("Host", "wss://" + region + ".stt.speech.microsoft.com");
    }

    private get InteractiveRelativeUri(): string {
        return Storage.Local.GetOrAdd("InteractiveRelativeUri", "/speech/recognition/interactive/cognitiveservices/v1");
    }

    private get ConversationRelativeUri(): string {
        return Storage.Local.GetOrAdd("ConversationRelativeUri", "/speech/recognition/conversation/cognitiveservices/v1");
    }

    private get DictationRelativeUri(): string {
        return Storage.Local.GetOrAdd("DictationRelativeUri", "/speech/recognition/dictation/cognitiveservices/v1");
    }

    private get IsDebugModeEnabled(): boolean {
        const value = Storage.Local.GetOrAdd("IsDebugModeEnabled", "false");
        return value.toLowerCase() === "true";
    }
}
