import { Context, RecognitionMode, RecognizerConfig, SpeechConfig, SpeechResultFormat } from "./RecognizerConfig";

export class TranslationConfig extends RecognizerConfig {
    private targetLanguages: string[];

    constructor(
        platformConfig: SpeechConfig,
        recognitionMode: RecognitionMode = RecognitionMode.Conversation,
        language: string = "en-us",
        targetLanguages: string[] = ["de-de"],
        format: SpeechResultFormat = SpeechResultFormat.Simple) {
        super(platformConfig, recognitionMode, language, format);

        this.targetLanguages = targetLanguages;
    }

    public get TargetLanguages(): string[] {
        return this.targetLanguages;
    }
}
