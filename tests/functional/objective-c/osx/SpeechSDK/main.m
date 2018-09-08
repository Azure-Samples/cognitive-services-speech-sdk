//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "speech_factory.h"

int main(int argc, char * argv[]) {
    @autoreleasepool {
        //return UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class]));
        __block bool end = false;
        SpeechFactory *factory = [SpeechFactory fromSubscription:@"" AndRegion:@"westus"];
        
        SpeechRecognizer *recognizer = [factory createSpeechRecognizerWithFileInput:@"/users/carbon/carbon/tests/input/audio/whatstheweatherlike.wav"];
        
        // Test1: Use Recognize()
        //SpeechRecognitionResult *result = [recognizer recognize];
        //NSLog(@"Recognition result %@. Status %ld.", result.text, (long)result.recognitionStatus);
        
        // Test2: Use RecognizeAsync() with completion block
        // [recognizer recognizeAsync: ^ (SpeechRecognitionResult *result){
        //    NSLog(@"Recognition result %@. Status %ld.", result.text, (long)result.recognitionStatus);
        //    end = true;
        // }];
        // while (end == false)
        //    [NSThread sleepForTimeInterval:1.0f];
        
        // Test3: Use StartContinuousRecognitionAsync()
        [recognizer addSessionEventListener: ^ (Recognizer * recognizer, SessionEventArgs *eventArgs) {
            NSLog(@"Received Session event. Type:%@(%d) SessionId: %@", eventArgs.eventType == SessionStartedEvent? @"SessionStart" : @"SessionStop", (int)eventArgs.eventType, eventArgs.sessionId);
            if (eventArgs.eventType == SessionStoppedEvent)
                end = true;
        }];
        
        [recognizer addRecognitionEventListener: ^ (Recognizer * recognizer, RecognitionEventArgs *eventArgs) {
            NSLog(@"Received Recognition event. Type:%@(%d) SessionId: %@ Offset: %d", eventArgs.eventType == SpeechStartDetectedEvent? @"SpeechStart" : @"SpeechEnd", (int)eventArgs.eventType, eventArgs.sessionId, (int)eventArgs.offset);
        }];
        [recognizer addFinalResultEventListener: ^ (SpeechRecognizer * reconizer, SpeechRecognitionResultEventArgs *eventArgs) {
            NSLog(@"Received final result event. SessionId: %@, recognition result:%@. Status %ld.", eventArgs.sessionId, eventArgs.result.text, (long)eventArgs.result.recognitionStatus);
        }];
        [recognizer addIntermediateResultEventListener: ^ (SpeechRecognizer * reconizer, SpeechRecognitionResultEventArgs *eventArgs) {
            NSLog(@"Received interemdiate result event. SessionId: %@, intermediate result:%@.", eventArgs.sessionId, eventArgs.result.text);
        }];
        [recognizer startContinuousRecognition];
        while (end == false)
            [NSThread sleepForTimeInterval:1.0f];
        [recognizer stopContinuousRecognition];
        
        [recognizer close];
        return 0;
    }
}
