//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "test.h"

@implementation EndToEndTests

+ (void)runTest:(NSString *)speechKey withRegion:(NSString *)serviceRegion withIntentKey:(NSString *)intentKey withIntentRegion:(NSString*)intentRegion
{
    NSBundle *mainBundle = [NSBundle mainBundle];
    NSString *weatherFile = [mainBundle pathForResource: @"whatstheweatherlike" ofType:@"wav"];
    NSLog(@"Main bundle path: %@", mainBundle);
    NSLog(@"weatherFile path: %@", weatherFile);
    NSString *lampFile = [mainBundle pathForResource: @"TurnOnTheLamp" ofType:@"wav"];
    SPXAudioConfiguration* weatherAudioSource = [[SPXAudioConfiguration alloc] initWithWavFileInput:weatherFile];
    SPXAudioConfiguration* lampAudioSource = [[SPXAudioConfiguration alloc] initWithWavFileInput:lampFile];

    __block bool end = false;

    SPXSpeechConfiguration *speechConfig = [[SPXSpeechConfiguration alloc] initWithSubscription:speechKey region:serviceRegion];
    SPXSpeechRecognizer* speechRecognizer;
    NSString *speechRegion = [speechConfig getPropertyByName:@"SPEECH-Region"];
    NSLog(@"Speech Config: Region is read from speech config: %@", speechRegion);

    [speechConfig setPropertyTo:@"de-DE" byId:SPXSpeechServiceConnectionRecognitionLanguage];
    NSString *speechRecoLang = [speechConfig getPropertyByName:@"SPEECH-RecoLanguage"];
    NSLog(@"Speech Config: RecoLang is read from speech config: %@", speechRecoLang);
    [speechConfig setPropertyTo:@"en-US" byName:@"SPEECH-RecoLanguage"];


    // Test1: Use RecognizeOnce()

    speechRecognizer= [[SPXSpeechRecognizer alloc] initWithSpeechConfiguration:speechConfig audioConfiguration:weatherAudioSource];
    speechRegion = [speechRecognizer.properties getPropertyById:SPXSpeechServiceConnectionRegion defaultValue:@"Unknown speech region"];
    NSLog(@"Property Collection: Region is read from property collection: %@", speechRegion);
    speechRecoLang = [speechRecognizer.properties getPropertyById:SPXSpeechServiceConnectionRecognitionLanguage];
    NSLog(@"Property Collection: RecoLang is read from property collection: %@", speechRecoLang);

    SPXSpeechRecognitionResult *speechResult = [speechRecognizer recognizeOnce];
    NSLog(@"RecognizeOnce: Recognition result %@. Status %ld, Offset %llu Duration %llu.", speechResult.text, (long)speechResult.reason, speechResult.offset, speechResult.duration);
    if (speechResult.reason == SPXResultReason_Canceled) {
     SPXCancellationDetails *details = [[SPXCancellationDetails alloc] initFromCanceledRecognitionResult:speechResult];
     NSLog(@"Received cancellation details. reason:%lu errorDetails:%@.", (unsigned long)details.reason, details.errorDetails);
    }

    // Test2: Use RecognizeOnceAsync() with completion block
    speechRecognizer = [[SPXSpeechRecognizer alloc] initWithSpeechConfiguration:speechConfig audioConfiguration:weatherAudioSource];

    end = false;
    [speechRecognizer recognizeOnceAsync: ^ (SPXSpeechRecognitionResult *result){
        NSLog(@"RecognizeOnceAsync: Recognition result %@. Status %ld.", result.text, (long)result.reason);
        end = true;
    }];
    while (end == false)
        [NSThread sleepForTimeInterval:1.0f];

    // Test3: Use StartContinuousRecognitionAsync()
    speechRecognizer = [[SPXSpeechRecognizer alloc] initWithSpeechConfiguration:speechConfig audioConfiguration:weatherAudioSource];
    [speechRecognizer addSessionStartedEventHandler: ^ (SPXRecognizer *recognizer, SPXSessionEventArgs *eventArgs) {
        NSLog(@"Received SessionStarted event. SessionId: %@", eventArgs.sessionId);
    }];
    [speechRecognizer addSessionStoppedEventHandler: ^ (SPXRecognizer *recognizer, SPXSessionEventArgs *eventArgs) {
        NSLog(@"Received SessionStopped event. SessionId: %@", eventArgs.sessionId);
        end = true;
    }];
    [speechRecognizer addSpeechStartDetectedEventHandler: ^ (SPXRecognizer *recognizer, SPXRecognitionEventArgs *eventArgs) {
        NSLog(@"Received SpeechStarted event. SessionId: %@ Offset: %d", eventArgs.sessionId, (int)eventArgs.offset);
    }];
    [speechRecognizer addSpeechEndDetectedEventHandler: ^ (SPXRecognizer *recognizer, SPXRecognitionEventArgs *eventArgs) {
        NSLog(@"Received SpeechEnd event. SessionId: %@ Offset: %d", eventArgs.sessionId, (int)eventArgs.offset);
    }];
    [speechRecognizer addRecognizedEventHandler: ^ (SPXSpeechRecognizer *recognizer, SPXSpeechRecognitionEventArgs *eventArgs) {
        NSLog(@"Received final result event. SessionId: %@, recognition result:%@. Status %ld. offset %llu duration %llu resultid:%@", eventArgs.sessionId, eventArgs.result.text, (long)eventArgs.result.reason, eventArgs.result.offset, eventArgs.result.duration, eventArgs.result.resultId);
        NSLog(@"Received JSON: %@", [eventArgs.result.properties getPropertyById:SPXSpeechServiceResponseJsonResult]);
    }];
    [speechRecognizer addRecognizingEventHandler: ^ (SPXSpeechRecognizer *recognizer, SPXSpeechRecognitionEventArgs *eventArgs) {
        NSLog(@"Received intermediate result event. SessionId: %@, intermediate result:%@.", eventArgs.sessionId, eventArgs.result.text);
    }];

    [speechRecognizer addCanceledEventHandler: ^ (SPXSpeechRecognizer *recognizer, SPXSpeechRecognitionCanceledEventArgs *eventArgs) {
        NSLog(@"Received canceled event. SessionId: %@, reason:%lu errorDetails:%@.", eventArgs.sessionId, (unsigned long)eventArgs.reason, eventArgs.errorDetails);
        SPXCancellationDetails *details = [[SPXCancellationDetails alloc] initFromCanceledRecognitionResult:eventArgs.result];
        NSLog(@"Received cancellation details. reason:%lu errorDetails:%@.", (unsigned long)details.reason, details.errorDetails);
    }];

    end = false;
    [speechRecognizer startContinuousRecognition];
    while (end == false)
        [NSThread sleepForTimeInterval:1.0f];
    [speechRecognizer stopContinuousRecognition];

    // Test: Translation
    SPXTranslationRecognizer *translationRecognizer;
    SPXSpeechTranslationConfiguration *translationConfig = [[SPXSpeechTranslationConfiguration alloc] initWithSubscription:speechKey region:serviceRegion];
    [translationConfig setSpeechRecognitionLanguage:@"en-us"];
    [translationConfig addTargetLanguage:@"de"];
    [translationConfig addTargetLanguage:@"zh-Hans"];
    [translationConfig setVoiceName:@"Microsoft Server Speech Text to Speech Voice (de-DE, Hedda)"];

    NSMutableString *targetLangsStr = [[NSMutableString alloc] init];
    for (id item in translationConfig.targetLanguages)
    {
        [targetLangsStr appendString:(NSString *)item];
        [targetLangsStr appendString:@","];
    }
    NSLog(@"Translation Configuration: RecoLanguage: %@, targetLangs: %@, voice:%@", translationConfig.speechRecognitionLanguage, targetLangsStr, translationConfig.voiceName);

    //Test1: Use RecognizeOnce()
    translationRecognizer = [[SPXTranslationRecognizer alloc] initWithSpeechTranslationConfiguration:translationConfig audioConfiguration:weatherAudioSource];
    NSString *translationVoice = [translationRecognizer.properties getPropertyByName:@"TRANSLATION-Voice"];
    NSLog(@"Property Collection: TranslationVoice is read from property collection: %@", translationVoice);
    SPXTranslationRecognitionResult *translationResult = [translationRecognizer recognizeOnce];
    NSLog(@"RecognizeOnce: Translation result: recognized: %@. Status %ld.", translationResult.text, (long)translationResult.reason);
    for (id lang in [translationResult.translations allKeys]) {
        NSLog(@"RecognizeOnce: Translation result: translated into %@: %@", lang, [translationResult.translations objectForKey:lang]);
    }

    // Test2: Use RecognizeOnceAsync() with completion block
    translationRecognizer = [[SPXTranslationRecognizer alloc] initWithSpeechTranslationConfiguration:translationConfig audioConfiguration:weatherAudioSource];
    end = false;
    [translationRecognizer recognizeOnceAsync: ^ (SPXTranslationRecognitionResult *result){
        NSLog(@"RecognizeOnceAsync: Translation result: recognized: %@. Status %ld.", result.text, (long)result.reason);
        for (id lang in [result.translations allKeys]) {
            NSLog(@"RecognizeOnceAsync: Translation result: translated into %@: %@", lang, [result.translations objectForKey:lang]);
        }
        end = true;
    }];
    while (end == false)
        [NSThread sleepForTimeInterval:1.0f];


    // Test3: Use StartContinuousRecognitionAsync()
    translationRecognizer = [[SPXTranslationRecognizer alloc] initWithSpeechTranslationConfiguration:translationConfig audioConfiguration:weatherAudioSource];;
    [translationRecognizer addSessionStartedEventHandler: ^ (SPXRecognizer *recognizer, SPXSessionEventArgs *eventArgs) {
        NSLog(@"Received SessionStarted event. SessionId: %@", eventArgs.sessionId);
    }];
    [translationRecognizer addSessionStoppedEventHandler: ^ (SPXRecognizer *recognizer, SPXSessionEventArgs *eventArgs) {
        NSLog(@"Received SessionStopped event. SessionId: %@", eventArgs.sessionId);
        end = true;
    }];
    [translationRecognizer addSpeechStartDetectedEventHandler: ^ (SPXRecognizer *recognizer, SPXRecognitionEventArgs *eventArgs) {
        NSLog(@"Received SpeechStarted event. SessionId: %@ Offset: %llu", eventArgs.sessionId, eventArgs.offset);
    }];
    [translationRecognizer addSpeechEndDetectedEventHandler: ^ (SPXRecognizer *recognizer, SPXRecognitionEventArgs *eventArgs) {
        NSLog(@"Received SpeechEnd event. SessionId: %@ Offset: %llu", eventArgs.sessionId, eventArgs.offset);
    }];
    [translationRecognizer addRecognizedEventHandler: ^ (SPXTranslationRecognizer *recognizer, SPXTranslationRecognitionEventArgs *eventArgs) {
        NSLog(@"Received final result event. SessionId: %@, recognition result:%@. Status %ld.", eventArgs.sessionId, eventArgs.result.text, (long)eventArgs.result.reason);
        for (id lang in [eventArgs.result.translations allKeys]) {
            NSLog(@"Translation result: translated into %@: %@", lang, [eventArgs.result.translations objectForKey:lang]);
        }
    }];
    [translationRecognizer addRecognizingEventHandler: ^ (SPXTranslationRecognizer *recognizer, SPXTranslationRecognitionEventArgs *eventArgs) {
        NSLog(@"Received intermediate result event. SessionId: %@, intermediate result:%@ offset %llu duration %llu resultid:%@.", eventArgs.sessionId, eventArgs.result.text, eventArgs.result.offset, eventArgs.result.duration, eventArgs.result.resultId);
        for (id lang in [eventArgs.result.translations allKeys]) {
            NSLog(@"Translation result: translated into %@: %@", lang, [eventArgs.result.translations objectForKey:lang]);
        }
        NSLog(@"Received JSON: %@", [eventArgs.result.properties getPropertyByName:@"RESULT-Json"]);
    }];
    [translationRecognizer addSynthesizingEventHandler: ^ (SPXTranslationRecognizer * recognizer, SPXTranslationSynthesisEventArgs *eventArgs) {
        NSLog(@"Received synthesis result event. SessionId: %@, Reason:%d audio length:%lu.", eventArgs.sessionId, (int)eventArgs.result.reason, (unsigned long)[eventArgs.result.audio length]);
    }];
    [translationRecognizer addCanceledEventHandler: ^ (SPXTranslationRecognizer *recognizer, SPXTranslationRecognitionCanceledEventArgs *eventArgs) {
        NSLog(@"Received canceled event. SessionId: %@, reason:%lu errorDetails:%@.", eventArgs.sessionId, (unsigned long)eventArgs.reason, eventArgs.errorDetails);
        SPXCancellationDetails *details = [[SPXCancellationDetails alloc] initFromCanceledRecognitionResult:eventArgs.result];
        NSLog(@"Received cancellation details. reason:%lu errorDetails:%@.", (unsigned long)details.reason, details.errorDetails);
    }];
    end = false;
    [translationRecognizer startContinuousRecognition];
    while (end == false)
        [NSThread sleepForTimeInterval:1.0f];
    [translationRecognizer stopContinuousRecognition];

    // Test: Intent
    SPXSpeechConfiguration *intentConfig = [[SPXSpeechConfiguration alloc] initWithSubscription:intentKey region:intentRegion];
    SPXIntentRecognizer *intentRecognizer;
    SPXLanguageUnderstandingModel *model = [[SPXLanguageUnderstandingModel alloc] initWithAppId:@"b687b851-56c5-4d31-816f-35a741a3f0be"];

    // test intent recognizer for completeness
    SPXIntentRecognizer *intentRecoTest = [[SPXIntentRecognizer alloc] init:intentConfig];
    [intentRecoTest addIntentFromPhrase:@"SimplePhrase"];
    [intentRecoTest addIntentFromPhrase:@"SimplePhrase2" mappingToId:@"TestIntent1"];
    [intentRecoTest addIntent:@"HomeAutomation.TurnOn" fromModel:model];
    [intentRecoTest addIntent:@"HomeAutomation.TurnOn" fromModel:model mappingToId:@"TestIntent2"];
    [intentRecoTest addAllIntentsFromModel:model];
    [intentRecoTest addAllIntentsFromModel:model mappingToId:@"LocalAllIntents"];


    // Test1: Use RecognizeOnce()

    intentRecognizer = [[SPXIntentRecognizer alloc] initWithSpeechConfiguration:intentConfig audioConfiguration:lampAudioSource];

    [intentRecognizer addIntent:@"HomeAutomation.TurnOn" fromModel:model mappingToId:@"MyIntentRecognizeOnce" ] ;
    SPXIntentRecognitionResult *result = [intentRecognizer recognizeOnce];
    NSLog(@"RecognizeOnce: Intent result: recognized: %@. Status %ld. IntentId %@.", result.text, (long)result.reason, result.intentId);

    // Test2: Use RecognizeOnceAsync() with completion block
    intentRecognizer = [[SPXIntentRecognizer alloc] initWithSpeechConfiguration:intentConfig audioConfiguration:lampAudioSource];
    [intentRecognizer addIntent:@"HomeAutomation.TurnOn" fromModel:model];
    end = false;
    [intentRecognizer recognizeOnceAsync: ^ (SPXIntentRecognitionResult *result){
        NSLog(@"RecognizeOnceAsnc: Intent result: recognized: %@. Status %ld. IntentId %@", result.text, (long)result.reason, result.intentId);
        end = true;
    }];
    while (end == false)
        [NSThread sleepForTimeInterval:1.0f];

    // Test3: Use StartContinuousRecognitionAsync()
    intentRecognizer = [[SPXIntentRecognizer alloc] initWithSpeechConfiguration:intentConfig audioConfiguration:lampAudioSource];
    [intentRecognizer addAllIntentsFromModel:model mappingToId:@"AllIntentInHomeAutomation"];
    [intentRecognizer addIntent:@"HomeAutomation.TurnOn" fromModel:model];

    NSString *invalidIntentRegion = [intentRecognizer.properties getPropertyByName:@"INTENT-region" defaultValue:@"unknown region"];
    NSLog(@"PropertyCollection: Region is read from property collection: %@", invalidIntentRegion);
    [intentRecognizer addSessionStartedEventHandler: ^ (SPXRecognizer *recognizer, SPXSessionEventArgs *eventArgs) {
        NSLog(@"Received SessionStarted event. SessionId: %@", eventArgs.sessionId);
    }];
    [intentRecognizer addSessionStoppedEventHandler: ^ (SPXRecognizer *recognizer, SPXSessionEventArgs *eventArgs) {
        NSLog(@"Received SessionStopped event. SessionId: %@", eventArgs.sessionId);
        end = true;
    }];
    [intentRecognizer addSpeechStartDetectedEventHandler: ^ (SPXRecognizer *recognizer, SPXRecognitionEventArgs *eventArgs) {
        NSLog(@"Received SpeechStarted event. SessionId: %@ Offset: %d", eventArgs.sessionId, (int)eventArgs.offset);
    }];
    [intentRecognizer addSpeechEndDetectedEventHandler: ^ (SPXRecognizer *recognizer, SPXRecognitionEventArgs *eventArgs) {
        NSLog(@"Received SpeechEnd event. SessionId: %@ Offset: %d", eventArgs.sessionId, (int)eventArgs.offset);
    }];
    [intentRecognizer addRecognizedEventHandler: ^ (SPXIntentRecognizer * recognizer, SPXIntentRecognitionEventArgs *eventArgs) {
        NSLog(@"Received final result event. SessionId: %@, recognition result:%@. Status %ld. IntentId %@.", eventArgs.sessionId, eventArgs.result.text, (long)eventArgs.result.reason, eventArgs.result.intentId);
        NSString *intentRegion2 = [recognizer.properties getPropertyByName:@"INTENT-region"];
        NSLog(@"Property Collection: Region is read from property collection: %@", intentRegion2);
        NSLog(@"Received Result JSON: %@", [eventArgs.result.properties getPropertyByName:@"RESULT-Json"]);
        // BUBGUG: LUIS is not back in Core
        NSLog(@"Received LUIS JSON: %@", [eventArgs.result.properties getPropertyByName:@"RESULT-LanguageUnderstandingJson"]);
    }];
    [intentRecognizer addRecognizingEventHandler: ^ (SPXIntentRecognizer * recognizer, SPXIntentRecognitionEventArgs *eventArgs) {
        NSLog(@"Received intermediate result event. SessionId: %@, intermediate result:%@. reason: %ld. IntentId %@", eventArgs.sessionId, eventArgs.result.text, (long)eventArgs.result.reason, eventArgs.result.intentId);
    }];
    [intentRecognizer addCanceledEventHandler: ^ (SPXIntentRecognizer *recognizer, SPXIntentRecognitionCanceledEventArgs *eventArgs) {
        NSLog(@"Received canceled event. SessionId: %@, reason:%lu errorDetails:%@.", eventArgs.sessionId, (unsigned long)eventArgs.reason, eventArgs.errorDetails);
        SPXCancellationDetails *details = [[SPXCancellationDetails alloc] initFromCanceledRecognitionResult:eventArgs.result];
        NSLog(@"Received cancellation details. reason:%lu errorDetails:%@.", (unsigned long)details.reason, details.errorDetails);
    }];
    end = false;
    [intentRecognizer startContinuousRecognition];
    while (end == false)
        [NSThread sleepForTimeInterval:1.0f];
    [intentRecognizer stopContinuousRecognition];

    return;
}

@end
