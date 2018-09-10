//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "speech_factory.h"
#import "translation_text_result.h"

int main(int argc, char * argv[]) {
    @autoreleasepool {
        //return UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class]));
        __block bool end = false;
        
        SpeechFactory *factory = [SpeechFactory fromSubscription:@"" AndRegion:@"westus"];
        /*
        SpeechRecognizer *recognizer = [factory createSpeechRecognizerWithFileInput:@"/users/carbon/carbon/tests/input/audio/whatstheweatherlike.wav"];
        */
        // Test1: Use Recognize()
        /*
        SpeechRecognitionResult *result = [recognizer recognize];
        NSLog(@"Recognition result %@. Status %ld.", result.text, (long)result.recognitionStatus);
        */
        // Test2: Use RecognizeAsync() with completion block
        // [recognizer recognizeAsync: ^ (SpeechRecognitionResult *result){
        //    NSLog(@"Recognition result %@. Status %ld.", result.text, (long)result.recognitionStatus);
        //    end = true;
        // }];
        // while (end == false)
        //    [NSThread sleepForTimeInterval:1.0f];
        
        // Test3: Use StartContinuousRecognitionAsync()
        /*
        [recognizer addSessionEventListener: ^ (Recognizer * recognizer, SessionEventArgs *eventArgs) {
            NSLog(@"Received Session event. Type:%@(%d) SessionId: %@", eventArgs.eventType == SessionStartedEvent? @"SessionStart" : @"SessionStop", (int)eventArgs.eventType, eventArgs.sessionId);
            if (eventArgs.eventType == SessionStoppedEvent)
                end = true;
        }];
        
        [recognizer addRecognitionEventListener: ^ (Recognizer * recognizer, RecognitionEventArgs *eventArgs) {
            NSLog(@"Received Recognition event. Type:%@(%d) SessionId: %@ Offset: %d", eventArgs.eventType == SpeechStartDetectedEvent? @"SpeechStart" : @"SpeechEnd", (int)eventArgs.eventType, eventArgs.sessionId, (int)eventArgs.offset);
        }];
        [recognizer addFinalResultEventListener: ^ (SpeechRecognizer * recognizer, SpeechRecognitionResultEventArgs *eventArgs) {
            NSLog(@"Received final result event. SessionId: %@, recognition result:%@. Status %ld.", eventArgs.sessionId, eventArgs.result.text, (long)eventArgs.result.recognitionStatus);
        }];
        [recognizer addIntermediateResultEventListener: ^ (SpeechRecognizer * recognizer, SpeechRecognitionResultEventArgs *eventArgs) {
            NSLog(@"Received intermediate result event. SessionId: %@, intermediate result:%@.", eventArgs.sessionId, eventArgs.result.text);
        }];
        [recognizer startContinuousRecognition];
        while (end == false)
            [NSThread sleepForTimeInterval:1.0f];
        [recognizer stopContinuousRecognition];
        
        [recognizer close];
         */
        
        // Test: Translation

        TranslationRecognizer *recognizer = [factory createTranslationRecognizerWithFileInput:@"/users/carbon/carbon/tests/input/audio/whatstheweatherlike.wav" FromLanguage:@"en-us" ToLanguages:@[@"de", @"zh-Hans"] WithVoiceOutput:@"de-DE-Hedda"];
        //TranslationRecognizer *recognizer = [factory createTranslationRecognizerWithFileInput:@"/users/carbon/carbon/tests/input/audio/whatstheweatherlike.wav" FromLanguage:@"en-us" ToLanguages:@[@"de", @"zh-Hans"]];
        
        // Test1: Use Recognize()
        /*
        TranslationTextResult *result = [recognizer translate];
        NSLog(@"Translation result: recognized: %@. Status %ld.", result.text, (long)result.recognitionStatus);
        for (id lang in [result.translations allKeys]) {
            NSLog(@"Translation result: translated into %@: %@", lang, [result.translations objectForKey:lang]);
        }
        */
        // Test2: Use RecognizeAsync() with completion block
        /*
        [recognizer translateAsync: ^ (TranslationTextResult *result){
            NSLog(@"Translation result: recognized: %@. Status %ld.", result.text, (long)result.recognitionStatus);
            for (id lang in [result.translations allKeys]) {
                NSLog(@"Translation result: translated into %@: %@", lang, [result.translations objectForKey:lang]);
            }
           end = true;
        }];
        while (end == false)
            [NSThread sleepForTimeInterval:1.0f];
        */
        // Test3: Use StartContinuousRecognitionAsync()
        
         [recognizer addSessionEventListener: ^ (Recognizer * recognizer, SessionEventArgs *eventArgs) {
             NSLog(@"Received Session event. Type:%@(%d) SessionId: %@", eventArgs.eventType == SessionStartedEvent? @"SessionStart" : @"SessionStop", (int)eventArgs.eventType, eventArgs.sessionId);
             if (eventArgs.eventType == SessionStoppedEvent)
                 end = true;
            }];
         
         [recognizer addRecognitionEventListener: ^ (Recognizer * recognizer, RecognitionEventArgs *eventArgs) {
             NSLog(@"Received Recognition event. Type:%@(%d) SessionId: %@ Offset: %d", eventArgs.eventType == SpeechStartDetectedEvent? @"SpeechStart" : @"SpeechEnd", (int)eventArgs.eventType, eventArgs.sessionId, (int)eventArgs.offset);
            }];
         [recognizer addFinalResultEventListener: ^ (TranslationRecognizer * recognizer, TranslationTextResultEventArgs *eventArgs) {
             NSLog(@"Received final result event. SessionId: %@, recognition result:%@. Status %ld.", eventArgs.sessionId, eventArgs.result.text, (long)eventArgs.result.recognitionStatus);
             for (id lang in [eventArgs.result.translations allKeys]) {
                 NSLog(@"Translation result: translated into %@: %@", lang, [eventArgs.result.translations objectForKey:lang]);
             }
            }];
         [recognizer addIntermediateResultEventListener: ^ (TranslationRecognizer * recognizer, TranslationTextResultEventArgs *eventArgs) {
             NSLog(@"Received intermediate result event. SessionId: %@, intermediate result:%@.", eventArgs.sessionId, eventArgs.result.text);
             for (id lang in [eventArgs.result.translations allKeys]) {
                 NSLog(@"Translation result: translated into %@: %@", lang, [eventArgs.result.translations objectForKey:lang]);
             }
            }];
        [recognizer addSynthesisResultEventListener: ^ (TranslationRecognizer * recognizer, TranslationSynthesisResultEventArgs *eventArgs) {
            NSString *statusStr;
            if (eventArgs.result.status == SynthesisSuccess)
                statusStr = @"Success";
            else if (eventArgs.result.status == SynthesisEnd)
                statusStr = @"End";
            else if (eventArgs.result.status == SynthesisError)
                statusStr = @"Error";
            
            NSLog(@"Received synthesis result event. SessionId: %@, status: %@ audio length:%d.", eventArgs.sessionId, statusStr, [eventArgs.result.audio length]);
        }];
         [recognizer startContinuousTranslation];
         while (end == false)
             [NSThread sleepForTimeInterval:1.0f];
         [recognizer stopContinuousTranslation];
         
         [recognizer close];
         
        return 0;
    }
}
