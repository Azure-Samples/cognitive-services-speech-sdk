//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import <XCTest/XCTest.h>
#import <AVFoundation/AVFoundation.h>
#import <MicrosoftCognitiveServicesSpeech/SPXSpeechApi.h>

@interface SPXSpeechRecognitionEndtoEndTest : XCTestCase {
    NSString *weatherFileName;
    NSString *weatherTextEnglish;
    NSMutableDictionary *result;

    NSPredicate *sessionStoppedCountPred;

    double timeoutInSeconds;
}

- (void) setAudioSource;

@property (nonatomic, assign) NSString * speechKey;
@property (nonatomic, assign) NSString * serviceRegion;
@property (nonatomic, retain) SPXSpeechRecognizer* speechRecognizer;
@property (nonatomic, retain) SPXAudioConfiguration* audioConfig;
@property (nonatomic, retain) SPXConnection* connection;
@end


@implementation SPXSpeechRecognitionEndtoEndTest

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

- (void)setAudioSource {
    // should be overridden in derived class
}

- (void)tearDown {
    [super tearDown];
}

- (void)_testContinuousRecognition {
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

- (void)_testRecognizeAsync {
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

- (void)_testRecognizeOnce {
    SPXSpeechRecognitionResult *result = [self.speechRecognizer recognizeOnce];
    NSLog(@"recognition result:%@. Status %ld. offset %llu duration %llu resultid:%@",
          result.text, (long)result.reason, result.offset, result.duration, result.resultId);

    XCTAssertTrue([result.text isEqualToString:weatherTextEnglish], "Final Result Text does not match");
    XCTAssertTrue(result.reason == SPXResultReason_RecognizedSpeech);
    XCTAssertTrue(result.duration > 0);
    XCTAssertTrue(result.offset > 0);
    XCTAssertTrue([result.resultId length] > 0);
}

- (void)_testRecognizerWithMultipleHandlers {
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

- (void)_testContinuousRecognitionWithPreConnection {
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

    XCTAssertTrue([[self->result valueForKey:@"finalText"] isEqualToString: self->weatherTextEnglish]);
    XCTAssertTrue([[self->result valueForKey:@"finalResultCount"] isEqualToNumber:@1]);
    long connectedEventCount = [[self->result valueForKey:@"connectedCount"] integerValue];
    long disconnectedEventCount = [[self->result valueForKey:@"disconnectedCount"] integerValue];
    XCTAssertTrue(connectedEventCount > 0, @"The connected event count must be greater than 0. connectedEventCount=%ld", connectedEventCount);
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
    [self.connection open:NO];

    NSPredicate *connectedCountPred = [NSPredicate predicateWithBlock: ^BOOL (id  _Nullable evaluatedObject, NSDictionary<NSString *,id> * _Nullable bindings){
        return [[evaluatedObject valueForKey:@"connectedCount"] isEqualToNumber:@1];
    }];

    // wait for connection
    [self expectationForPredicate:connectedCountPred evaluatedWithObject:result handler:nil];
    [self waitForExpectationsWithTimeout:timeoutInSeconds handler:nil];

    // trigger recognition
    [self.speechRecognizer recognizeOnceAsync: ^ (SPXSpeechRecognitionResult *srresult) {
        XCTAssertTrue([srresult.text isEqualToString:self->weatherTextEnglish], "Final Result Text does not match");
    }];

    // wait for session stopped event result
    [self expectationForPredicate:sessionStoppedCountPred evaluatedWithObject:result handler:nil];
    [self waitForExpectationsWithTimeout:timeoutInSeconds handler:nil];
    // check result
    XCTAssertTrue([[self->result valueForKey:@"finalText"] isEqualToString: self->weatherTextEnglish]);
    XCTAssertTrue([[self->result valueForKey:@"finalResultCount"] compare:@0] == NSOrderedDescending);

    // check event counts
    long connectedEventCount = [[self->result valueForKey:@"connectedCount"] integerValue];
    long disconnectedEventCount = [[self->result valueForKey:@"disconnectedCount"] integerValue];
    XCTAssertTrue(connectedEventCount > 0, @"The connected event count must be greater than 0. connectedEventCount=%ld", connectedEventCount);
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
    NSString *weatherFile = [bundle pathForResource: weatherFileName ofType:@"wav"];

    self.audioConfig = [[SPXAudioConfiguration alloc] initWithWavFileInput:weatherFile];
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


@interface SPXSpeechRecognitionTest : XCTestCase {
}
@property (nonatomic, assign) NSString * speechKey;
@property (nonatomic, assign) NSString * serviceRegion;
@end


@implementation SPXSpeechRecognitionTest

- (void)setUp {
    [super setUp];

    self.speechKey = [[[NSProcessInfo processInfo] environment] objectForKey:@"subscriptionKey"];
    self.serviceRegion = [[[NSProcessInfo processInfo] environment] objectForKey:@"serviceRegion"];
}

- (void)testInvalidSubscriptionKey {
    NSString *weatherFileName = @"whatstheweatherlike";

    NSBundle *bundle = [NSBundle bundleForClass:[self class]];
    NSString *weatherFile = [bundle pathForResource: weatherFileName ofType:@"wav"];

    SPXAudioConfiguration* weatherAudioSource = [[SPXAudioConfiguration alloc] initWithWavFileInput:weatherFile];

    SPXSpeechConfiguration *speechConfig = [[SPXSpeechConfiguration alloc] initWithSubscription:@"InvalidSubscriptionKey" region:@"westus"];
    XCTAssertNotNil(speechConfig);

    // this shouldn't throw, but fail on opening the connection
    SPXSpeechRecognizer *speechRecognizer = [[SPXSpeechRecognizer alloc] initWithSpeechConfiguration:speechConfig audioConfiguration:weatherAudioSource];
    XCTAssertNotNil(speechRecognizer);

    SPXSpeechRecognitionResult *result = [speechRecognizer recognizeOnce];
    NSLog(@"recognition result:%@. Status %ld. offset %llu duration %llu resultid:%@",
          result.text, (long)result.reason, result.offset, result.duration, result.resultId);

    SPXCancellationDetails *details = [[SPXCancellationDetails alloc] initFromCanceledRecognitionResult:result];
    NSLog(@"Error Details: %@", details.errorDetails);

    XCTAssertTrue([result.text isEqualToString:@""], "Final Result Text does not match");
    XCTAssertEqual(details.reason, SPXCancellationReason_Error);
    XCTAssertEqualObjects(details.errorDetails, @"WebSocket Upgrade failed with an authentication error (401). Please check for correct subscription key (or authorization token) and region name.");
    XCTAssertEqual(result.reason, SPXResultReason_Canceled);
    XCTAssertEqual(result.duration, 0);
    XCTAssertEqual(result.offset, 0);
    XCTAssertGreaterThan([result.resultId length], 0);

}

- (void)testInvalidRegion {
    NSString *weatherFileName = @"whatstheweatherlike";

    NSBundle *bundle = [NSBundle bundleForClass:[self class]];
    NSString *weatherFile = [bundle pathForResource: weatherFileName ofType:@"wav"];

    SPXAudioConfiguration* weatherAudioSource = [[SPXAudioConfiguration alloc] initWithWavFileInput:weatherFile];

    SPXSpeechConfiguration *speechConfig = [[SPXSpeechConfiguration alloc] initWithSubscription:self.speechKey region:@"InvalidServiceRegion"];
    XCTAssertNotNil(speechConfig);

    // this shouldn't throw, but fail on opening the connection
    SPXSpeechRecognizer *speechRecognizer = [[SPXSpeechRecognizer alloc] initWithSpeechConfiguration:speechConfig audioConfiguration:weatherAudioSource];
    XCTAssertNotNil(speechRecognizer);

    SPXSpeechRecognitionResult *result = [speechRecognizer recognizeOnce];
    NSLog(@"recognition result:%@. Status %ld. offset %llu duration %llu resultid:%@",
          result.text, (long)result.reason, result.offset, result.duration, result.resultId);

    SPXCancellationDetails *details = [[SPXCancellationDetails alloc] initFromCanceledRecognitionResult:result];
    NSLog(@"Error Details: %@", details.errorDetails);

    XCTAssertTrue([result.text isEqualToString:@""], "Final Result Text does not match");
    XCTAssertEqual(details.reason, SPXCancellationReason_Error);
    XCTAssertEqualObjects(details.errorDetails, @"Connection failed (no connection to the remote host). Internal error: 11. Error details: 0. Please check network connection, firewall setting, and the region name used to create speech factory.");
    XCTAssertEqual(result.reason, SPXResultReason_Canceled);
    XCTAssertEqual(result.duration, 0);
    XCTAssertEqual(result.offset, 0);
    XCTAssertGreaterThan([result.resultId length], 0);
}

- (void)testEmptyRegion {
    SPXSpeechConfiguration *speechConfig = [[SPXSpeechConfiguration alloc] initWithSubscription:@"InvalidSubscriptionKey" region:@""];
    XCTAssertNil(speechConfig);
}

- (void)testEmptyKey {
    SPXSpeechConfiguration *speechConfig = [[SPXSpeechConfiguration alloc] initWithSubscription:@"" region:@"westus"];
    XCTAssertNil(speechConfig);
}

- (void)testInvalidWavefileName {
    SPXAudioConfiguration* invalidAudioSource = [[SPXAudioConfiguration alloc] initWithWavFileInput:@"invalidFilename"];

    SPXSpeechConfiguration *speechConfig = [[SPXSpeechConfiguration alloc] initWithSubscription:@"InvalidKey" region:@"westus"];
    XCTAssertNotNil(speechConfig);

    SPXSpeechRecognizer *r = [[SPXSpeechRecognizer alloc] initWithSpeechConfiguration:speechConfig audioConfiguration:invalidAudioSource];
    XCTAssertNil(r);
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
    NSString *weatherFile = [bundle pathForResource: weatherFileName ofType:@"wav"];
    NSURL *weatherUrl = [NSURL URLWithString:weatherFile];

    NSError *error = nil;
    AVAudioFile *audioFile = [[AVAudioFile alloc] initForReading:weatherUrl commonFormat:AVAudioPCMFormatInt16 interleaved:NO error:&error];
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
}

- (void) setAudioSource;

@end

@implementation SPXSpeechRecognitionPushStreamEndtoEndTest

- (void) setAudioSource {
    NSBundle *bundle = [NSBundle bundleForClass:[self class]];
    NSString *weatherFile = [bundle pathForResource: weatherFileName ofType:@"wav"];

    NSURL *weatherUrl = [NSURL URLWithString:weatherFile];
    NSError *error = nil;
    audioFile = [[AVAudioFile alloc] initForReading:weatherUrl commonFormat:AVAudioPCMFormatInt16 interleaved:NO error:&error];

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

    self->stream = [[SPXPushAudioInputStream alloc] init];

    self.audioConfig  = [[SPXAudioConfiguration alloc] initWithStreamInput:self->stream];
}

- (void)testContinuousRecognition {
    [self.speechRecognizer startContinuousRecognition];
    const AVAudioFrameCount nBytesToRead = 3200;
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

            NSLog(@"Read %d bytes from file", (int)nBytesRead);

            NSData *data = [NSData dataWithBytesNoCopy:buffer.int16ChannelData[0] length:nBytesRead freeWhenDone:NO];
            NSLog(@"%d bytes data returned", (int)[data length]);

            [self->stream write:data];
            NSLog(@"Wrote %d bytes to stream", (int)[data length]);
        }

        [NSThread sleepForTimeInterval:0.1f];
    }

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

@end
