import { WebsocketConnection } from "../common.browser/Exports";
import {
    IConnection,
    IStringDictionary,
    Storage,
} from "../common/Exports";
import { PropertyId } from "../sdk/Exports";
import {
    AuthInfo,
    IConnectionFactory,
    RecognizerConfig,
    WebsocketMessageFormatter,
} from "./Exports";

const TestHooksParamName: string = "testhooks";
const ConnectionIdHeader: string = "X-ConnectionId";

export class TranslationConnectionFactory implements IConnectionFactory {

    public Create = (
        config: RecognizerConfig,
        authInfo: AuthInfo,
        connectionId?: string): IConnection => {

        let endpoint: string = config.parameters.getProperty(PropertyId.SpeechServiceConnection_Endpoint, undefined);
        if (!endpoint) {
            const region: string = config.parameters.getProperty(PropertyId.SpeechServiceConnection_Region, undefined);

            endpoint = this.Host(region) + Storage.Local.GetOrAdd("TranslationRelativeUri", "/speech/translation/cognitiveservices/v1");
        }

        const queryParams: IStringDictionary<string> = {
            from: config.parameters.getProperty(PropertyId.SpeechServiceConnection_RecoLanguage),
            to: config.parameters.getProperty(PropertyId.SpeechServiceConnection_TranslationToLanguages),
        };

        if (this.IsDebugModeEnabled) {
            queryParams[TestHooksParamName] = "1";
        }

        const voiceName: string = "voice";
        const featureName: string = "features";

        if (config.parameters.getProperty(PropertyId.SpeechServiceConnection_TranslationVoice, undefined) !== undefined) {
            queryParams[voiceName] = config.parameters.getProperty(PropertyId.SpeechServiceConnection_TranslationVoice);
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
