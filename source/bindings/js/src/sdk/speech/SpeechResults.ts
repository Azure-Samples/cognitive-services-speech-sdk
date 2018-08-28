
export enum RecognitionStatus2 {
    Success,
    NoMatch,
    InitialSilenceTimeout,
    BabbleTimeout,
    Error,
    EndOfDictation,
}

export interface ISpeechStartDetectedResult {
    Offset?: number;
}

export interface ISpeechFragment {
    Text: string;
    Offset?: number;
    Duration?: number;
}

export interface ISpeechEndDetectedResult {
    Offset?: number;
}

export interface ISimpleSpeechPhrase {
    RecognitionStatus: RecognitionStatus2;
    DisplayText: string;
    Duration?: number;
    Offset?: number;
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
