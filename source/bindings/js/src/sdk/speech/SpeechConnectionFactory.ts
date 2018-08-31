import { WebsocketConnection } from "../../common.browser/Exports";
import {
    IConnection,
    IStringDictionary,
    Promise,
    Storage,
} from "../../common/Exports";
import {
    AuthInfo,
    IAuthentication,
    IConnectionFactory,
    RecognitionMode,
    RecognizerConfig,
    SpeechResultFormat,
    WebsocketMessageFormatter,
} from "./Exports";

const TestHooksParamName: string = "testhooks";
const ConnectionIdHeader: string = "X-ConnectionId";

export class SpeechConnectionFactory implements IConnectionFactory {

    public Create = (
        config: RecognizerConfig,
        authInfo: AuthInfo,
        connectionId?: string): IConnection => {

        let endpoint = "";
        switch (config.RecognitionMode) {
            case RecognitionMode.Conversation:
                endpoint = this.Host + this.ConversationRelativeUri;
                break;
            case RecognitionMode.Dictation:
                endpoint = this.Host + this.DictationRelativeUri;
                break;
            default:
                endpoint = this.Host + this.InteractiveRelativeUri; // default is interactive
                break;
        }

        const queryParams: IStringDictionary<string> = {
            format: SpeechResultFormat[config.Format].toString().toLowerCase(),
            language: config.Language,
        };

        if (this.IsDebugModeEnabled) {
            queryParams[TestHooksParamName] = "1";
        }

        const headers: IStringDictionary<string> = {};
        headers[authInfo.HeaderName] = authInfo.Token;
        headers[ConnectionIdHeader] = connectionId;

        return new WebsocketConnection(endpoint, queryParams, headers, new WebsocketMessageFormatter(), connectionId);
    }

    private get Host(): string {
        return Storage.Local.GetOrAdd("Host", "wss://westus.stt.speech.microsoft.com");
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
