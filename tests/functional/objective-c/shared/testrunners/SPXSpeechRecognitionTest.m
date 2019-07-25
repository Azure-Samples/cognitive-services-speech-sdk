//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import <XCTest/XCTest.h>
#import <AVFoundation/AVFoundation.h>
#import <MicrosoftCognitiveServicesSpeech/SPXSpeechApi.h>
#import "SPXSpeechRecognitionEndtoEndTest.h"


@implementation SPXSpeechRecognitionEndtoEndTest

- (void)setUp {
    [super setUp];
    timeoutInSeconds = 20.;
    margaritaFileName = @"Margarita-44100";
    margaritaTextEnglish = @"And.  We all know and Love, a margarita and if you're anything like me, you probably had your fair share and if you have. You've probably noticed that margaritas? Can vary from incredible too. So I'm going to show you? How to make one of those incredibly fresh and Delicious Margaritas. There are 3 key steps you need to follow 1st.";

    weatherFileName = @"whatstheweatherlike";
    weatherTextEnglish = @"What's the weather like?";

    // set weather as the default test file, overrwite in test cases as needed
    targetFileName = weatherFileName;
    targetTranscription = weatherTextEnglish;
}

- (void)speechInit {
    self.speechKey = [[[NSProcessInfo processInfo] environment] objectForKey:@"subscriptionKey"];
    self.serviceRegion = [[[NSProcessInfo processInfo] environment] objectForKey:@"serviceRegion"];

    result = [NSMutableDictionary new];
    [result setObject:@0 forKey:@"connectedCount"];
    [result setObject:@0 forKey:@"disconnectedCount"];
    [result setObject:@0 forKey:@"finalResultCount"];
    [result setObject:@0 forKey:@"sessionStoppedCount"];
    [result setObject:@"" forKey:@"finalText"];

    [self setAudioSource];

    SPXSpeechConfiguration *speechConfig = [[SPXSpeechConfiguration alloc] initWithSubscription:self.speechKey region:self.serviceRegion];
    self.speechRecognizer = [[SPXSpeechRecognizer alloc] initWithSpeechConfiguration:speechConfig audioConfiguration:self.audioConfig];
    self.connection = [[SPXConnection alloc] initFromRecognizer:self.speechRecognizer];

    __unsafe_unretained typeof(self) weakSelf = self;

    [self.connection addConnectedEventHandler: ^ (SPXConnection *connection, SPXConnectionEventArgs *eventArgs) {
        NSLog(@"Received connected event. SessionId: %@", eventArgs.sessionId);
        [weakSelf->result setObject:[NSNumber numberWithLong:[[weakSelf->result objectForKey:@"connectedCount"] integerValue] + 1] forKey:@"connectedCount"];
    }];

    [self.connection addDisconnectedEventHandler: ^ (SPXConnection *connection, SPXConnectionEventArgs *eventArgs) {
        NSLog(@"Received disconnected event. SessionId: %@", eventArgs.sessionId);
        [weakSelf->result setObject:[NSNumber numberWithLong:[[weakSelf->result objectForKey:@"disconnectedCount"] integerValue] + 1] forKey:@"disconnectedCount"];
    }];

    [self.speechRecognizer addRecognizedEventHandler: ^ (SPXSpeechRecognizer *recognizer, SPXSpeechRecognitionEventArgs *eventArgs) {
        NSLog(@"Received final result event. SessionId: %@, recognition result:%@. Status %ld. offset %llu duration %llu resultid:%@", eventArgs.sessionId, eventArgs.result.text, (long)eventArgs.result.reason, eventArgs.result.offset, eventArgs.result.duration, eventArgs.result.resultId);
        NSLog(@"Received JSON: %@", [eventArgs.result.properties getPropertyById:SPXSpeechServiceResponseJsonResult]);
        if ([[weakSelf->result valueForKey:@"finalText"] length] == 0)
        {
            [weakSelf->result setObject:eventArgs.result.text forKey: @"finalText"];
        }
        else
        {
            NSString *concatenatedResults = [NSString stringWithFormat:@"%@ %@", [weakSelf->result valueForKey:@"finalText"], eventArgs.result.text];
            [weakSelf->result setObject:concatenatedResults forKey: @"finalText"];
        }
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

- (void)setAudioSource {
    // should be overridden in derived class
}

- (void)tearDown {
    [super tearDown];
}

- (void)_testContinuousRecognition {
    [self speechInit];
    [self.speechRecognizer startContinuousRecognition];

    [self expectationForPredicate:sessionStoppedCountPred evaluatedWithObject:result handler:nil];
    [self waitForExpectationsWithTimeout:timeoutInSeconds handler:nil];

    XCTAssertEqualObjects([self->result valueForKey:@"finalText"], self->weatherTextEnglish);
    XCTAssertEqualObjects([self->result valueForKey:@"finalResultCount"], @1);
    long connectedEventCount = [[self->result valueForKey:@"connectedCount"] integerValue];
    long disconnectedEventCount = [[self->result valueForKey:@"disconnectedCount"] integerValue];
    XCTAssertGreaterThan(connectedEventCount, 0);
    XCTAssertTrue(connectedEventCount == disconnectedEventCount + 1 || connectedEventCount == disconnectedEventCount,
                  @"The connected event count (%ld) does not match the disconnected event count (%ld)", connectedEventCount, disconnectedEventCount);
    [self.speechRecognizer stopContinuousRecognition];
}

- (void)_testRecognizeAsync {
    [self speechInit];
    __block SPXSpeechRecognitionResult * asyncResult;
    [self.speechRecognizer recognizeOnceAsync: ^ (SPXSpeechRecognitionResult *srresult) {
        asyncResult = srresult;
    }];

    [self expectationForPredicate:sessionStoppedCountPred evaluatedWithObject:result handler:nil];
    [self waitForExpectationsWithTimeout:timeoutInSeconds handler:nil];

    XCTAssertEqualObjects(asyncResult.text, self->weatherTextEnglish);

    XCTAssertEqualObjects([self->result valueForKey:@"finalText"], self->weatherTextEnglish);
    XCTAssertEqualObjects([self->result valueForKey:@"finalResultCount"], @1);
    long connectedEventCount = [[self->result valueForKey:@"connectedCount"] integerValue];
    long disconnectedEventCount = [[self->result valueForKey:@"disconnectedCount"] integerValue];
    XCTAssertGreaterThan(connectedEventCount, 0);
    XCTAssertTrue(connectedEventCount == disconnectedEventCount + 1 || connectedEventCount == disconnectedEventCount,
                  @"The connected event count (%ld) does not match the disconnected event count (%ld)", connectedEventCount, disconnectedEventCount);
}

- (void)_testRecognizeOnce {
    [self speechInit];
    SPXSpeechRecognitionResult *result = [self.speechRecognizer recognizeOnce];
    NSLog(@"recognition result:%@. Status %ld. offset %llu duration %llu resultid:%@",
          result.text, (long)result.reason, result.offset, result.duration, result.resultId);

    XCTAssertEqualObjects(result.text, weatherTextEnglish);
    XCTAssertEqual(result.reason, SPXResultReason_RecognizedSpeech);
    XCTAssertGreaterThan(result.duration, 0);
    XCTAssertGreaterThan(result.offset, 0);
    XCTAssertGreaterThan([result.resultId length], 0);
}

- (void)_testRecognizerWithMultipleHandlers {
    [self speechInit];
    [result setObject:@0 forKey:@"finalResultCount2"];

    __unsafe_unretained typeof(self) weakSelf = self;

    [self.speechRecognizer addRecognizedEventHandler: ^ (SPXSpeechRecognizer *recognizer, SPXSpeechRecognitionEventArgs *eventArgs) {
        [weakSelf->result setObject:[NSNumber numberWithLong:[[weakSelf->result objectForKey:@"finalResultCount2"] integerValue] + 1] forKey:@"finalResultCount2"];
    }];

    NSPredicate* finalResultCount2Pred = [NSPredicate predicateWithBlock: ^BOOL (id  _Nullable evaluatedObject, NSDictionary<NSString *,id> * _Nullable bindings){
        return [[evaluatedObject valueForKey:@"finalResultCount"] isEqualToNumber:@1];
    }];

    [self.speechRecognizer recognizeOnceAsync: ^ (SPXSpeechRecognitionResult *srresult) {
        XCTAssertTrue([srresult.text isEqualToString:self->targetTranscription], "Final Result Text does not match");
    }];

    [self expectationForPredicate:sessionStoppedCountPred evaluatedWithObject:self->result handler:nil];
    [self expectationForPredicate:finalResultCount2Pred evaluatedWithObject:self->result handler:nil];
    [self waitForExpectationsWithTimeout:timeoutInSeconds handler:nil];

    XCTAssertEqualObjects([self->result valueForKey:@"finalText"], self->weatherTextEnglish);
    XCTAssertEqualObjects([self->result valueForKey:@"finalResultCount"], @1);
}

- (void)_testContinuousRecognitionWithPreConnection {
    [self speechInit];
    // open connection
    [self.connection open:YES];

    NSPredicate *connectedCountPred = [NSPredicate predicateWithBlock: ^BOOL (id  _Nullable evaluatedObject, NSDictionary<NSString *,id> * _Nullable bindings){
        return [[evaluatedObject valueForKey:@"connectedCount"] isEqualToNumber:@1];
    }];

    // wait for connection
    [self expectationForPredicate:connectedCountPred evaluatedWithObject:result handler:nil];
    [self waitForExpectationsWithTimeout:timeoutInSeconds handler:nil];

    [self.speechRecognizer startContinuousRecognition];

    [self expectationForPredicate:sessionStoppedCountPred evaluatedWithObject:result handler:nil];
    [self waitForExpectationsWithTimeout:timeoutInSeconds handler:nil];

    XCTAssertEqualObjects([self->result valueForKey:@"finalText"], self->weatherTextEnglish);
    XCTAssertEqualObjects([self->result valueForKey:@"finalResultCount"], @1);
    long connectedEventCount = [[self->result valueForKey:@"connectedCount"] integerValue];
    long disconnectedEventCount = [[self->result valueForKey:@"disconnectedCount"] integerValue];
    XCTAssertGreaterThan(connectedEventCount, 0);
    XCTAssertTrue(connectedEventCount == disconnectedEventCount + 1 || connectedEventCount == disconnectedEventCount,
                  @"The connected event count (%ld) does not match the disconnected event count (%ld)", connectedEventCount, disconnectedEventCount);

    [self.speechRecognizer stopContinuousRecognition];

    // start disconnect
    [self.connection close];

    NSPredicate *disconnectedCountPred = [NSPredicate predicateWithBlock: ^BOOL (id  _Nullable evaluatedObject, NSDictionary<NSString *,id> * _Nullable bindings){
        return [[evaluatedObject valueForKey:@"disconnectedCount"] isEqualToNumber:@1];
    }];

    // wait for disconnect
    [self expectationForPredicate:disconnectedCountPred evaluatedWithObject:result handler:nil];
    [self waitForExpectationsWithTimeout:timeoutInSeconds handler:nil];
}

- (void)_testRecognizeAsyncWithPreConnection {
    [self speechInit];
    [self.connection open:NO];

    NSPredicate *connectedCountPred = [NSPredicate predicateWithBlock: ^BOOL (id  _Nullable evaluatedObject, NSDictionary<NSString *,id> * _Nullable bindings){
        return [[evaluatedObject valueForKey:@"connectedCount"] isEqualToNumber:@1];
    }];

    // wait for connection
    [self expectationForPredicate:connectedCountPred evaluatedWithObject:result handler:nil];
    [self waitForExpectationsWithTimeout:timeoutInSeconds handler:nil];

    // trigger recognition
    [self.speechRecognizer recognizeOnceAsync: ^ (SPXSpeechRecognitionResult *srresult) {
        XCTAssertTrue([srresult.text isEqualToString:self->targetTranscription], "Final Result Text does not match");
    }];

    // wait for session stopped event result
    [self expectationForPredicate:sessionStoppedCountPred evaluatedWithObject:result handler:nil];
    [self waitForExpectationsWithTimeout:timeoutInSeconds handler:nil];

    // check result
    XCTAssertEqualObjects([self->result valueForKey:@"finalText"], self->weatherTextEnglish);
    XCTAssertEqualObjects([self->result valueForKey:@"finalResultCount"], @1);
    long connectedEventCount = [[self->result valueForKey:@"connectedCount"] integerValue];
    long disconnectedEventCount = [[self->result valueForKey:@"disconnectedCount"] integerValue];
    XCTAssertGreaterThan(connectedEventCount, 0);
    XCTAssertTrue(connectedEventCount == disconnectedEventCount + 1 || connectedEventCount == disconnectedEventCount,
                  @"The connected event count (%ld) does not match the disconnected event count (%ld)", connectedEventCount, disconnectedEventCount);

    [self.connection close];

    NSPredicate *disconnectedCountPred = [NSPredicate predicateWithBlock: ^BOOL (id  _Nullable evaluatedObject, NSDictionary<NSString *,id> * _Nullable bindings){
        return [[evaluatedObject valueForKey:@"disconnectedCount"] compare:@0] == NSOrderedDescending;
    }];

    // wait for disconnect
    [self expectationForPredicate:disconnectedCountPred evaluatedWithObject:result handler:nil];
    [self waitForExpectationsWithTimeout:timeoutInSeconds handler:nil];
}

@end


@interface SPXSpeechRecognitionFromFileEndtoEndTest : SPXSpeechRecognitionEndtoEndTest {
}

- (void) setAudioSource;

@end

@implementation SPXSpeechRecognitionFromFileEndtoEndTest
- (void) setAudioSource {
    NSBundle *bundle = [NSBundle bundleForClass:[self class]];
    NSString *targetFile = [bundle pathForResource: targetFileName ofType:@"wav"];

    self.audioConfig = [[SPXAudioConfiguration alloc] initWithWavFileInput:targetFile];
}

- (void) testRecognizeAsyncWithPreConnection {
    [self _testRecognizeAsyncWithPreConnection];
}

- (void) testContinuousRecognition {
    [self _testContinuousRecognition];
}

- (void) testRecognizeAsync {
    [self _testRecognizeAsync];
}

- (void) testRecognizeOnce {
    [self _testRecognizeOnce];
}

- (void) testContinuousRecognitionWithPreConnection {
    [self _testContinuousRecognitionWithPreConnection];
}

- (void) testRecognizerWithMultipleHandlers {
    [self _testRecognizerWithMultipleHandlers];
}

- (void)testContinuousRecognitionWithError {
    [self speechInit];
    NSError * err = nil;
    BOOL success = [self.speechRecognizer startContinuousRecognition:&err];
    XCTAssertTrue(success);
    XCTAssertNil(err);

    [self expectationForPredicate:sessionStoppedCountPred evaluatedWithObject:result handler:nil];
    [self waitForExpectationsWithTimeout:timeoutInSeconds handler:nil];

    XCTAssertEqualObjects([self->result valueForKey:@"finalText"], self->weatherTextEnglish);
    XCTAssertEqualObjects([self->result valueForKey:@"finalResultCount"], @1);
    long connectedEventCount = [[self->result valueForKey:@"connectedCount"] integerValue];
    long disconnectedEventCount = [[self->result valueForKey:@"disconnectedCount"] integerValue];
    XCTAssertGreaterThan(connectedEventCount, 0);
    XCTAssertTrue(connectedEventCount == disconnectedEventCount + 1 || connectedEventCount == disconnectedEventCount,
                  @"The connected event count (%ld) does not match the disconnected event count (%ld)", connectedEventCount, disconnectedEventCount);

    err = nil;
    success = [self.speechRecognizer stopContinuousRecognition:&err];
    XCTAssertTrue(success);
    XCTAssertNil(err);

}

- (void)testRecognizeAsyncWithError {
    [self speechInit];
    __block SPXSpeechRecognitionResult * asyncResult;
    NSError * err = nil;
    BOOL success = [self.speechRecognizer recognizeOnceAsync: ^ (SPXSpeechRecognitionResult *srresult) {
        asyncResult = srresult;
    } error:&err];
    XCTAssertTrue(success);
    XCTAssertNil(err);

    [self expectationForPredicate:sessionStoppedCountPred evaluatedWithObject:result handler:nil];
    [self waitForExpectationsWithTimeout:timeoutInSeconds handler:nil];

    XCTAssertEqualObjects(asyncResult.text, self->weatherTextEnglish);

    XCTAssertEqualObjects([self->result valueForKey:@"finalText"], self->weatherTextEnglish);
    XCTAssertEqualObjects([self->result valueForKey:@"finalResultCount"], @1);
    long connectedEventCount = [[self->result valueForKey:@"connectedCount"] integerValue];
    long disconnectedEventCount = [[self->result valueForKey:@"disconnectedCount"] integerValue];
    XCTAssertGreaterThan(connectedEventCount, 0);
    XCTAssertTrue(connectedEventCount == disconnectedEventCount + 1 || connectedEventCount == disconnectedEventCount,
                  @"The connected event count (%ld) does not match the disconnected event count (%ld)", connectedEventCount, disconnectedEventCount);
}

- (void)testRecognizeOnceWithError {
    [self speechInit];
    NSError * err = nil;
    SPXSpeechRecognitionResult *result = [self.speechRecognizer recognizeOnce:&err];
    XCTAssertNotNil(result);
    XCTAssertNil(err);

    NSLog(@"recognition result:%@. Status %ld. offset %llu duration %llu resultid:%@",
          result.text, (long)result.reason, result.offset, result.duration, result.resultId);

    XCTAssertEqualObjects(result.text, weatherTextEnglish);
    XCTAssertEqual(result.reason, SPXResultReason_RecognizedSpeech);
    XCTAssertGreaterThan(result.duration, 0);
    XCTAssertGreaterThan(result.offset, 0);
    XCTAssertGreaterThan([result.resultId length], 0);

    err = nil;
    SPXCancellationDetails * details = [[SPXCancellationDetails alloc] initFromCanceledRecognitionResult:result error:&err];
    XCTAssertNotNil(details);
    XCTAssertNil(err);
}

@end


@interface SPXSpeechRecognitionTest : XCTestCase {
    NSString *targetFile;
}
@property (nonatomic, assign) NSString * speechKey;
@property (nonatomic, assign) NSString * serviceRegion;
@end


@implementation SPXSpeechRecognitionTest

- (void)setUp {
    [super setUp];
    NSString *targetFileName = @"whatstheweatherlike";
    NSBundle *bundle = [NSBundle bundleForClass:[self class]];
    targetFile = [bundle pathForResource:targetFileName ofType:@"wav"];

    self.speechKey = [[[NSProcessInfo processInfo] environment] objectForKey:@"subscriptionKey"];
    self.serviceRegion = [[[NSProcessInfo processInfo] environment] objectForKey:@"serviceRegion"];
}

- (void)testInvalidSubscriptionKey {
    SPXAudioConfiguration* audioSource = [[SPXAudioConfiguration alloc] initWithWavFileInput:targetFile];

    SPXSpeechConfiguration *speechConfig = [[SPXSpeechConfiguration alloc] initWithSubscription:@"InvalidSubscriptionKey" region:@"westus"];
    XCTAssertNotNil(speechConfig);

    // this shouldn't throw, but fail on opening the connection
    SPXSpeechRecognizer *speechRecognizer = [[SPXSpeechRecognizer alloc] initWithSpeechConfiguration:speechConfig audioConfiguration:audioSource];
    XCTAssertNotNil(speechRecognizer);

    SPXSpeechRecognitionResult *result = [speechRecognizer recognizeOnce];
    NSLog(@"recognition result:%@. Status %ld. offset %llu duration %llu resultid:%@",
          result.text, (long)result.reason, result.offset, result.duration, result.resultId);

    SPXCancellationDetails *details = [[SPXCancellationDetails alloc] initFromCanceledRecognitionResult:result];
    NSLog(@"Error Details: %@", details.errorDetails);

    NSError * err = nil;
    SPXCancellationDetails *details2 = [[SPXCancellationDetails alloc] initFromCanceledRecognitionResult:result error:&err];

    XCTAssertEqual(details2.reason, SPXCancellationReason_Error);
    XCTAssertTrue([details2.errorDetails containsString:@"WebSocket Upgrade failed with an authentication error (401)."]);
    XCTAssertNil(err);

    XCTAssertTrue([result.text isEqualToString:@""], "Final Result Text does not match");
    XCTAssertEqual(details.reason, SPXCancellationReason_Error);
    XCTAssertTrue([details.errorDetails containsString:@"WebSocket Upgrade failed with an authentication error (401)."]);
    XCTAssertEqual(result.reason, SPXResultReason_Canceled);
    XCTAssertEqual(result.duration, 0);
    XCTAssertEqual(result.offset, 0);
    XCTAssertGreaterThan([result.resultId length], 0);
}

- (void)testInvalidRegion {
    SPXAudioConfiguration* audioSource = [[SPXAudioConfiguration alloc] initWithWavFileInput:targetFile];

    NSError* error = nil;
    SPXSpeechConfiguration *speechConfig = [[SPXSpeechConfiguration alloc] initWithSubscription:self.speechKey region:@"InvalidServiceRegion" error:&error];
    XCTAssertNotNil(speechConfig);

    // this shouldn't throw, but fail on opening the connection
    SPXSpeechRecognizer *speechRecognizer = [[SPXSpeechRecognizer alloc] initWithSpeechConfiguration:speechConfig audioConfiguration:audioSource];
    XCTAssertNotNil(speechRecognizer);

    SPXSpeechRecognitionResult *result = [speechRecognizer recognizeOnce];
    NSLog(@"recognition result:%@. Status %ld. offset %llu duration %llu resultid:%@",
          result.text, (long)result.reason, result.offset, result.duration, result.resultId);

    SPXCancellationDetails *details = [[SPXCancellationDetails alloc] initFromCanceledRecognitionResult:result];
    NSLog(@"Error Details: %@", details.errorDetails);

    XCTAssertTrue([result.text isEqualToString:@""], "Final Result Text does not match");
    XCTAssertEqual(details.reason, SPXCancellationReason_Error);
    XCTAssertTrue([details.errorDetails containsString:@"Connection failed (no connection to the remote host). Internal error: 11. Error details: 0."]);
    XCTAssertEqual(result.reason, SPXResultReason_Canceled);
    XCTAssertEqual(result.duration, 0);
    XCTAssertEqual(result.offset, 0);
    XCTAssertGreaterThan([result.resultId length], 0);
}

- (void)testEmptyRegion {
    XCTAssertThrowsSpecificNamed([[SPXSpeechConfiguration alloc] initWithSubscription:@"InvalidSubscriptionKey" region:@""], NSException, @"SPXException");
}

- (void)testEmptyRegionWithError {
    NSError * err = nil;
    SPXSpeechConfiguration *speechConfig = [[SPXSpeechConfiguration alloc] initWithSubscription:@"InvalidSubscriptionKey" region:@"" error:&err];
    NSLog(@"Received Error: %@", err);
    XCTAssertNil(speechConfig);
    XCTAssert([err.description containsString:@"SPXERR_INVALID_ARG"]);
    XCTAssertEqual(err.code, 0x5);
}

- (void)testEmptyKey {
    XCTAssertThrowsSpecificNamed([[SPXSpeechConfiguration alloc] initWithSubscription:@"" region:@"westus"], NSException, @"SPXException");
}

- (void)testEmptyKeyWithError {
    NSError * err = nil;
    SPXSpeechConfiguration *speechConfig = [[SPXSpeechConfiguration alloc] initWithSubscription:@"" region:@"westus" error:&err];
    XCTAssertNil(speechConfig);
    NSLog(@"Received Error: %@", err);
    XCTAssert([err.description containsString:@"SPXERR_INVALID_ARG"]);
    XCTAssertEqual(err.code, 0x5);
}

- (void)testEmptyEndpoint {
    XCTAssertThrowsSpecificNamed([[SPXSpeechConfiguration alloc] initWithEndpoint:@""], NSException, @"SPXException");
    XCTAssertThrowsSpecificNamed([[SPXSpeechConfiguration alloc] initWithEndpoint:@"" subscription:@"InvalidSubscription"], NSException, @"SPXException");
}

- (void)testEmptyEndpointWithError {
    NSError * err = nil;
    SPXSpeechConfiguration *speechConfig = [[SPXSpeechConfiguration alloc] initWithEndpoint:@"" error:&err];
    XCTAssertNil(speechConfig);
    NSLog(@"Received Error: %@", err);
    XCTAssert([err.description containsString:@"SPXERR_INVALID_ARG"]);
    XCTAssertEqual(err.code, 0x5);
}

- (void)testProxySettings {
    NSError * err = nil;
    SPXSpeechConfiguration *speechConfig = [[SPXSpeechConfiguration alloc] initWithEndpoint:@"myendpoint" error:&err];
    XCTAssertNotNil(speechConfig);
    XCTAssertNil(err);

    [speechConfig setProxyUsingHost:@"proxy.xyz" Port:8080 UserName:@"" Password:@""];

    XCTAssertThrowsSpecificNamed([speechConfig setProxyUsingHost:@"" Port:8080 UserName:nil Password:nil], NSException, @"Invalid proxy host name");
}

- (void)testProxySettingsWithError {
    SPXSpeechConfiguration *speechConfig = [[SPXSpeechConfiguration alloc] initWithEndpoint:@"myendpoint"];
    XCTAssertNotNil(speechConfig);

    NSError * err = nil;
    BOOL success = [speechConfig setProxyUsingHost:@"" Port:8080 UserName:nil Password:nil error:&err];
    XCTAssertFalse(success);
    NSLog(@"Error message: %@", err.description);
    XCTAssert([err.description containsString:@"proxy host name is null or empty."]);
    XCTAssertEqual(err.code, 0x5);

    err = nil;
    success = [speechConfig setProxyUsingHost:@"proxy.xyz" Port:8080 UserName:@"" Password:@"" error:&err];
    XCTAssertTrue(success);
    XCTAssertNil(err);

    err = nil;
    success = [speechConfig setProxyUsingHost:@"proxy.xyz" Port:8080 UserName:@"user" Password:@"pass" error:&err];
    XCTAssertTrue(success);
    XCTAssertNil(err);

    err = nil;
    success = [speechConfig setProxyUsingHost:@"proxy.xyz" Port:8080 UserName:nil Password:nil error:&err];
    XCTAssertFalse(success);
    XCTAssert([err.description containsString:@"Error: Proxy user name or password is null."]);
    XCTAssertEqual(err.code, 0x5);
}

- (void)testInvalidWavefileName {
    SPXAudioConfiguration* invalidAudioSource = [[SPXAudioConfiguration alloc] initWithWavFileInput:@"invalidFilename"];

    SPXSpeechConfiguration *speechConfig = [[SPXSpeechConfiguration alloc] initWithSubscription:@"InvalidKey" region:@"westus"];
    XCTAssertNotNil(speechConfig);

    XCTAssertThrowsSpecificNamed([[SPXSpeechRecognizer alloc] initWithSpeechConfiguration:speechConfig audioConfiguration:invalidAudioSource], NSException, @"SPXException");
}

- (void)testFromEndpointWithoutKeyAndToken {
    NSString *weatherFileName = @"whatstheweatherlike";
    NSBundle *bundle = [NSBundle bundleForClass:[self class]];
    NSString *weatherFile = [bundle pathForResource: weatherFileName ofType:@"wav"];

    NSString *endpoint = [NSString stringWithFormat:@"wss://%@.stt.speech.microsoft.com/speech/recognition/conversation/cognitiveservices/v1", self.serviceRegion];
    SPXAudioConfiguration* weatherAudioSource = [[SPXAudioConfiguration alloc] initWithWavFileInput:weatherFile];
    SPXSpeechConfiguration* speechConfig = [[SPXSpeechConfiguration alloc] initWithEndpoint:endpoint];
    XCTAssertNotNil(speechConfig);

    SPXSpeechRecognizer *r = [[SPXSpeechRecognizer alloc] initWithSpeechConfiguration:speechConfig audioConfiguration:weatherAudioSource];
    XCTAssertNotNil(r);
    // We cannot really test whether recognizer works, since there is no test endpoint available which supports no authentication.
    XCTAssertTrue([[r.properties getPropertyById:SPXSpeechServiceConnectionKey] length] == 0);
    XCTAssertTrue([r.authorizationToken length] == 0);
}

- (void)testSetServiceProperty {
    NSString *weatherFileName = @"whatstheweatherlike";
    NSString *weatherTextEnglish = @"What's the weather like?";
    NSBundle *bundle = [NSBundle bundleForClass:[self class]];
    NSString *weatherFile = [bundle pathForResource: weatherFileName ofType:@"wav"];
    SPXAudioConfiguration* weatherAudioSource = [[SPXAudioConfiguration alloc] initWithWavFileInput:weatherFile];
    SPXSpeechConfiguration *speechConfig = [[SPXSpeechConfiguration alloc] initWithSubscription:self.speechKey region:self.serviceRegion];
    speechConfig.speechRecognitionLanguage = @"Invalid";
    [speechConfig setServicePropertyTo:@"en-us" byName:@"language" usingChannel:SPXServicePropertyChannel_UriQueryParameter];
    SPXSpeechRecognizer* r = [[SPXSpeechRecognizer alloc] initWithSpeechConfiguration:speechConfig audioConfiguration:weatherAudioSource];

    SPXSpeechRecognitionResult *result = [r recognizeOnce];
    XCTAssertEqual(result.reason, SPXResultReason_RecognizedSpeech);
    XCTAssertEqualObjects(result.text, weatherTextEnglish);
}

- (void)testLogFileExists {
    NSString *filePath = [NSTemporaryDirectory() stringByAppendingPathComponent:@"myfile3.txt"];
    NSLog(@"file path is %@", filePath);

    NSString *weatherFileName = @"whatstheweatherlike";
    NSString *weatherTextEnglish = @"What's the weather like?";
    NSBundle *bundle = [NSBundle bundleForClass:[self class]];
    NSString *weatherFile = [bundle pathForResource: weatherFileName ofType:@"wav"];
    SPXAudioConfiguration* weatherAudioSource = [[SPXAudioConfiguration alloc] initWithWavFileInput:weatherFile];
    SPXSpeechConfiguration *speechConfig = [[SPXSpeechConfiguration alloc] initWithSubscription:self.speechKey region:self.serviceRegion];
    XCTAssertNotNil(speechConfig);

    [speechConfig setPropertyTo:filePath byId:SPXSpeechLogFilename];

    speechConfig.speechRecognitionLanguage = @"en-us";
    SPXSpeechRecognizer* r = [[SPXSpeechRecognizer alloc] initWithSpeechConfiguration:speechConfig audioConfiguration:weatherAudioSource];
    XCTAssertNotNil(r);

    SPXSpeechRecognitionResult *result = [r recognizeOnce];
    XCTAssertEqual(result.reason, SPXResultReason_RecognizedSpeech);
    XCTAssertEqualObjects(result.text, weatherTextEnglish);

    NSFileManager *fileManager = [NSFileManager defaultManager];

    XCTAssertTrue([fileManager fileExistsAtPath:filePath]);
    NSDictionary *attributes = [fileManager attributesOfItemAtPath:filePath error:nil];
    unsigned long long size = [attributes fileSize];
    XCTAssertGreaterThan(size, 0, @"log file size is 0");

    NSString* content = [NSString stringWithContentsOfFile:filePath
                                                  encoding:NSUTF8StringEncoding
                                                     error:NULL];
    XCTAssertTrue([content rangeOfString:result.resultId].location != NSNotFound);
    XCTAssertTrue([content rangeOfString:@"SPX_DBG_TRACE_VERBOSE"].location != NSNotFound);
}

- (void)testPropertyGetAndSetViaPropertyId
{
    NSString *initialSilenceTimeout = @"6000";
    NSString *endSilenceTimeout = @"10000";

    SPXSpeechConfiguration *config = [[SPXSpeechConfiguration alloc] initWithSubscription:@"InvalidSubscriptionKey" region:@"someregion"];
    XCTAssertNotNil(config);

    [config setPropertyTo:initialSilenceTimeout byId:SPXSpeechServiceConnectionInitialSilenceTimeoutMs];
    [config setPropertyTo:endSilenceTimeout byId:SPXSpeechServiceConnectionEndSilenceTimeoutMs];
    XCTAssertEqualObjects(initialSilenceTimeout, [config getPropertyById:SPXSpeechServiceConnectionInitialSilenceTimeoutMs]);
    XCTAssertEqualObjects(endSilenceTimeout, [config getPropertyById:SPXSpeechServiceConnectionEndSilenceTimeoutMs]);

    NSString *threshold = @"5";
    [config setPropertyTo:threshold byId:SPXSpeechServiceResponseStablePartialResultThreshold];
    XCTAssertEqualObjects(threshold, [config getPropertyById:SPXSpeechServiceResponseStablePartialResultThreshold]);

    NSString * valStr = @"something";
    [config setPropertyTo:valStr byId:SPXSpeechServiceResponseOutputFormatOption];
    XCTAssertEqualObjects(valStr, [config getPropertyById:SPXSpeechServiceResponseOutputFormatOption]);

    NSString * profanity = @"removed";
    [config setPropertyTo:profanity byId:SPXSpeechServiceResponseProfanityOption];
    XCTAssertEqualObjects(profanity, [config getPropertyById:SPXSpeechServiceResponseProfanityOption]);

    NSString * falseStr = @"false";
    [config setPropertyTo:falseStr byId:SPXSpeechServiceConnectionEnableAudioLogging];
    XCTAssertEqualObjects(falseStr, [config getPropertyById:SPXSpeechServiceConnectionEnableAudioLogging]);

    [config setPropertyTo:falseStr byId:SPXSpeechServiceResponseRequestWordLevelTimestamps];
    XCTAssertEqualObjects(falseStr, [config getPropertyById:SPXSpeechServiceResponseRequestWordLevelTimestamps]);

    [config setPropertyTo:falseStr byId:SPXSpeechServiceResponseTranslationRequestStablePartialResult];
    XCTAssertEqualObjects(falseStr, [config getPropertyById:SPXSpeechServiceResponseTranslationRequestStablePartialResult]);

    NSString * trueText = @"TrueText";
    [config setPropertyTo:trueText byId:SPXSpeechServiceResponsePostProcessingOption];
    XCTAssertEqualObjects(trueText, [config getPropertyById:SPXSpeechServiceResponsePostProcessingOption]);

    NSString *weatherFileName = @"whatstheweatherlike";
    NSBundle *bundle = [NSBundle bundleForClass:[self class]];
    NSString *weatherFile = [bundle pathForResource: weatherFileName ofType:@"wav"];
    SPXAudioConfiguration* weatherAudioSource = [[SPXAudioConfiguration alloc] initWithWavFileInput:weatherFile];

    SPXSpeechRecognizer* recognizer = [[SPXSpeechRecognizer alloc] initWithSpeechConfiguration:config audioConfiguration:weatherAudioSource];
    XCTAssertNotNil(recognizer);

    XCTAssertEqualObjects(initialSilenceTimeout, [[recognizer properties] getPropertyById:SPXSpeechServiceConnectionInitialSilenceTimeoutMs]);
    XCTAssertEqualObjects(endSilenceTimeout, [[recognizer properties] getPropertyById:SPXSpeechServiceConnectionEndSilenceTimeoutMs]);
    XCTAssertEqualObjects(threshold, [[recognizer properties] getPropertyById:SPXSpeechServiceResponseStablePartialResultThreshold]);
    XCTAssertEqualObjects(valStr, [[recognizer properties] getPropertyById:SPXSpeechServiceResponseOutputFormatOption]);
    XCTAssertEqualObjects(profanity, [[recognizer properties] getPropertyById:SPXSpeechServiceResponseProfanityOption]);
    XCTAssertEqualObjects(falseStr, [[recognizer properties] getPropertyById:SPXSpeechServiceConnectionEnableAudioLogging]);
    XCTAssertEqualObjects(falseStr, [[recognizer properties] getPropertyById:SPXSpeechServiceResponseRequestWordLevelTimestamps]);
    XCTAssertEqualObjects(falseStr, [[recognizer properties] getPropertyById:SPXSpeechServiceResponseTranslationRequestStablePartialResult]);
    XCTAssertEqualObjects(trueText, [[recognizer properties] getPropertyById:SPXSpeechServiceResponsePostProcessingOption]);
}

-(void)testPropertyDirectSetAndGetViaPropertyId
{
    SPXSpeechConfiguration *config = [[SPXSpeechConfiguration alloc] initWithSubscription:@"InvalidSubscriptionKey" region:@"somregion"];
    XCTAssertNotNil(config);

    NSString * profanity = @"removed";
    [config setProfanityOptionTo:SPXSpeechConfigProfanityOption_ProfanityRemoved];
    [config enableAudioLogging];
    [config requestWordLevelTimestamps];
    [config enableDictation];

    NSString *weatherFileName = @"whatstheweatherlike";
    NSBundle *bundle = [NSBundle bundleForClass:[self class]];
    NSString *weatherFile = [bundle pathForResource: weatherFileName ofType:@"wav"];
    SPXAudioConfiguration* weatherAudioSource = [[SPXAudioConfiguration alloc] initWithWavFileInput:weatherFile];

    SPXSpeechRecognizer* recognizer = [[SPXSpeechRecognizer alloc] initWithSpeechConfiguration:config audioConfiguration:weatherAudioSource];
    XCTAssertNotNil(recognizer);

    XCTAssertEqualObjects(@"DICTATION", [config getPropertyById:SPXSpeechServiceConnectionRecognitionMode]);
    XCTAssertEqualObjects(@"DICTATION", [[recognizer properties] getPropertyById:SPXSpeechServiceConnectionRecognitionMode]);
    XCTAssertEqualObjects(profanity, [config getPropertyById:SPXSpeechServiceResponseProfanityOption]);
    XCTAssertEqualObjects(profanity, [[recognizer properties] getPropertyById:SPXSpeechServiceResponseProfanityOption]);
    XCTAssertEqualObjects(@"true", [config getPropertyById:SPXSpeechServiceConnectionEnableAudioLogging]);
    XCTAssertEqualObjects(@"true", [[recognizer properties] getPropertyById:SPXSpeechServiceConnectionEnableAudioLogging]);
    XCTAssertEqualObjects(@"true", [config getPropertyById:SPXSpeechServiceResponseRequestWordLevelTimestamps]);
    XCTAssertEqualObjects(@"true", [[recognizer properties] getPropertyById:SPXSpeechServiceResponseRequestWordLevelTimestamps]);
}

-(void)testNoMatchResultAndDetails
{
    NSString *audioFileName = @"silenceshort";
    NSBundle *bundle = [NSBundle bundleForClass:[self class]];
    NSString *audioFile = [bundle pathForResource: audioFileName ofType:@"wav"];
    XCTAssertNotNil(audioFile);
    SPXAudioConfiguration* audioSource = [[SPXAudioConfiguration alloc] initWithWavFileInput:audioFile];
    XCTAssertNotNil(audioSource);
    SPXSpeechConfiguration *speechConfig = [[SPXSpeechConfiguration alloc] initWithSubscription:self.speechKey region:self.serviceRegion];
    XCTAssertNotNil(speechConfig);

    SPXSpeechRecognizer* recognizer = [[SPXSpeechRecognizer alloc] initWithSpeechConfiguration:speechConfig audioConfiguration:audioSource];

    SPXSpeechRecognitionResult *result = [recognizer recognizeOnce];
    NSLog(@"recognition result:%@. Status %ld. offset %llu duration %llu resultid:%@",
          result.text, (long)result.reason, result.offset, result.duration, result.resultId);

    SPXNoMatchDetails * details = [[SPXNoMatchDetails alloc] initFromNoMatchRecognitionResult:result];
    XCTAssertNotNil(details);
    XCTAssertEqual(details.reason, SPXNoMatchReason_InitialSilenceTimeout);

    NSError * err = nil;
    SPXNoMatchDetails * details2 = [[SPXNoMatchDetails alloc] initFromNoMatchRecognitionResult:result error:&err];
    XCTAssertNotNil(details2);
    XCTAssertNil(err);
    XCTAssertEqual(details2.reason, SPXNoMatchReason_InitialSilenceTimeout);

    XCTAssertTrue([result.text isEqualToString:@""], "Final Result Text does not match");
    XCTAssertEqual(result.reason, SPXResultReason_NoMatch);
    XCTAssertEqual(result.duration, 0);
    XCTAssertEqual(result.offset, 10000000);
    XCTAssertTrue([result.resultId length] > 0);
}
@end


@interface SPXSpeechRecognitionPullStreamEndtoEndTest : SPXSpeechRecognitionEndtoEndTest {
    SPXPullAudioInputStream* stream;
}

- (void) setAudioSource;

@end

@implementation SPXSpeechRecognitionPullStreamEndtoEndTest

- (void) setAudioSource {
    NSBundle *bundle = [NSBundle bundleForClass:[self class]];
    NSString *targetFile = [bundle pathForResource:targetFileName ofType:@"wav"];
    NSURL *targetUrl = [NSURL URLWithString:targetFile];

    NSError *error = nil;
    AVAudioFile *audioFile = [[AVAudioFile alloc] initForReading:targetUrl commonFormat:AVAudioPCMFormatInt16 interleaved:NO error:&error];
    const NSInteger bytesPerFrame = audioFile.fileFormat.streamDescription->mBytesPerFrame;

    if (error)
    {
        NSLog(@"Error while opening file: %@", error);
        self->stream = nil;
        self.audioConfig = nil;
        return;
    }

    // check the format of the file
    NSAssert(1 == audioFile.fileFormat.channelCount, @"Bad channel count");
    NSAssert(16000 == audioFile.fileFormat.sampleRate, @"Unexpected sample rate");

    self->stream = [[SPXPullAudioInputStream alloc]
                                       initWithReadHandler:
                                       ^NSInteger(NSMutableData *data, NSUInteger size) {
                                           AVAudioPCMBuffer *buffer = [[AVAudioPCMBuffer alloc] initWithPCMFormat:audioFile.fileFormat frameCapacity:(AVAudioFrameCount) size / bytesPerFrame];
                                           NSError *bufferError = nil;
                                           bool success = [audioFile readIntoBuffer:buffer error:&bufferError];

                                           NSInteger nBytes = 0;
                                           if (!success) {
                                               // returns 0 to close the stream on read error.
                                               NSLog(@"Read error on stream: %@", bufferError);
                                           }
                                           else
                                           {
                                               // number of bytes in the buffer
                                               nBytes = [buffer frameLength] * bytesPerFrame;

                                               NSRange range;
                                               range.location = 0;
                                               range.length = nBytes;

                                               NSAssert(1 == buffer.stride, @"only one channel allowed");
                                               NSAssert(nil != buffer.int16ChannelData, @"assure correct format");

                                               [data replaceBytesInRange:range withBytes:buffer.int16ChannelData[0]];
                                               NSLog(@"%d bytes data returned", (int)[data length]);
                                           }
                                           // returns the number of bytes that have been read, 0 closes the stream.
                                           return nBytes;
                                       }
                                       closeHandler:
                                       ^(void) {
                                       }];

    self.audioConfig = [[SPXAudioConfiguration alloc] initWithStreamInput:self->stream];
}

- (void) testRecognizeAsyncWithPreConnection {
    [self _testRecognizeAsyncWithPreConnection];
}

- (void) testContinuousRecognition {
    [self _testContinuousRecognition];
}

- (void) testRecognizeAsync {
    [self _testRecognizeAsync];
}

- (void) testRecognizeOnce {
    [self _testRecognizeOnce];
}

- (void) testContinuousRecognitionWithPreConnection {
    [self _testContinuousRecognitionWithPreConnection];
}

- (void) testRecognizerWithMultipleHandlers {
    [self _testRecognizerWithMultipleHandlers];
}

@end


@interface SPXSpeechRecognitionPushStreamEndtoEndTest : SPXSpeechRecognitionEndtoEndTest {
    AVAudioFile *audioFile;
    SPXPushAudioInputStream* stream;
    
    NSUInteger sampleRate;
}

- (void) setAudioSource;
- (void)pushStream;

@end

@implementation SPXSpeechRecognitionPushStreamEndtoEndTest
- (void) setAudioSource {
    NSBundle *bundle = [NSBundle bundleForClass:[self class]];
    NSString *targetFile = [bundle pathForResource:targetFileName ofType:@"wav"];

    NSURL *targetUrl = [NSURL URLWithString:targetFile];
    NSError *error = nil;
    audioFile = [[AVAudioFile alloc] initForReading:targetUrl commonFormat:AVAudioPCMFormatInt16 interleaved:NO error:&error];

    if (error)
    {
        NSLog(@"Error while opening file: %@", error);
        self->stream = nil;
        self.audioConfig = nil;
        return;
    }

    // check the format of the file
    NSAssert(1 == audioFile.fileFormat.channelCount, @"Bad channel count");
    NSAssert(self->sampleRate == audioFile.fileFormat.sampleRate, @"Unexpected sample rate");

    SPXAudioStreamFormat *audioFormat = [[SPXAudioStreamFormat alloc] initUsingPCMWithSampleRate:self->sampleRate bitsPerSample:16 channels:1];
    self->stream = [[SPXPushAudioInputStream alloc] initWithAudioFormat:audioFormat];

    self.audioConfig  = [[SPXAudioConfiguration alloc] initWithStreamInput:self->stream];
}

- (void)testContinuousRecognitionWeather {
    sampleRate = 16000;
    [self speechInit];
    [self pushStream];
}

- (void)testContinuousRecognitionMargarita {
    targetFileName = margaritaFileName;
    targetTranscription = margaritaTextEnglish;
    sampleRate = 44100;
    [self speechInit];
    [self pushStream];
}

- (void)pushStream {
    [self.speechRecognizer startContinuousRecognition];
    const AVAudioFrameCount nBytesToRead = 5000;
    const NSInteger bytesPerFrame = audioFile.fileFormat.streamDescription->mBytesPerFrame;
    AVAudioPCMBuffer *buffer = [[AVAudioPCMBuffer alloc] initWithPCMFormat:audioFile.fileFormat frameCapacity:nBytesToRead / bytesPerFrame];

    NSAssert(1 == buffer.stride, @"only one channel allowed");
    NSAssert(nil != buffer.int16ChannelData, @"assure correct format");

    // push data to stream;
    while (1)
    {
        NSError *bufferError = nil;
        bool success = [audioFile readIntoBuffer:buffer error:&bufferError];
        if (!success) {
            NSLog(@"Read error on stream: %@", bufferError);
            [self->stream close];
            break;
        }
        else
        {
            NSInteger nBytesRead = [buffer frameLength] * bytesPerFrame;
            if (0 == nBytesRead)
            {
                [self->stream close];
                break;
            }

            NSLog(@"Read %d bytes from file", (int)nBytesRead);

            NSData *data = [NSData dataWithBytesNoCopy:buffer.int16ChannelData[0] length:nBytesRead freeWhenDone:NO];
            NSLog(@"%d bytes data returned", (int)[data length]);

            [self->stream write:data];
            NSLog(@"Wrote %d bytes to stream", (int)[data length]);
        }

        [NSThread sleepForTimeInterval:0.05f];
    }

    [self expectationForPredicate:sessionStoppedCountPred evaluatedWithObject:result handler:nil];
    [self waitForExpectationsWithTimeout:timeoutInSeconds handler:nil];

    NSString *finalResultText = [[[[self->result valueForKey:@"finalText"] componentsSeparatedByCharactersInSet:[[NSCharacterSet letterCharacterSet] invertedSet]] componentsJoinedByString:@""] lowercaseString];
    NSString *normalizedDesiredResultText = [[[self->targetTranscription componentsSeparatedByCharactersInSet:[[NSCharacterSet letterCharacterSet] invertedSet]] componentsJoinedByString:@""] lowercaseString];

    NSLog(@"final result: \n%@\ndesired:\n%@", finalResultText, normalizedDesiredResultText);
    XCTAssertTrue([finalResultText isEqualToString: normalizedDesiredResultText]);
    XCTAssertTrue([[self->result valueForKey:@"finalResultCount"] compare:@0] == NSOrderedDescending);
    long connectedEventCount = [[self->result valueForKey:@"connectedCount"] integerValue];
    long disconnectedEventCount = [[self->result valueForKey:@"disconnectedCount"] integerValue];
    XCTAssertTrue(connectedEventCount > 0, @"The connected event count must be greater than 0. connectedEventCount=%ld", connectedEventCount);
    XCTAssertTrue(connectedEventCount == disconnectedEventCount + 1 || connectedEventCount == disconnectedEventCount,
                  @"The connected event count (%ld) does not match the disconnected event count (%ld)", connectedEventCount, disconnectedEventCount);
    [self.speechRecognizer stopContinuousRecognition];
}

@end


@interface SPXRecognitionGrammarTests: XCTestCase {
    NSString *beachFileName;
    NSString *beachNoHelpTranscription;
    NSString *beachTranscription;
}

- (void) setAudioSource;

@property (nonatomic, assign) NSString * speechKey;
@property (nonatomic, assign) NSString * serviceRegion;
@property (nonatomic, retain) SPXAudioConfiguration* audioConfig;
@end

@implementation SPXRecognitionGrammarTests

- (void) setUp {
    beachFileName = @"wreck-a-nice-beach";
    beachNoHelpTranscription = @"Recognize speech.";
    beachTranscription = @"Wreck a nice beach.";

    self.speechKey = [[[NSProcessInfo processInfo] environment] objectForKey:@"subscriptionKey"];
    self.serviceRegion = [[[NSProcessInfo processInfo] environment] objectForKey:@"serviceRegion"];
    [self setAudioSource];
}

- (void) setAudioSource {
    NSBundle *bundle = [NSBundle bundleForClass:[self class]];
    NSString *beachFile = [bundle pathForResource: beachFileName ofType:@"wav"];

    self.audioConfig = [[SPXAudioConfiguration alloc] initWithWavFileInput:beachFile];
}

- (void)testIntentRecognitionPhraseList {
    SPXSpeechConfiguration *speechConfig = [[SPXSpeechConfiguration alloc] initWithSubscription:self.speechKey region:self.serviceRegion];
    NSAssert(nil != speechConfig, @"nil");
    SPXSpeechRecognizer* recognizer = [[SPXSpeechRecognizer alloc] initWithSpeechConfiguration:speechConfig audioConfiguration:self.audioConfig];
    NSAssert(nil != recognizer, @"nil");

    SPXPhraseListGrammar * phraseListGrammar = [[SPXPhraseListGrammar alloc] initWithRecognizer:recognizer];
    [phraseListGrammar addPhrase:@"Wreck a nice beach"];

    SPXSpeechRecognitionResult *result = [recognizer recognizeOnce];
    NSLog(@"recognition result:%@. Status %ld. offset %llu duration %llu resultid:%@",
          result.text, (long)result.reason, result.offset, result.duration, result.resultId);

    XCTAssertEqualObjects(result.text, beachTranscription);
    XCTAssertEqual(result.reason, SPXResultReason_RecognizedSpeech);
    XCTAssertTrue(result.duration > 0);
    XCTAssertTrue(result.offset > 0);
    XCTAssertTrue([result.resultId length] > 0);
}

- (void)testIntentRecognitionPhraseListCleared {
    SPXSpeechConfiguration *speechConfig = [[SPXSpeechConfiguration alloc] initWithSubscription:self.speechKey region:self.serviceRegion];
    NSAssert(nil != speechConfig, @"nil");
    SPXSpeechRecognizer* recognizer = [[SPXSpeechRecognizer alloc] initWithSpeechConfiguration:speechConfig audioConfiguration:self.audioConfig];
    NSAssert(nil != recognizer, @"nil");

    SPXPhraseListGrammar * phraseListGrammar = [[SPXPhraseListGrammar alloc] initWithRecognizer:recognizer];
    [phraseListGrammar addPhrase:@"Wreck a nice beach"];
    [phraseListGrammar clear];

    SPXSpeechRecognitionResult *result = [recognizer recognizeOnce];
    NSLog(@"recognition result:%@. Status %ld. offset %llu duration %llu resultid:%@",
          result.text, (long)result.reason, result.offset, result.duration, result.resultId);

    XCTAssertEqualObjects(result.text, beachNoHelpTranscription);
    XCTAssertEqual(result.reason, SPXResultReason_RecognizedSpeech);
    XCTAssertTrue(result.duration > 0);
    XCTAssertTrue(result.offset > 0);
    XCTAssertTrue([result.resultId length] > 0);
}

@end

