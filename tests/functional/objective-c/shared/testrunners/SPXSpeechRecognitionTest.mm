//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import <XCTest/XCTest.h>
#import <MicrosoftCognitiveServicesSpeech/SPXSpeechApi.h>

#import "stream_utils.hpp"

#include <iostream>
#include <fstream>


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
    std::fstream m_fs;
    SPXPullAudioInputStream* stream;
}

- (void) setAudioSource;

@end

@implementation SPXSpeechRecognitionPullStreamEndtoEndTest

- (void) setAudioSource {
    NSBundle *bundle = [NSBundle bundleForClass:[self class]];
    NSString *weatherFile = [bundle pathForResource: weatherFileName ofType:@"wav"];

    std::string audioFileName = [weatherFile UTF8String];
    if (audioFileName.empty())
        throw std::invalid_argument("Audio filename is empty");

    std::ios_base::openmode mode = std::ios_base::binary | std::ios_base::in;
    self->m_fs.open(audioFileName, mode);
    if (!self->m_fs.good())
        throw std::invalid_argument("Failed to open the specified audio file.");

    // Get audio format from the file header.
    GetFormatFromWavFile(self->m_fs);

    self->stream = [[SPXPullAudioInputStream alloc]
                                       initWithReadHandler:
                                       ^NSInteger(NSMutableData *data, NSUInteger size) {
                                           if (self->m_fs.eof()) {
                                               // returns 0 to indicate that the stream reaches end.
                                               return 0;
                                           }

                                           char* buffer = (char *)malloc(size);
                                           self->m_fs.read((char*)buffer, size);
                                           int count = (int)self->m_fs.gcount();
                                           if (!self->m_fs.eof() && !self->m_fs.good()) {
                                               // returns 0 to close the stream on read error.
                                               NSLog(@"Read error on stream.");
                                               count = 0;
                                           }
                                           else
                                           {
                                               NSRange range;
                                               range.location = 0;
                                               range.length = count;
                                               [data replaceBytesInRange:range withBytes:buffer];
                                               NSLog(@"%d bytes data returned", (int)[data length]);
                                           }
                                           free(buffer);
                                           // returns the number of bytes that have been read.
                                           return (NSInteger)count;
                                       }
                                       closeHandler:
                                       ^(void) {
                                           self->m_fs.close();
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
    std::fstream m_fs;
    SPXPushAudioInputStream* stream;
}

- (void) setAudioSource;

@end

@implementation SPXSpeechRecognitionPushStreamEndtoEndTest

- (void) setAudioSource {
    NSBundle *bundle = [NSBundle bundleForClass:[self class]];
    NSString *weatherFile = [bundle pathForResource: weatherFileName ofType:@"wav"];

    std::string audioFileName = [weatherFile UTF8String];
    if (audioFileName.empty()) {
        throw std::invalid_argument("Audio filename is empty");
    }

    std::ios_base::openmode mode = std::ios_base::binary | std::ios_base::in;
    self->m_fs.open(audioFileName, mode);
    if (!self->m_fs.good())
        throw std::invalid_argument("Failed to open the specified audio file.");

    // Get audio format from the file header.
    GetFormatFromWavFile(self->m_fs);

    self->stream = [[SPXPushAudioInputStream alloc] init];

    self.audioConfig  = [[SPXAudioConfiguration alloc] initWithStreamInput:self->stream];
}

- (void)testContinuousRecognition {
    [self.speechRecognizer startContinuousRecognition];

    // push stream;
    size_t bytes = 3200;
    char* buffer = (char *)malloc(bytes);
    while (1)
    {
        if (m_fs.eof()) {
            // returns 0 to indicate that the stream reaches end.
            [self->stream close];
            break;
        }
        [NSThread sleepForTimeInterval:0.2f];
        m_fs.read((char*)buffer, bytes);
        int count = (int)m_fs.gcount();
        if (!m_fs.eof() && !m_fs.good()) {
            // returns 0 to close the stream on read error.
            NSLog(@"Read error on stream.");
            [self->stream close];
            break;
        }
        else
        {
            NSData *data = [NSData dataWithBytes:buffer length:count];
            [self->stream write:data];
            NSLog(@"Wrote %d bytes", (int)[data length]);
        }
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
