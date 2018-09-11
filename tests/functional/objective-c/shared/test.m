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
        
        NSString *speechKey = @"";
        NSString *intentKey = @"";
        
        __block bool end = false;
        
        SpeechFactory *factory = [SpeechFactory fromSubscription:speechKey AndRegion:@"westus"];
        SpeechRecognizer *speechRecognizer = [factory createSpeechRecognizerWithFileInput:weatherFile];
        
        // Test1: Use Recognize()
        /*
         SpeechRecognitionResult *result = [recognizer recognize];
         NSLog(@"Recognition result %@. Status %ld.", result.text, (long)result.reason);
         
         */
        // Test2: Use RecognizeAsync() with completion block
        // [recognizer recognizeAsync: ^ (SpeechRecognitionResult *result){
        //    NSLog(@"Recognition result %@. Status %ld.", result.text, (long)result.reason);
        //    end = true;
        // }];
        // while (end == false)
        //    [NSThread sleepForTimeInterval:1.0f];
        
        // Test3: Use StartContinuousRecognitionAsync()
        [speechRecognizer addSessionEventListener: ^ (Recognizer * recognizer, SessionEventArgs *eventArgs) {
            NSLog(@"Received Session event. Type:%@(%d) SessionId: %@", eventArgs.eventType == SessionStartedEvent? @"SessionStart" : @"SessionStop", (int)eventArgs.eventType, eventArgs.sessionId);
            if (eventArgs.eventType == SessionStoppedEvent)
                end = true;
        }];
        
        [speechRecognizer addRecognitionEventListener: ^ (Recognizer * recognizer, RecognitionEventArgs *eventArgs) {
            NSLog(@"Received Recognition event. Type:%@(%d) SessionId: %@ Offset: %d", eventArgs.eventType == SpeechStartDetectedEvent? @"SpeechStart" : @"SpeechEnd", (int)eventArgs.eventType, eventArgs.sessionId, (int)eventArgs.offset);
        }];
        [speechRecognizer addFinalResultEventListener: ^ (SpeechRecognizer * recognizer, SpeechRecognitionEventArgs *eventArgs) {
            NSLog(@"Received final result event. SessionId: %@, recognition result:%@. Status %ld.", eventArgs.sessionId, eventArgs.result.text, (long)eventArgs.result.reason);
        }];
        [speechRecognizer addIntermediateResultEventListener: ^ (SpeechRecognizer * recognizer, SpeechRecognitionEventArgs *eventArgs) {
            NSLog(@"Received intermediate result event. SessionId: %@, intermediate result:%@.", eventArgs.sessionId, eventArgs.result.text);
        }];
        [speechRecognizer startContinuousRecognition];
        while (end == false)
            [NSThread sleepForTimeInterval:1.0f];
        [speechRecognizer stopContinuousRecognition];
        
        [speechRecognizer close];
        
        // Test: Translation
        
        TranslationRecognizer *translationRecognizer = [factory createTranslationRecognizerWithFileInput:weatherFile FromLanguage:@"en-us" ToLanguages:@[@"de", @"zh-Hans"] WithVoiceOutput:@"de-DE-Hedda"];
        
        //TranslationRecognizer *recognizer = [factory createTranslationRecognizerWithFileInput:@"/users/carbon/carbon/tests/input/audio/whatstheweatherlike.wav" FromLanguage:@"en-us" ToLanguages:@[@"de", @"zh-Hans"]];
        
        // Test1: Use Recognize()
        /*
         TranslationTextResult *result = [recognizer translate];
         NSLog(@"Translation result: recognized: %@. Status %ld.", result.text, (long)result.reason);
         for (id lang in [result.translations allKeys]) {
         NSLog(@"Translation result: translated into %@: %@", lang, [result.translations objectForKey:lang]);
         }
         */
        // Test2: Use RecognizeAsync() with completion block
        /*
         [recognizer translateAsync: ^ (TranslationTextResult *result){
         NSLog(@"Translation result: recognized: %@. Status %ld.", result.text, (long)result.reason);
         for (id lang in [result.translations allKeys]) {
         NSLog(@"Translation result: translated into %@: %@", lang, [result.translations objectForKey:lang]);
         }
         end = true;
         }];
         while (end == false)
         [NSThread sleepForTimeInterval:1.0f];
         */
        // Test3: Use StartContinuousRecognitionAsync()
        [translationRecognizer addSessionEventListener: ^ (Recognizer * recognizer, SessionEventArgs *eventArgs) {
            NSLog(@"Received Session event. Type:%@(%d) SessionId: %@", eventArgs.eventType == SessionStartedEvent? @"SessionStart" : @"SessionStop", (int)eventArgs.eventType, eventArgs.sessionId);
            if (eventArgs.eventType == SessionStoppedEvent)
                end = true;
        }];
        
        [translationRecognizer addRecognitionEventListener: ^ (Recognizer * recognizer, RecognitionEventArgs *eventArgs) {
            NSLog(@"Received Recognition event. Type:%@(%d) SessionId: %@ Offset: %d", eventArgs.eventType == SpeechStartDetectedEvent? @"SpeechStart" : @"SpeechEnd", (int)eventArgs.eventType, eventArgs.sessionId, (int)eventArgs.offset);
        }];
        [translationRecognizer addFinalResultEventListener: ^ (TranslationRecognizer * recognizer, TranslationTextEventArgs *eventArgs) {
            NSLog(@"Received final result event. SessionId: %@, recognition result:%@. Status %ld.", eventArgs.sessionId, eventArgs.result.text, (long)eventArgs.result.reason);
            for (id lang in [eventArgs.result.translations allKeys]) {
                NSLog(@"Translation result: translated into %@: %@", lang, [eventArgs.result.translations objectForKey:lang]);
            }
        }];
        [translationRecognizer addIntermediateResultEventListener: ^ (TranslationRecognizer * recognizer, TranslationTextEventArgs *eventArgs) {
            NSLog(@"Received intermediate result event. SessionId: %@, intermediate result:%@.", eventArgs.sessionId, eventArgs.result.text);
            for (id lang in [eventArgs.result.translations allKeys]) {
                NSLog(@"Translation result: translated into %@: %@", lang, [eventArgs.result.translations objectForKey:lang]);
            }
        }];
        [translationRecognizer addSynthesisResultEventListener: ^ (TranslationRecognizer * recognizer, TranslationSynthesisEventArgs *eventArgs) {
            NSString *statusStr;
            if (eventArgs.result.status == SynthesisSuccess)
                statusStr = @"Success";
            else if (eventArgs.result.status == SynthesisEnd)
                statusStr = @"End";
            else if (eventArgs.result.status == SynthesisError)
                statusStr = @"Error";
            
            NSLog(@"Received synthesis result event. SessionId: %@, status: %@ audio length:%lu.", eventArgs.sessionId, statusStr, (unsigned long)[eventArgs.result.audio length]);
        }];
        end = false;
        [translationRecognizer startContinuousTranslation];
        while (end == false)
            [NSThread sleepForTimeInterval:1.0f];
        [translationRecognizer stopContinuousTranslation];
        
        [translationRecognizer close];
        
        
        // Test: Intent
        SpeechFactory *intentFactory = [SpeechFactory fromSubscription:intentKey AndRegion:@"westus"];
        
        IntentRecognizer *intentRecognizer = [intentFactory createIntentRecognizerWithFileInput:lampFile];
        
        LanguageUnderstandingModel *model = [LanguageUnderstandingModel FromAppId:@"b687b851-56c5-4d31-816f-35a741a3f0be"];
        [intentRecognizer AddIntentUsingId:@"MyIntent1" FromModel:model WithIntentName:@"HomeAutomation.TurnOn"];
        
        // Test1: Use Recognize()
        /*
         IntentRecognitionResult *result = [recognizer recognize];
         NSLog(@"Intent result: recognized: %@. Status %ld. IntentId %@.", result.text, (long)result.reason, result.intentId);
         */
        // Test2: Use RecognizeAsync() with completion block
        /*
         [recognizer recognizeAsync: ^ (IntentRecognitionResult *result){
         NSLog(@"Intent recognition result: recognized: %@. Status %ld. IntentId %@", result.text, (long)result.reason, result.intentId);
         end = true;
         }];
         while (end == false)
         [NSThread sleepForTimeInterval:1.0f];
         */
        // Test3: Use StartContinuousRecognitionAsync()
        
        [intentRecognizer addSessionEventListener: ^ (Recognizer * recognizer, SessionEventArgs *eventArgs) {
            NSLog(@"Received Session event. Type:%@(%d) SessionId: %@", eventArgs.eventType == SessionStartedEvent? @"SessionStart" : @"SessionStop", (int)eventArgs.eventType, eventArgs.sessionId);
            if (eventArgs.eventType == SessionStoppedEvent)
                end = true;
        }];
        
        [intentRecognizer addRecognitionEventListener: ^ (Recognizer * recognizer, RecognitionEventArgs *eventArgs) {
            NSLog(@"Received Recognition event. Type:%@(%d) SessionId: %@ Offset: %d", eventArgs.eventType == SpeechStartDetectedEvent? @"SpeechStart" : @"SpeechEnd", (int)eventArgs.eventType, eventArgs.sessionId, (int)eventArgs.offset);
        }];
        [intentRecognizer addFinalResultEventListener: ^ (IntentRecognizer * recognizer, IntentRecognitionEventArgs *eventArgs) {
            NSLog(@"Received final result event. SessionId: %@, recognition result:%@. Status %ld. IntentId %@.", eventArgs.sessionId, eventArgs.result.text, (long)eventArgs.result.reason, eventArgs.result.intentId);
        }];
        [intentRecognizer addIntermediateResultEventListener: ^ (IntentRecognizer * recognizer, IntentRecognitionEventArgs *eventArgs) {
            NSLog(@"Received intermediate result event. SessionId: %@, intermediate result:%@. IntentId %@", eventArgs.sessionId, eventArgs.result.text, eventArgs.result.intentId);
        }];
        end = false;
        [intentRecognizer startContinuousRecognition];
        while (end == false)
            [NSThread sleepForTimeInterval:1.0f];
        [intentRecognizer stopContinuousRecognition];
        
        [intentRecognizer close];
        
    return;
}

@end
