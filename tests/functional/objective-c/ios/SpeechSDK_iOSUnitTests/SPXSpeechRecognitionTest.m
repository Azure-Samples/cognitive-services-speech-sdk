//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import <XCTest/XCTest.h>
#import <MicrosoftCognitiveServicesSpeech/SPXSpeechApi.h>

@interface SPXSpeechRecognitionTest : XCTestCase {
    NSString *weatherFileName;
    NSString *weatherTextEnglish;
    NSMutableDictionary *result;

    NSPredicate *sessionStoppedCountPred;

    double timeoutInSeconds;
}

    @property (nonatomic, assign) NSString * speechKey;
    @property (nonatomic, assign) NSString * serviceRegion;
    @property (nonatomic, retain) SPXSpeechRecognizer* speechRecognizer;
    @property (nonatomic, retain) SPXConnection* connection;
@end

@implementation SPXSpeechRecognitionTest

- (void)setUp {
    [super setUp];
    timeoutInSeconds = 20.;
    weatherFileName = @"whatstheweatherlike";
    weatherTextEnglish = @"What's the weather like?";

    self.speechKey = [[[NSProcessInfo processInfo] environment] objectForKey:@"subscriptionKey"];
    self.serviceRegion = [[[NSProcessInfo processInfo] environment] objectForKey:@"serviceRegion"];

    result = [NSMutableDictionary new];
    [result setObject:@0 forKey:@"connectedCount"];
    [result setObject:@0 forKey:@"disconnectedCount"];
    [result setObject:@0 forKey:@"finalResultCount"];
    [result setObject:@0 forKey:@"sessionStoppedCount"];
    [result setObject:@"" forKey:@"finalText"];

    NSBundle *bundle = [NSBundle bundleForClass:[self class]];
    NSString *weatherFile = [bundle pathForResource: weatherFileName ofType:@"wav"];
    SPXAudioConfiguration* weatherAudioSource = [[SPXAudioConfiguration alloc] initWithWavFileInput:weatherFile];
    SPXSpeechConfiguration *speechConfig = [[SPXSpeechConfiguration alloc] initWithSubscription:self.speechKey region:self.serviceRegion];
    self.speechRecognizer = [[SPXSpeechRecognizer alloc] initWithSpeechConfiguration:speechConfig audioConfiguration:weatherAudioSource];
    self.connection = [[SPXConnection alloc] initFromRecognizer:self.speechRecognizer];

    __unsafe_unretained typeof(self) weakSelf = self;

    [self.connection addConnectedEventHandler: ^ (SPXConnection *connection, SPXConnectionEventArgs *eventArgs) {
        NSLog(@"Received connected event. SessionId: %@", eventArgs.sessionId);
        [weakSelf->result setObject:[NSNumber numberWithLong:[[weakSelf->result objectForKey:@"connectedCount"] integerValue] + 1] forKey:@"connectedCount"];
    }];
    
    [self.connection addDisconnectedEventHandler: ^ (SPXConnection *connection, SPXConnectionEventArgs *eventArgs) {
        NSLog(@"Received disconnected event. SessionId: %@", eventArgs.sessionId);
        [weakSelf->result setObject:[NSNumber numberWithLong:[[weakSelf->result objectForKey:@"disconnectedCount"] integerValue] + 1] forKey:@"disconnected"];
    }];

    [self.speechRecognizer addRecognizedEventHandler: ^ (SPXSpeechRecognizer *recognizer, SPXSpeechRecognitionEventArgs *eventArgs) {
        NSLog(@"Received final result event. SessionId: %@, recognition result:%@. Status %ld. offset %llu duration %llu resultid:%@", eventArgs.sessionId, eventArgs.result.text, (long)eventArgs.result.reason, eventArgs.result.offset, eventArgs.result.duration, eventArgs.result.resultId);
        NSLog(@"Received JSON: %@", [eventArgs.result.properties getPropertyById:SPXSpeechServiceResponseJsonResult]);
        [weakSelf->result setObject:eventArgs.result.text forKey: @"finalText"];
        [weakSelf->result setObject:[NSNumber numberWithLong:[[weakSelf->result objectForKey:@"finalResultCount"] integerValue] + 1] forKey:@"finalResultCount"];
    }];

    [self.speechRecognizer addSessionStartedEventHandler: ^ (SPXRecognizer *recognizer, SPXSessionEventArgs *eventArgs) {
        NSLog(@"Received session started event. SessionId: %@", eventArgs.sessionId);
    }];

    [self.speechRecognizer addSessionStoppedEventHandler: ^ (SPXRecognizer *recognizer, SPXSessionEventArgs *eventArgs) {
        [weakSelf->result setObject:[NSNumber numberWithLong:[[weakSelf->result objectForKey:@"sessionStoppedCount"] integerValue] + 1] forKey:@"sessionStoppedCount"];
    }];

    sessionStoppedCountPred = [NSPredicate predicateWithBlock: ^BOOL (id  _Nullable evaluatedObject, NSDictionary<NSString *,id> * _Nullable bindings){
        return [[evaluatedObject valueForKey:@"sessionStoppedCount"] isEqualToNumber:@1];
    }];
}

- (void)tearDown {
    [super tearDown];
}

- (void)testContinuousRecognition {
    [self.speechRecognizer startContinuousRecognition];

    [self expectationForPredicate:sessionStoppedCountPred evaluatedWithObject:result handler:nil];
    [self waitForExpectationsWithTimeout:timeoutInSeconds handler:nil];

    XCTAssertTrue([[self->result valueForKey:@"finalText"] isEqualToString: self->weatherTextEnglish]);
    XCTAssertTrue([[self->result valueForKey:@"finalResultCount"] isEqualToNumber:@1]);
    long connectedEventCount = [[self->result valueForKey:@"connectedCount"] integerValue];
    long disconnectedEventCount = [[self->result valueForKey:@"disconnectedCount"] integerValue];
    XCTAssertTrue(connectedEventCount > 0, @"The connected event count must be greater than 0. connectedEventCount=%ld", connectedEventCount);
    XCTAssertTrue(connectedEventCount == disconnectedEventCount + 1 || connectedEventCount == disconnectedEventCount,
        @"The connected event count (%ld) does not match the disconnected event count (%ld)", connectedEventCount, disconnectedEventCount);
    [self.speechRecognizer stopContinuousRecognition];
}

- (void)testRecognizeAsync {
    [self.speechRecognizer recognizeOnceAsync: ^ (SPXSpeechRecognitionResult *srresult) {
        XCTAssertTrue([srresult.text isEqualToString:self->weatherTextEnglish], "Final Result Text does not match");
    }];

    [self expectationForPredicate:sessionStoppedCountPred evaluatedWithObject:result handler:nil];
    [self waitForExpectationsWithTimeout:timeoutInSeconds handler:nil];

    XCTAssertTrue([[self->result valueForKey:@"finalText"] isEqualToString: self->weatherTextEnglish]);
    XCTAssertTrue([[self->result valueForKey:@"finalResultCount"] isEqualToNumber:@1]);
    long connectedEventCount = [[self->result valueForKey:@"connectedCount"] integerValue];
    long disconnectedEventCount = [[self->result valueForKey:@"disconnectedCount"] integerValue];
    XCTAssertTrue(connectedEventCount > 0, @"The connected event count must be greater than 0. connectedEventCount=%ld", connectedEventCount);
    XCTAssertTrue(connectedEventCount == disconnectedEventCount + 1 || connectedEventCount == disconnectedEventCount,
        @"The connected event count (%ld) does not match the disconnected event count (%ld)", connectedEventCount, disconnectedEventCount);
}

- (void)testRecognizeOnce {
    SPXSpeechRecognitionResult *result = [self.speechRecognizer recognizeOnce];
    NSLog(@"recognition result:%@. Status %ld. offset %llu duration %llu resultid:%@",
          result.text, (long)result.reason, result.offset, result.duration, result.resultId);

    XCTAssertTrue([result.text isEqualToString:weatherTextEnglish], "Final Result Text does not match");
    XCTAssertTrue(result.reason == SPXResultReason_RecognizedSpeech);
    XCTAssertTrue(result.duration > 0);
    XCTAssertTrue(result.offset > 0);
    XCTAssertTrue([result.resultId length] > 0);
}

- (void)testRecognizerWithMultipleHandlers {
    [result setObject:@0 forKey:@"finalResultCount2"];

    __unsafe_unretained typeof(self) weakSelf = self;

    [self.speechRecognizer addRecognizedEventHandler: ^ (SPXSpeechRecognizer *recognizer, SPXSpeechRecognitionEventArgs *eventArgs) {
        [weakSelf->result setObject:[NSNumber numberWithLong:[[weakSelf->result objectForKey:@"finalResultCount2"] integerValue] + 1] forKey:@"finalResultCount2"];
    }];

    NSPredicate* finalResultCount2Pred = [NSPredicate predicateWithBlock: ^BOOL (id  _Nullable evaluatedObject, NSDictionary<NSString *,id> * _Nullable bindings){
        return [[evaluatedObject valueForKey:@"finalResultCount"] isEqualToNumber:@1];
    }];

    [self.speechRecognizer recognizeOnceAsync: ^ (SPXSpeechRecognitionResult *srresult) {
        XCTAssertTrue([srresult.text isEqualToString:self->weatherTextEnglish], "Final Result Text does not match");
    }];

    [self expectationForPredicate:sessionStoppedCountPred evaluatedWithObject:self->result handler:nil];
    [self expectationForPredicate:finalResultCount2Pred evaluatedWithObject:self->result handler:nil];
    [self waitForExpectationsWithTimeout:timeoutInSeconds handler:nil];

    XCTAssertTrue([[self->result valueForKey:@"finalText"] isEqualToString: self->weatherTextEnglish]);
    XCTAssertTrue([[self->result valueForKey:@"finalResultCount"] isEqualToNumber:@1]);
}

- (void)testContinuousRecognitionWithPreConnection {
    __block bool end = false;
    __block int finalResultCount = 0;
    __block NSString* finalText = @"";

    SPXConnection *connection = [[SPXConnection alloc] initFromRecognizer:self.speechRecognizer];
    // [connection startConnection(true)];
    [self.speechRecognizer addRecognizedEventHandler: ^ (SPXSpeechRecognizer *recognizer, SPXSpeechRecognitionEventArgs *eventArgs) {
        NSLog(@"Received final result event. SessionId: %@, recognition result:%@. Status %ld. offset %llu duration %llu resultid:%@", eventArgs.sessionId, eventArgs.result.text, (long)eventArgs.result.reason, eventArgs.result.offset, eventArgs.result.duration, eventArgs.result.resultId);
        NSLog(@"Received JSON: %@", [eventArgs.result.properties getPropertyById:SPXSpeechServiceResponseJsonResult]);
        finalText = eventArgs.result.text;
        finalResultCount++;
        end = true;
    }];
    
    [self.speechRecognizer startContinuousRecognition];
    while (end == false)
        [NSThread sleepForTimeInterval:1.0f];
    [self.speechRecognizer stopContinuousRecognition];
    
    XCTAssertEqual(finalResultCount, 1,  "no Final Result Event received");
    XCTAssertTrue([finalText isEqualToString:weatherTextEnglish], "Final Result Text does not match");
}

- (void)testRecognizeAsyncWithPreConnection {
    __block int finalResultCount = 0;
    __block NSString* finalText = @"";
    __block bool end = false;

    SPXConnection *connection = [[SPXConnection alloc] initFromRecognizer:self.speechRecognizer];
    // [connection startConnection(true)];
    
    [self.speechRecognizer addRecognizedEventHandler: ^ (SPXSpeechRecognizer *recognizer, SPXSpeechRecognitionEventArgs *eventArgs) {
        finalResultCount++;
    }];
    
    end = false;
    [self.speechRecognizer recognizeOnceAsync: ^ (SPXSpeechRecognitionResult *result){
        finalText = result.text;
        XCTAssertEqual(finalResultCount, 1,  "wrong count of final events");
        XCTAssertTrue([finalText isEqualToString:weatherTextEnglish], "Final Result Text does not match");
        end = true;
    }];
    while (end == false)
        [NSThread sleepForTimeInterval:1.0f];
}

@end
