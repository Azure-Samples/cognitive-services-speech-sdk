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

const TestHooksParamName: string = "testhooks";
const ConnectionIdHeader: string = "X-ConnectionId";

export class SpeechConnectionFactory implements IConnectionFactory {

    public Create = (
        config: RecognizerConfig,
        authInfo: AuthInfo,
        connectionId?: string): IConnection => {

        let endpoint: string = config.parameters.getProperty(PropertyId.SpeechServiceConnection_Endpoint, undefined);
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

        const queryParams: IStringDictionary<string> = {
            format: config.parameters.getProperty(OutputFormatPropertyName, OutputFormat[OutputFormat.Simple]).toLowerCase(),
            language: config.parameters.getProperty(PropertyId.SpeechServiceConnection_RecoLanguage),
        };

        if (this.IsDebugModeEnabled) {
            queryParams[TestHooksParamName] = "1";
        }

        const headers: IStringDictionary<string> = {};
        headers[authInfo.HeaderName] = authInfo.Token;
        headers[ConnectionIdHeader] = connectionId;

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
