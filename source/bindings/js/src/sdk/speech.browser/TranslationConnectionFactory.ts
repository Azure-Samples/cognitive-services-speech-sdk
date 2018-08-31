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
    TranslationConfig,
    WebsocketMessageFormatter,
} from "../speech/Exports";

const TestHooksParamName: string = "testhooks";
const ConnectionIdHeader: string = "X-ConnectionId";

export class TranslationConnectionFactory implements IConnectionFactory {

    public Create = (
        config: TranslationConfig,
        authInfo: AuthInfo,
        connectionId?: string): IConnection => {

        const endpoint = this.Host + Storage.Local.GetOrAdd("TranslationRelativeUri", "/speech/translation/cognitiveservices/v1");

        let targets: string = "";
        config.TargetLanguages.forEach((value: string, index: number, array: string[]) => targets += value + ",");
        targets = targets.substr(0, targets.length - 1);

        const queryParams: IStringDictionary<string> = {
            format: SpeechResultFormat[config.Format].toString().toLowerCase(),
            from: config.Language,
            to: targets,
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
        return Storage.Local.GetOrAdd("Host", "wss://westus.s2s.speech.microsoft.com"); // TODO: More than one region....
    }

    private get IsDebugModeEnabled(): boolean {
        const value = Storage.Local.GetOrAdd("IsDebugModeEnabled", "false");
        return value.toLowerCase() === "true";
    }
}
