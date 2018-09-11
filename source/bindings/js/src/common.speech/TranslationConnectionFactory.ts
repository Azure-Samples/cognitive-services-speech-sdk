import { WebsocketConnection } from "../common.browser/Exports";
import {
    IConnection,
    IStringDictionary,
    Promise,
    Storage,
} from "../common/Exports";
import { RecognizerParameterNames } from "../sdk/Exports";
import {
    AuthInfo,
    IConnectionFactory,
    SpeechResultFormat,
    TranslationConfig,
    WebsocketMessageFormatter,
} from "./Exports";

const TestHooksParamName: string = "testhooks";
const ConnectionIdHeader: string = "X-ConnectionId";

export class TranslationConnectionFactory implements IConnectionFactory {

    public Create = (
        config: TranslationConfig,
        authInfo: AuthInfo,
        connectionId?: string): IConnection => {

        let endpoint: string = config.SpeechConfig.getProperty(RecognizerParameterNames.Endpoint, undefined);
        if (!endpoint) {
            const region: string = config.SpeechConfig.getProperty(RecognizerParameterNames.Region, "westus");

            endpoint = this.Host(region) + Storage.Local.GetOrAdd("TranslationRelativeUri", "/speech/translation/cognitiveservices/v1");
        }

        const queryParams: IStringDictionary<string> = {
            format: SpeechResultFormat[config.Format].toString().toLowerCase(),
            from: config.Language,
            to: config.SpeechConfig.getProperty(RecognizerParameterNames.TranslationToLanguage),
        };

        if (this.IsDebugModeEnabled) {
            queryParams[TestHooksParamName] = "1";
        }

        const voiceName: string = "voice";
        const featureName: string = "features";

        if (undefined !== config.Voice) {
            queryParams[voiceName] = config.Voice;
            queryParams[featureName] = "texttospeech";
        }

        const headers: IStringDictionary<string> = {};
        headers[authInfo.HeaderName] = authInfo.Token;
        headers[ConnectionIdHeader] = connectionId;

        return new WebsocketConnection(endpoint, queryParams, headers, new WebsocketMessageFormatter(), connectionId);
    }

    private Host(region: string): string {
        return Storage.Local.GetOrAdd("Host", "wss://" + region + ".s2s.speech.microsoft.com");
    }

    private get IsDebugModeEnabled(): boolean {
        const value = Storage.Local.GetOrAdd("IsDebugModeEnabled", "false");
        return value.toLowerCase() === "true";
    }
}
