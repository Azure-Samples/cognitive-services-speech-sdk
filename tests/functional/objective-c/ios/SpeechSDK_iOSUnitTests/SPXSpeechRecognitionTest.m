//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import <XCTest/XCTest.h>
#import <MicrosoftCognitiveServicesSpeech/SPXSpeechApi.h>

@interface SPXSpeechRecognitionTest : XCTestCase {
    NSString *weatherFileName;
    NSString *weatherTextEnglish;
}
    @property (nonatomic, assign) NSString * speechKey;
    @property (nonatomic, assign) NSString * serviceRegion;
    @property (nonatomic, retain) SPXSpeechRecognizer* speechRecognizer;
@end

@implementation SPXSpeechRecognitionTest

- (void)setUp {
    [super setUp];
    weatherFileName = @"whatstheweatherlike";
    weatherTextEnglish = @"What's the weather like?";

    self.speechKey = [[[NSProcessInfo processInfo] environment] objectForKey:@"subscriptionKey"];
    self.serviceRegion = [[[NSProcessInfo processInfo] environment] objectForKey:@"serviceRegion"];
    
    NSBundle *bundle = [NSBundle bundleForClass:[self class]];
    NSString *weatherFile = [bundle pathForResource: weatherFileName ofType:@"wav"];
    SPXAudioConfiguration* weatherAudioSource = [[SPXAudioConfiguration alloc] initWithWavFileInput:weatherFile];
    SPXSpeechConfiguration *speechConfig = [[SPXSpeechConfiguration alloc] initWithSubscription:self.speechKey region:self.serviceRegion];
    self.speechRecognizer = [[SPXSpeechRecognizer alloc] initWithSpeechConfiguration:speechConfig audioConfiguration:weatherAudioSource];
}

- (void)tearDown {
    [super tearDown];
}

- (void)testContinuousRecognition {
    __block bool end = false;
    __block int finalResultCount = 0;
    __block NSString* finalText = @"";
    
    [self.speechRecognizer addRecognizedEventHandler: ^ (SPXSpeechRecognizer *recognizer, SPXSpeechRecognitionEventArgs *eventArgs) {
        NSLog(@"Received final result event. SessionId: %@, recognition result:%@. Status %ld. offset %llu duration %llu resultid:%@", eventArgs.sessionId, eventArgs.result.text, (long)eventArgs.result.reason, eventArgs.result.offset, eventArgs.result.duration, eventArgs.result.resultId);
        NSLog(@"Received JSON: %@", [eventArgs.result.properties getPropertyById:SPXSpeechServiceResponseJsonResult]);
        finalText = eventArgs.result.text;
        finalResultCount++;
    }];

    [self.speechRecognizer addSessionStoppedEventHandler: ^ (SPXRecognizer *recognizer, SPXSessionEventArgs *eventArgs) {
        end = true;
    }];
    
    [self.speechRecognizer startContinuousRecognition];
    while (end == false)
        [NSThread sleepForTimeInterval:1.0f];
    [self.speechRecognizer stopContinuousRecognition];

    XCTAssertTrue([finalText isEqualToString:weatherTextEnglish], "Final Result Text does not match");
    XCTAssertEqual(finalResultCount, 1,  "no Final Result Event received");
}

- (void)testRecognizeAsync {
    __block int finalResultCount = 0;
    __block NSString* finalText = @"";
    __block bool end = false;
    
    [self.speechRecognizer addRecognizedEventHandler: ^ (SPXSpeechRecognizer *recognizer, SPXSpeechRecognitionEventArgs *eventArgs) {
        finalResultCount++;
    }];
    
    [self.speechRecognizer addSessionStoppedEventHandler: ^ (SPXRecognizer *recognizer, SPXSessionEventArgs *eventArgs) {
        end = true;
    }];

    end = false;
    [self.speechRecognizer recognizeOnceAsync: ^ (SPXSpeechRecognitionResult *result) {
        finalText = result.text;
    }];

    while (end == false)
        [NSThread sleepForTimeInterval:1.0f];

    XCTAssertTrue([finalText isEqualToString:weatherTextEnglish], "Final Result Text does not match");
    XCTAssertEqual(finalResultCount, 1,  "wrong count of final events");
}

- (void)testRecognizeOnce {
    __block int finalResultCount = 0;
    __block NSString* finalText = @"";
    
    [self.speechRecognizer addRecognizedEventHandler: ^ (SPXSpeechRecognizer *recognizer, SPXSpeechRecognitionEventArgs *eventArgs) {
        finalResultCount++;
    }];

    SPXSpeechRecognitionResult *result = [self.speechRecognizer recognizeOnce];
    finalText = result.text;

    XCTAssertTrue([finalText isEqualToString:weatherTextEnglish], "Final Result Text does not match");
    XCTAssertEqual(finalResultCount, 1,  "wrong count of final events");
}

- (void)testRecognizerWithMultipleHandlers {
    __block int finalResultCount1 = 0;
    __block int finalResultCount2 = 0;
    __block NSString* finalText = @"";
    __block bool end = false;

    [self.speechRecognizer addRecognizedEventHandler: ^ (SPXSpeechRecognizer *recognizer, SPXSpeechRecognitionEventArgs *eventArgs) {
            finalResultCount1++;
    }];

    [self.speechRecognizer addRecognizedEventHandler: ^ (SPXSpeechRecognizer *recognizer, SPXSpeechRecognitionEventArgs *eventArgs) {
            finalResultCount2++;
    }];

    [self.speechRecognizer addSessionStoppedEventHandler: ^ (SPXRecognizer *recognizer, SPXSessionEventArgs *eventArgs) {
        end = true;
    }];

    end = false;
    [self.speechRecognizer recognizeOnceAsync: ^ (SPXSpeechRecognitionResult *result) {
        finalText = result.text;
    }];
    
    while (end == false)
        [NSThread sleepForTimeInterval:1.0f];
    
    XCTAssertEqual(finalResultCount1, finalResultCount2,  "Final Result Counts do not match.");
    XCTAssertEqual(finalResultCount1, 1,  "Final Result Count wrong");
    XCTAssertEqual(finalResultCount2, 1,  "Final Result Count wrong");
    XCTAssertTrue([finalText isEqualToString:weatherTextEnglish], "Final Result Text does not match");
}

@end
