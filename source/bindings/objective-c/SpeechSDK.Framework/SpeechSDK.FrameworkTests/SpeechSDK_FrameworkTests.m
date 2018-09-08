//
//  SpeechSDK_FrameworkTests.m
//  SpeechSDK.FrameworkTests
//
//  Created by carbon on 05.09.18.
//  Copyright © 2018 carbon. All rights reserved.
//

#import <XCTest/XCTest.h>
#import "speech_factory.h"
#import "speech_recognizer.h"

@interface SpeechSDK_FrameworkTests : XCTestCase

@end

@implementation SpeechSDK_FrameworkTests

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (void)testExample {
    // This is an example of a functional test case.
    // Use XCTAssert and related functions to verify your tests produce the correct results.
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
    
    [recognizer addFinalResultEventListener: ^ (SpeechRecognizer * recognizer, SpeechRecognitionResultEventArgs *eventArgs) {
        NSLog(@"Received final result event. SessionId: %@, recognition result:%@. Status %ld.", eventArgs.sessionId, eventArgs.result.text, (long)eventArgs.result.recognitionStatus);
        end = true;
    }];
    
    [recognizer addIntermediateResultEventListener: ^ (SpeechRecognizer * recognizer, SpeechRecognitionResultEventArgs *eventArgs) {
        NSLog(@"Received intermediate result event. SessionId: %@, intermediate result:%@.", eventArgs.sessionId, eventArgs.result.text);
    }];
    
    
    [recognizer startContinuousRecognition];
    while (end == false)
        [NSThread sleepForTimeInterval:1.0f];
    [recognizer stopContinuousRecognition];
    
    [recognizer close];
}

- (void)testPerformanceExample {
    // This is an example of a performance test case.
    [self measureBlock:^{
        // Put the code you want to measure the time of here.
    }];
}

@end
