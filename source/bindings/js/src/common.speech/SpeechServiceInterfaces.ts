import { RecognitionCompletionStatus } from "../../src/common.speech/Exports";
import { SynthesisStatus } from "../sdk/SynthesisStatus";
import { TranslationStatus } from "../sdk/TranslationStatus";

export enum RecognitionStatus2 {
    Success,
    NoMatch,
    InitialSilenceTimeout,
    BabbleTimeout,
    Error,
    EndOfDictation,
}

// speech.endDetected
export interface ISpeechStartDetected {
    Offset?: number;
}

// speech.hypothesis
export interface ISpeechHypothesis {
    Text: string;
    Offset?: number;
    Duration?: number;
}

export interface ITranslations {
    TranslationStatus: TranslationStatus;
    Translations: ITranslation[];
    FailureReason: string;
}

export interface ITranslation {
    Language: string;
    Text: string;
}

export interface ISpeechEndDetectedResult {
    Offset?: number;
}

// speech.phrase
export interface ISimpleSpeechPhrase {
    RecognitionStatus: RecognitionStatus2;
    DisplayText: string;
    Offset?: number;
    Duration?: number;
}

// translation.hypothesis
export interface ITranslationHypothesis {
    Duration: number;
    Offset: number;
    Text: string;
    Translation: ITranslations;
}

// translation.phrase
export interface ITranslationPhrase {
    RecognitionStatus: RecognitionStatus2;
    Offset: number;
    Duration: number;
    Text: string;
    Translation: ITranslations;
}

export interface IDetailedSpeechPhrase {
    RecognitionStatus: RecognitionStatus2;
    NBest: IPhrase[];
    Duration?: number;
    Offset?: number;
}

export interface IPhrase {
    Confidence?: number;
    Lexical: string;
    ITN: string;
    MaskedITN: string;
    Display: string;
}

// translation.synthesis.end
export interface ITranslationSynthesisEnd {
    SynthesisStatus: SynthesisStatus;
    FailureReason: string;
}

// response
export interface IIntentResponse {
    query: string;
    topScoringIntent: ISingleIntent;
    entities: IIntentEntity[];
}

export interface IIntentEntity {
    entity: string;
    type: string;
    startIndex: number;
    endIndex: number;
    score: number;
}

export interface ISingleIntent {
    intent: string;
    score: number;
}

// turn.start
export interface ITurnStart {
    context: ITurnStartContext;
}

export interface ITurnStartContext {
    serviceTag: string;
}

export interface IResultErrorDetails {
    errorText: string;
    recogSate: RecognitionCompletionStatus;
}
