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
import { FactoryParameterNames } from "./Exports";

const TestHooksParamName: string = "testhooks";
const ConnectionIdHeader: string = "X-ConnectionId";

export class TranslationConnectionFactory implements IConnectionFactory {

    public Create = (
        config: TranslationConfig,
        authInfo: AuthInfo,
        connectionId?: string): IConnection => {

        let endpoint: string = config.RecognizerProperties.get(FactoryParameterNames.Endpoint, undefined);
        if (!endpoint) {
            const region: string = config.RecognizerProperties.get(FactoryParameterNames.Region, "westus");

            endpoint = this.Host(region) + Storage.Local.GetOrAdd("TranslationRelativeUri", "/speech/translation/cognitiveservices/v1");
        }

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
