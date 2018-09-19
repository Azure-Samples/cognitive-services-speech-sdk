//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "microphone_test.h"

@implementation MicrophoneTest
NSString *speechKey = @"";
NSString *serviceRegion = @"";

+ (void) runAsync
{
    // NSString *intentKey = @"";
    
    SpeechConfig *speechConfig = [SpeechConfig fromSubscription:speechKey andRegion:serviceRegion];
    SpeechRecognizer* speechRecognizer;
    
    speechRecognizer= [SpeechRecognizer fromConfig:speechConfig];
    NSString *speechRegion = [speechRecognizer.properties getPropertyByName:@"SPEECH-Region"];
    NSLog(@"Property Collection: Region is read from property collection: %@", speechRegion);
    SpeechRecognitionResult *speechResult = [speechRecognizer recognizeOnce];
    NSLog(@"RecognizeOnce: Recognition result %@. Status %ld.", speechResult.text, (long)speechResult.reason);
}

+ (void) runContinuous
{
    SpeechConfig *speechConfig = [SpeechConfig fromSubscription:speechKey andRegion:serviceRegion];
    SpeechRecognizer* speechRecognizer;
    
    speechRecognizer= [SpeechRecognizer fromConfig:speechConfig];

    __block bool end = false;

    [speechRecognizer addSessionStartedEventListener: ^ (Recognizer *recognizer, SessionEventArgs *eventArgs) {
//        NSLog(@"Received SessionStarted event. SessionId: %@", eventArgs.sessionId);
    }];
    [speechRecognizer addSessionStoppedEventListener: ^ (Recognizer *recognizer, SessionEventArgs *eventArgs) {
//        NSLog(@"Received SessionStopped event. SessionId: %@", eventArgs.sessionId);
    }];
    [speechRecognizer addSpeechStartDetectedEventListener: ^ (Recognizer *recognizer, RecognitionEventArgs *eventArgs) {
//        NSLog(@"Received SpeechStarted event. SessionId: %@ Offset: %d", eventArgs.sessionId, (int)eventArgs.offset);
    }];
    [speechRecognizer addSpeechEndDetectedEventListener: ^ (Recognizer *recognizer, RecognitionEventArgs *eventArgs) {
        NSLog(@"Received SpeechEnd event. SessionId: %@ Offset: %d", eventArgs.sessionId, (int)eventArgs.offset);
        end = true;
    }];
    [speechRecognizer addRecognizedEventListener: ^ (SpeechRecognizer *recognizer, SpeechRecognitionEventArgs *eventArgs) {
        NSLog(@"Received final result event. SessionId: %@, recognition result:%@. Status %ld.", eventArgs.sessionId, eventArgs.result.text, (long)eventArgs.result.reason);
//        NSLog(@"Received JSON: %@", [eventArgs.result.properties getPropertyByName:@"RESULT-Json"]);
    }];
    [speechRecognizer addRecognizingEventListener: ^ (SpeechRecognizer *recognizer, SpeechRecognitionEventArgs *eventArgs) {
//        NSLog(@"Received intermediate result event. SessionId: %@, intermediate result:%@.", eventArgs.sessionId, eventArgs.result.text);
    }];
    [speechRecognizer addCanceledEventListener: ^ (SpeechRecognizer *recognizer, SpeechRecognitionCanceledEventArgs *eventArgs) {
//        NSLog(@"Received canceled event. SessionId: %@, reason:%lu errorDetails:%@.", eventArgs.sessionId, (unsigned long)eventArgs.reason, eventArgs.errorDetails);
//        CancellationDetails *details = [CancellationDetails fromResult:eventArgs.result];
//        NSLog(@"Received cancellation details. reason:%lu errorDetails:%@.", (unsigned long)details.reason, details.errorDetails);
    }];
    end = false;
    [speechRecognizer startContinuousRecognition];
    while (end == false)
        [NSThread sleepForTimeInterval:1.0f];
    [speechRecognizer stopContinuousRecognition];
    
    NSLog(@"Finishing recognition");
}
    
+ (void) runTranslation{
    __block bool end = false;
    TranslationRecognizer *translationRecognizer;
    SpeechTranslationConfig *translationConfig = [SpeechTranslationConfig fromSubscription:speechKey andRegion:serviceRegion];
    [translationConfig setSpeechRecognitionLanguage:@"en-us"];
    [translationConfig addTargetLanguage:@"de"];
    [translationConfig addTargetLanguage:@"zh-Hans"];
    [translationConfig setVoiceName:@"de-DE-Hedda"];
    translationRecognizer = [TranslationRecognizer fromConfig:translationConfig];

    [translationRecognizer recognizeOnceAsync: ^ (TranslationTextResult *result){
        NSLog(@"RecognizeOnceAsync: Translation result: recognized: %@. Status %ld.", result.text, (long)result.reason);
        for (id lang in [result.translations allKeys]) {
            NSLog(@"RecognizeOnceAsync: Translation result: translated into %@: %@", lang, [result.translations objectForKey:lang]);
        }
        end = true;
    }];
    while (end == false)
        [NSThread sleepForTimeInterval:1.0f];
}

@end
