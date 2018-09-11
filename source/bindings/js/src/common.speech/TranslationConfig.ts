import { RecognizerParameterNames } from "../sdk/Exports";
import { SpeechConfigImpl } from "../sdk/SpeechConfig";
import { PlatformConfig, RecognitionMode, RecognizerConfig } from "./RecognizerConfig";

export class TranslationConfig extends RecognizerConfig {

    constructor(
        platformConfig: PlatformConfig,
        speechConfig: SpeechConfigImpl) {
        super(platformConfig, RecognitionMode.Conversation, speechConfig);
    }

    public get TargetLanguages(): string {
        return this.SpeechConfig.getProperty(RecognizerParameterNames.TranslationToLanguage);
    }

    public get Voice(): string {
        return this.SpeechConfig.getProperty(RecognizerParameterNames.TranslationVoice);
    }
}
