//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "test.h"

@implementation EndToEndTests

+ (void)runTest
{
    NSBundle *mainBundle = [NSBundle mainBundle];
    NSString *weatherFile = [mainBundle pathForResource: @"whatstheweatherlike" ofType:@"wav"];
    NSLog(@"Main bundle path: %@", mainBundle);
    NSLog(@"weatherFile path: %@", weatherFile);
    NSString *lampFile = [mainBundle pathForResource: @"TurnOnTheLamp" ofType:@"wav"];
    AudioConfig* weatherAudioSource = [AudioConfig fromWavFileInput:weatherFile];
    AudioConfig* lampAudioSource = [AudioConfig fromWavFileInput:lampFile];
    
    NSString *speechKey = @"";
    NSString *intentKey = @"";
    
    __block bool end = false;
    
    SpeechConfig *speechConfig = [SpeechConfig fromSubscription:speechKey andRegion:@"westus"];
    SpeechRecognizer* speechRecognizer;
    
    // Test1: Use RecognizeOnce()
    speechRecognizer= [SpeechRecognizer fromConfig:speechConfig usingAudio:weatherAudioSource];
    NSString *speechRegion = [speechRecognizer.properties getPropertyByName:@"SPEECH-Region"];
    NSLog(@"Property Collection: Region is read from property collection: %@", speechRegion);
    SpeechRecognitionResult *speechResult = [speechRecognizer recognizeOnce];
    NSLog(@"RecognizeOnce: Recognition result %@. Status %ld.", speechResult.text, (long)speechResult.reason);
    
    // Test2: Use RecognizeOnceAsync() with completion block
    speechRecognizer = [SpeechRecognizer fromConfig:speechConfig usingAudio:weatherAudioSource];
    end = false;
    [speechRecognizer recognizeOnceAsync: ^ (SpeechRecognitionResult *result){
        NSLog(@"RecognizeOnceAsync: Recognition result %@. Status %ld.", result.text, (long)result.reason);
        end = true;
    }];
    while (end == false)
        [NSThread sleepForTimeInterval:1.0f];
    
    // Test3: Use StartContinuousRecognitionAsync()
    speechRecognizer = [SpeechRecognizer fromConfig:speechConfig usingAudio:weatherAudioSource];
    [speechRecognizer addSessionStartedEventListener: ^ (Recognizer *recognizer, SessionEventArgs *eventArgs) {
        NSLog(@"Received SessionStarted event. SessionId: %@", eventArgs.sessionId);
    }];
    [speechRecognizer addSessionStoppedEventListener: ^ (Recognizer *recognizer, SessionEventArgs *eventArgs) {
        NSLog(@"Received SessionStopped event. SessionId: %@", eventArgs.sessionId);
        end = true;
    }];
    [speechRecognizer addSpeechStartDetectedEventListener: ^ (Recognizer *recognizer, RecognitionEventArgs *eventArgs) {
        NSLog(@"Received SpeechStarted event. SessionId: %@ Offset: %d", eventArgs.sessionId, (int)eventArgs.offset);
    }];
    [speechRecognizer addSpeechEndDetectedEventListener: ^ (Recognizer *recognizer, RecognitionEventArgs *eventArgs) {
        NSLog(@"Received SpeechEnd event. SessionId: %@ Offset: %d", eventArgs.sessionId, (int)eventArgs.offset);
    }];
    [speechRecognizer addRecognizedEventListener: ^ (SpeechRecognizer *recognizer, SpeechRecognitionEventArgs *eventArgs) {
        NSLog(@"Received final result event. SessionId: %@, recognition result:%@. Status %ld.", eventArgs.sessionId, eventArgs.result.text, (long)eventArgs.result.reason);
        NSLog(@"Received JSON: %@", [eventArgs.result.properties getPropertyByName:@"RESULT-Json"]);
    }];
    [speechRecognizer addRecognizingEventListener: ^ (SpeechRecognizer *recognizer, SpeechRecognitionEventArgs *eventArgs) {
        NSLog(@"Received intermediate result event. SessionId: %@, intermediate result:%@.", eventArgs.sessionId, eventArgs.result.text);
    }];
    [speechRecognizer addCanceledEventListener: ^ (SpeechRecognizer *recognizer, SpeechRecognitionCanceledEventArgs *eventArgs) {
        NSLog(@"Received canceled event. SessionId: %@, reason:%lu errorDetails:%@.", eventArgs.sessionId, (unsigned long)eventArgs.reason, eventArgs.errorDetails);
        CancellationDetails *details = [CancellationDetails fromResult:eventArgs.result];
        NSLog(@"Received cancellation details. reason:%lu errorDetails:%@.", (unsigned long)details.reason, details.errorDetails);
    }];
    end = false;
    [speechRecognizer startContinuousRecognition];
    while (end == false)
        [NSThread sleepForTimeInterval:1.0f];
    [speechRecognizer stopContinuousRecognition];

    // Test: Translation
    TranslationRecognizer *translationRecognizer;
    SpeechTranslationConfig *translationConfig = [SpeechTranslationConfig fromSubscription:speechKey andRegion:@"westus"];
    [translationConfig setSpeechRecognitionLanguage:@"en-us"];
    [translationConfig addTargetLanguage:@"de"];
    [translationConfig addTargetLanguage:@"zh-Hans"];
    [translationConfig setVoiceName:@"de-DE-Hedda"];

    //Test1: Use RecognizeOnce()
    translationRecognizer = [TranslationRecognizer fromConfig:translationConfig usingAudio:weatherAudioSource];
    NSString *translationVoice = [translationRecognizer.properties getPropertyByName:@"TRANSLATION-Voice"];
    NSLog(@"Property Collection: TranslationVoice is read from property collection: %@", translationVoice);
    TranslationTextResult *translationResult = [translationRecognizer recognizeOnce];
    NSLog(@"RecognizeOnce: Translation result: recognized: %@. Status %ld.", translationResult.text, (long)translationResult.reason);
    for (id lang in [translationResult.translations allKeys]) {
        NSLog(@"RecognizeOnce: Translation result: translated into %@: %@", lang, [translationResult.translations objectForKey:lang]);
    }
    
    // Test2: Use RecognizeOnceAsync() with completion block
    translationRecognizer = [TranslationRecognizer fromConfig:translationConfig usingAudio:weatherAudioSource];
    end = false;
    [translationRecognizer recognizeOnceAsync: ^ (TranslationTextResult *result){
        NSLog(@"RecognizeOnceAsync: Translation result: recognized: %@. Status %ld.", result.text, (long)result.reason);
        for (id lang in [result.translations allKeys]) {
            NSLog(@"RecognizeOnceAsync: Translation result: translated into %@: %@", lang, [result.translations objectForKey:lang]);
        }
        end = true;
    }];
    while (end == false)
        [NSThread sleepForTimeInterval:1.0f];


    // Test3: Use StartContinuousRecognitionAsync()
    translationRecognizer = [TranslationRecognizer fromConfig:translationConfig usingAudio:weatherAudioSource];
    [translationRecognizer addSessionStartedEventListener: ^ (Recognizer *recognizer, SessionEventArgs *eventArgs) {
        NSLog(@"Received SessionStarted event. SessionId: %@", eventArgs.sessionId);
    }];
    [translationRecognizer addSessionStoppedEventListener: ^ (Recognizer *recognizer, SessionEventArgs *eventArgs) {
        NSLog(@"Received SessionStopped event. SessionId: %@", eventArgs.sessionId);
        end = true;
    }];
    [translationRecognizer addSpeechStartDetectedEventListener: ^ (Recognizer *recognizer, RecognitionEventArgs *eventArgs) {
        NSLog(@"Received SpeechStarted event. SessionId: %@ Offset: %d", eventArgs.sessionId, (int)eventArgs.offset);
    }];
    [translationRecognizer addSpeechEndDetectedEventListener: ^ (Recognizer *recognizer, RecognitionEventArgs *eventArgs) {
        NSLog(@"Received SpeechEnd event. SessionId: %@ Offset: %d", eventArgs.sessionId, (int)eventArgs.offset);
    }];
    [translationRecognizer addRecognizedEventListener: ^ (TranslationRecognizer *recognizer, TranslationTextResultEventArgs *eventArgs) {
        NSLog(@"Received final result event. SessionId: %@, recognition result:%@. Status %ld.", eventArgs.sessionId, eventArgs.result.text, (long)eventArgs.result.reason);
        for (id lang in [eventArgs.result.translations allKeys]) {
            NSLog(@"Translation result: translated into %@: %@", lang, [eventArgs.result.translations objectForKey:lang]);
        }
    }];
    [translationRecognizer addRecognizingEventListener: ^ (TranslationRecognizer *recognizer, TranslationTextResultEventArgs *eventArgs) {
        NSLog(@"Received intermediate result event. SessionId: %@, intermediate result:%@.", eventArgs.sessionId, eventArgs.result.text);
        for (id lang in [eventArgs.result.translations allKeys]) {
            NSLog(@"Translation result: translated into %@: %@", lang, [eventArgs.result.translations objectForKey:lang]);
        }
        NSLog(@"Received JSON: %@", [eventArgs.result.properties getPropertyByName:@"RESULT-Json"]);
    }];
    [translationRecognizer addSynthesizingEventListener: ^ (TranslationRecognizer * recognizer, TranslationSynthesisResultEventArgs *eventArgs) {
        NSLog(@"Received synthesis result event. SessionId: %@, audio length:%lu.", eventArgs.sessionId, (unsigned long)[eventArgs.result.audio length]);
    }];
    end = false;
    [translationRecognizer startContinuousRecognition];
    while (end == false)
        [NSThread sleepForTimeInterval:1.0f];
    [translationRecognizer stopContinuousRecognition];
    
    // Test: Intent
    SpeechConfig *intentConfig = [SpeechConfig fromSubscription:intentKey andRegion:@"westus"];
    IntentRecognizer *intentRecognizer;
    LanguageUnderstandingModel *model = [LanguageUnderstandingModel FromAppId:@"b687b851-56c5-4d31-816f-35a741a3f0be"];
    
    // Test1: Use RecognizeOnce()
    intentRecognizer = [IntentRecognizer fromConfig:intentConfig usingAudio:lampAudioSource];
    
    [intentRecognizer addIntent:@"HomeAutomation.TurnOn" fromModel:model mappingToId:@"MyIntentRecognizeOnce" ] ;
    IntentRecognitionResult *result = [intentRecognizer recognizeOnce];
    NSLog(@"RecognizeOnce: Intent result: recognized: %@. Status %ld. IntentId %@.", result.text, (long)result.reason, result.intentId);
    
    // Test2: Use RecognizeOnceAsync() with completion block
    intentRecognizer = [IntentRecognizer fromConfig:intentConfig usingAudio:lampAudioSource];
    [intentRecognizer addIntent:@"HomeAutomation.TurnOn" fromModel:model];
    end = false;
    [intentRecognizer recognizeOnceAsync: ^ (IntentRecognitionResult *result){
        NSLog(@"RecognizeOnceAsnc: Intent result: recognized: %@. Status %ld. IntentId %@", result.text, (long)result.reason, result.intentId);
        end = true;
    }];
    while (end == false)
        [NSThread sleepForTimeInterval:1.0f];

    // Test3: Use StartContinuousRecognitionAsync()
    intentRecognizer = [IntentRecognizer fromConfig:intentConfig usingAudio:lampAudioSource];
    // BUGUG: addAllIntents throw exception at c++
    // [intentRecognizer addAllIntentsFromModel:model mappingToId:@"AllIntentInHomeAutomation"];
    [intentRecognizer addIntent:@"HomeAutomation.TurnOn" fromModel:model];
    NSString *intentRegion = [intentRecognizer.properties getPropertyByName:@"INTENT-region"];
    NSLog(@"PropertyCollection: Region is read from property collection: %@", intentRegion);
    [intentRecognizer addSessionStartedEventListener: ^ (Recognizer *recognizer, SessionEventArgs *eventArgs) {
        NSLog(@"Received SessionStarted event. SessionId: %@", eventArgs.sessionId);
    }];
    [intentRecognizer addSessionStoppedEventListener: ^ (Recognizer *recognizer, SessionEventArgs *eventArgs) {
        NSLog(@"Received SessionStopped event. SessionId: %@", eventArgs.sessionId);
        end = true;
    }];
    [intentRecognizer addSpeechStartDetectedEventListener: ^ (Recognizer *recognizer, RecognitionEventArgs *eventArgs) {
        NSLog(@"Received SpeechStarted event. SessionId: %@ Offset: %d", eventArgs.sessionId, (int)eventArgs.offset);
    }];
    [intentRecognizer addSpeechEndDetectedEventListener: ^ (Recognizer *recognizer, RecognitionEventArgs *eventArgs) {
        NSLog(@"Received SpeechEnd event. SessionId: %@ Offset: %d", eventArgs.sessionId, (int)eventArgs.offset);
    }];
    [intentRecognizer addRecognizedEventListener: ^ (IntentRecognizer * recognizer, IntentRecognitionEventArgs *eventArgs) {
        NSLog(@"Received final result event. SessionId: %@, recognition result:%@. Status %ld. IntentId %@.", eventArgs.sessionId, eventArgs.result.text, (long)eventArgs.result.reason, eventArgs.result.intentId);
        NSString *intentRegion2 = [recognizer.properties getPropertyByName:@"INTENT-region"];
        NSLog(@"Property Collection: Region is read from property collection: %@", intentRegion2);
        NSLog(@"Received Result JSON: %@", [eventArgs.result.properties getPropertyByName:@"RESULT-Json"]);
        // BUBGUG: LUIS is not back in Core
        NSLog(@"Received LUIS JSON: %@", [eventArgs.result.properties getPropertyByName:@"RESULT-LanguageUnderstandingJson"]);
    }];
    [intentRecognizer addRecognizingEventListener: ^ (IntentRecognizer * recognizer, IntentRecognitionEventArgs *eventArgs) {
        NSLog(@"Received intermediate result event. SessionId: %@, intermediate result:%@. IntentId %@", eventArgs.sessionId, eventArgs.result.text, eventArgs.result.intentId);
    }];
    end = false;
    [intentRecognizer startContinuousRecognition];
    while (end == false)
        [NSThread sleepForTimeInterval:1.0f];
    [intentRecognizer stopContinuousRecognition];
    
    return;
}

@end
