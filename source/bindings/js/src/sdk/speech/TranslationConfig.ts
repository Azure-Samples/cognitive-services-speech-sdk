import { ISpeechProperties } from "../speech.browser/Exports";
import { Context, RecognitionMode, RecognizerConfig, SpeechConfig, SpeechResultFormat } from "./RecognizerConfig";

export class TranslationConfig extends RecognizerConfig {
    private targetLanguages: string[];

    constructor(
        platformConfig: SpeechConfig,
        recognitionMode: RecognitionMode = RecognitionMode.Conversation,
        language: string = "en-us",
        targetLanguages: string[] = ["de-de"],
        sdkProperties: ISpeechProperties) {
        super(platformConfig, recognitionMode, language, sdkProperties);

        this.targetLanguages = targetLanguages;
    }

    public get TargetLanguages(): string[] {
        return this.targetLanguages;
    }
}
