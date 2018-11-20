// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

// Make sure not to export internal modules.
//
export * from "./CognitiveSubscriptionKeyAuthentication";
export * from "./CognitiveTokenAuthentication";
export * from "./IAuthentication";
export * from "./IConnectionFactory";
export * from "./IntentConnectionFactory";
export * from "./RecognitionEvents";
export * from "./ServiceRecognizerBase";
export * from "./RecognizerConfig";
export * from "./SpeechServiceInterfaces";
export * from "./WebsocketMessageFormatter";
export * from "./SpeechConnectionFactory";
export * from "./TranslationConnectionFactory";
export * from "./EnumTranslation";
export * from "./ServiceMessages/Enums";
export * from "./ServiceMessages/TranslationSynthesisEnd";
export * from "./ServiceMessages/TranslationHypothesis";
export * from "./ServiceMessages/TranslationPhrase";
export * from "./TranslationServiceRecognizer";
export * from "./ServiceMessages/SpeechDetected";
export * from "./ServiceMessages/SpeechHypothesis";
export * from "./SpeechServiceRecognizer";
export * from "./ServiceMessages/DetailedSpeechPhrase";
export * from "./ServiceMessages/SimpleSpeechPhrase";
export * from "./AddedLmIntent";
export * from "./IntentServiceRecognizer";
export * from "./ServiceMessages/IntentResponse";
export * from "./RequestSession";

export const OutputFormatPropertyName: string = "OutputFormat";
export const CancellationErrorCodePropertyName: string = "CancellationErrorCode";
