import { FileAudioSource, MicAudioSource, PcmRecorder } from "../../common.browser/Exports";
import { IAudioSource, Promise, Storage } from "../../common/Exports";
import { IAuthentication, Recognizer, RecognizerConfig } from "../speech/Exports";
import { SpeechConnectionFactory } from "./SpeechConnectionFactory";
import { SpeechFactory } from "./SpeechFactory";

const CreateRecognizer = (recognizerConfig: RecognizerConfig, authentication: IAuthentication): Recognizer => {
    return CreateRecognizerWithPcmRecorder(
        recognizerConfig,
        authentication);
};

const CreateRecognizerWithPcmRecorder = (recognizerConfig: RecognizerConfig, authentication: IAuthentication): Recognizer => {
    return CreateRecognizerWithCustomAudioSource(
        recognizerConfig,
        authentication,
        new MicAudioSource(new PcmRecorder()));
};

const CreateRecognizerWithFileAudioSource = (recognizerConfig: RecognizerConfig, authentication: IAuthentication, file: File): Recognizer => {
    return CreateRecognizerWithCustomAudioSource(
        recognizerConfig,
        authentication,
        new FileAudioSource(file));
};

const CreateRecognizerWithCustomAudioSource = (recognizerConfig: RecognizerConfig, authentication: IAuthentication, audioSource: IAudioSource): Recognizer => {
    return new Recognizer(
        authentication,
        new SpeechConnectionFactory(),
        audioSource,
        recognizerConfig);
};

export { CreateRecognizer, CreateRecognizerWithCustomAudioSource, CreateRecognizerWithFileAudioSource, CreateRecognizerWithPcmRecorder };
