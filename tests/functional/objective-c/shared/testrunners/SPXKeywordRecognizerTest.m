//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import <XCTest/XCTest.h>
#import <AVFoundation/AVFoundation.h>
#import <MicrosoftCognitiveServicesSpeech/SPXSpeechApi.h>

@interface SPXKeywordRecognizerTest : XCTestCase {
    NSString *keywordUtteranceText;
    NSString *keywordUtteranceFileName;
    NSString *keywordRecognitionModelFileName;    

    NSMutableDictionary *result;
    NSPredicate *recognizedKeywordCountPred;
    double timeoutInSeconds;
}

@property (nonatomic, retain) SPXAudioConfiguration* audioConfig;
@property (nonatomic, retain) SPXKeywordRecognizer* keywordRecognizer;
@property (nonatomic, retain) SPXKeywordRecognitionModel* keywordRecognitionModel;

@end

@implementation SPXKeywordRecognizerTest

- (void)setUp {
    [super setUp];
    timeoutInSeconds = 20.;
    keywordUtteranceFileName = @"kws_whatstheweatherlike";
    keywordUtteranceText = @"Computer, what's the weather like?";
    keywordRecognitionModelFileName = @"kws";
        
    NSBundle *bundle = [NSBundle bundleForClass:[self class]];
    NSString *targetFile = [bundle pathForResource: keywordUtteranceFileName ofType:@"wav"];
    self.audioConfig = [[SPXAudioConfiguration alloc] initWithWavFileInput:targetFile];
    self.keywordRecognizer = [[SPXKeywordRecognizer alloc] init:self.audioConfig];
    targetFile = [bundle pathForResource: keywordRecognitionModelFileName ofType:@"table"];
    self.keywordRecognitionModel = [[SPXKeywordRecognitionModel alloc] initFromFile:targetFile];
    [self setEventHandlers];
}

- (void)setEventHandlers {
    result = [NSMutableDictionary new];
    [result setObject:@0 forKey:@"recognizedKeywordCount"];
    [result setObject:@0 forKey:@"canceledCount"];    
    [result setObject:@"" forKey:@"finalText"];

    __unsafe_unretained typeof(self) weakSelf = self;

    [self.keywordRecognizer addRecognizedEventHandler: ^ (SPXKeywordRecognizer *recognizer, SPXKeywordRecognitionEventArgs *eventArgs) {
        NSLog(@"Received final result event. SessionId: %@, recognition result:%@. Status %ld. offset %llu duration %llu resultid:%@", 
            eventArgs.sessionId, 
            eventArgs.result.text,
            (long)eventArgs.result.reason,
            eventArgs.result.offset, 
            eventArgs.result.duration,
            eventArgs.result.resultId);

        if (SPXResultReason_RecognizedKeyword == eventArgs.result.reason) {
            if ([eventArgs.result.text length] != 0) {
                [weakSelf->result setObject:eventArgs.result.text forKey: @"finalText"];
                [weakSelf->result setObject:[NSNumber numberWithLong:[[weakSelf->result objectForKey:@"recognizedKeywordCount"] integerValue] + 1] forKey:@"recognizedKeywordCount"];
            }
        }
    }];
    
    [self.keywordRecognizer addCanceledEventHandler:^(SPXKeywordRecognizer * recognizer, SPXKeywordRecognitionCanceledEventArgs * eventArgs) {
        NSLog(@"Received canceled event. SessionId: %@", eventArgs.sessionId);
        [weakSelf->result setObject:[NSNumber numberWithLong:[[weakSelf->result objectForKey:@"canceledCount"] integerValue] + 1] forKey:@"canceledCount"];
    }];
        
    recognizedKeywordCountPred = [NSPredicate predicateWithBlock: ^BOOL (id  _Nullable evaluatedObject, NSDictionary<NSString *,id> * _Nullable bindings){
        return [[evaluatedObject valueForKey:@"recognizedKeywordCount"] isEqualToNumber:@1];
    }];
}

- (void)tearDown {
    [super tearDown];
}

- (void)testCreateKeywordRecognizer {
    SPXKeywordRecognizer* keywordRecognizer = [[SPXKeywordRecognizer alloc] init];
    XCTAssertNotNil(keywordRecognizer);
}

- (void)testCreateKeywordRecognitionModel {
    NSBundle *bundle = [NSBundle bundleForClass:[self class]];
    NSString *targetFile = [bundle pathForResource: keywordRecognitionModelFileName ofType:@"table"];
    SPXKeywordRecognitionModel* keywordRecognitionModel = [[SPXKeywordRecognitionModel alloc] initFromFile:targetFile];
    XCTAssertNotNil(keywordRecognitionModel);
}

- (void)testKeywordRecognitionStop {
    
    XCTestExpectation *expectation = [self expectationWithDescription:@"async request"];

    __block SPXKeywordRecognitionResult * asyncResult;
    __block BOOL stopResult = false;
    
    [self.keywordRecognizer recognizeOnceAsync: ^ (SPXKeywordRecognitionResult *srresult) {
        NSLog(@"Keyword recognizeOnce completed");
        asyncResult = srresult;
    }keywordModel:self.keywordRecognitionModel];
    
    [self.keywordRecognizer stopRecognitionAsync: ^(BOOL stopped, NSError * err) {
        NSLog(@"Keyword recognition stopping completed");
        stopResult = stopped;
        [expectation fulfill];
    }];

    NSLog(@"Waiting for completion of keyword recognition stopping");
    [self waitForExpectationsWithTimeout:timeoutInSeconds handler:nil];

    XCTAssertTrue(stopResult);
}

@end

@interface SPXKeywordRecognizerPushStreamTest : SPXKeywordRecognizerTest {
    AVAudioFile *audioFile;
    SPXPushAudioInputStream* stream;
    NSUInteger sampleRate;
}

- (void)setPushStreamAudio;

@end

@implementation SPXKeywordRecognizerPushStreamTest

- (void)setPushStreamAudio {
    sampleRate = 16000;
    
    NSBundle *bundle = [NSBundle bundleForClass:[self class]];
    NSString *targetFile = [bundle pathForResource:keywordUtteranceFileName ofType:@"wav"];

    NSURL *targetUrl = [NSURL URLWithString:targetFile];
    NSError *error = nil;
    audioFile = [[AVAudioFile alloc] initForReading:targetUrl commonFormat:AVAudioPCMFormatInt16 interleaved:NO error:&error];

    if (error) {
        NSLog(@"Error while opening file: %@", error);
        self->stream = nil;
        self.audioConfig = nil;
        return;
    }

    // check the format of the file
    NSAssert(1 == audioFile.fileFormat.channelCount, @"Bad channel count");
    NSAssert(self->sampleRate == audioFile.fileFormat.sampleRate, @"Unexpected sample rate");

    self->stream = [[SPXPushAudioInputStream alloc] init];
    self.audioConfig  = [[SPXAudioConfiguration alloc] initWithStreamInput:self->stream];
    self.keywordRecognizer = [[SPXKeywordRecognizer alloc] init:self.audioConfig];
    [self setEventHandlers];
}

- (void)testPushStreamKeywordRecognition {
    [self setPushStreamAudio];

    XCTestExpectation *expectation = [self expectationWithDescription:@"async request"];

    __block SPXKeywordRecognitionResult * asyncResult;
    [self.keywordRecognizer recognizeOnceAsync: ^ (SPXKeywordRecognitionResult *srresult) {
        asyncResult = srresult;
        [expectation fulfill];
    }keywordModel:self.keywordRecognitionModel];
    
    const AVAudioFrameCount nBytesToRead = 5000;
    const NSInteger bytesPerFrame = self->audioFile.fileFormat.streamDescription->mBytesPerFrame;
    AVAudioPCMBuffer *buffer = [[AVAudioPCMBuffer alloc] initWithPCMFormat:self->audioFile.fileFormat frameCapacity:nBytesToRead / bytesPerFrame];

    NSAssert(1 == buffer.stride, @"only one channel allowed");
    NSAssert(nil != buffer.int16ChannelData, @"assure correct format");

    // push data to stream;
    while (1) {
        NSError *bufferError = nil;
        bool success = [self->audioFile readIntoBuffer:buffer error:&bufferError];
        if (!success) {
            NSLog(@"Read error on stream: %@", bufferError);
            [self->stream close];
            break;
        }
        else {
            NSInteger nBytesRead = [buffer frameLength] * bytesPerFrame;
            if (0 == nBytesRead) {
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

    NSLog(@"Waiting for keyword recognition async callback");
    [self waitForExpectationsWithTimeout:timeoutInSeconds handler:nil];

    [self expectationForPredicate:self->recognizedKeywordCountPred evaluatedWithObject:self->result handler:nil];
    [self waitForExpectationsWithTimeout:self->timeoutInSeconds handler:nil];
    
    BOOL doesContainKeyword = ([asyncResult.text rangeOfString:@"Computer"].location != NSNotFound);
    XCTAssertTrue(doesContainKeyword);
    XCTAssertEqual(asyncResult.reason, SPXResultReason_RecognizedKeyword);
}

@end

@interface SPXKeywordRecognizerPullStreamTest : SPXKeywordRecognizerTest {
    SPXPullAudioInputStream* stream;
}

- (void)setPullStreamAudio;

@end

@implementation SPXKeywordRecognizerPullStreamTest

- (void) setPullStreamAudio {
    NSBundle *bundle = [NSBundle bundleForClass:[self class]];
    NSString *targetFile = [bundle pathForResource:keywordUtteranceFileName ofType:@"wav"];
    NSURL *targetUrl = [NSURL URLWithString:targetFile];

    NSError *error = nil;
    AVAudioFile *audioFile = [[AVAudioFile alloc] initForReading:targetUrl commonFormat:AVAudioPCMFormatInt16 interleaved:NO error:&error];
    const NSInteger bytesPerFrame = audioFile.fileFormat.streamDescription->mBytesPerFrame;

    if (error) {
        NSLog(@"Error while opening file: %@", error);
        self->stream = nil;
        self.audioConfig = nil;
        return;
    }

    // check the format of the file
    NSAssert(1 == audioFile.fileFormat.channelCount, @"Bad channel count");
    NSAssert(16000 == audioFile.fileFormat.sampleRate, @"Unexpected sample rate");

    self->stream = [[SPXPullAudioInputStream alloc] initWithReadHandler: ^ NSInteger(NSMutableData *data, NSUInteger size) {
        
        AVAudioPCMBuffer *buffer = [[AVAudioPCMBuffer alloc] initWithPCMFormat:audioFile.fileFormat frameCapacity:(AVAudioFrameCount) size / bytesPerFrame];
        NSError *bufferError = nil;
        bool success = [audioFile readIntoBuffer:buffer error:&bufferError];
        
        NSInteger nBytes = 0;
        if (!success) {
            // returns 0 to close the stream on read error.
            NSLog(@"Read error on stream: %@", bufferError);
        }
        else {
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
    closeHandler: ^(void) {
    }];
    
    self.audioConfig = [[SPXAudioConfiguration alloc] initWithStreamInput:self->stream];
    self.keywordRecognizer = [[SPXKeywordRecognizer alloc] init:self.audioConfig];
    [self setEventHandlers];
}

- (void) testPullStreamKeywordRecognition {
    [self setPullStreamAudio];

    XCTestExpectation *expectation = [self expectationWithDescription:@"async request"];

    __block SPXKeywordRecognitionResult * asyncResult;
    [self.keywordRecognizer recognizeOnceAsync: ^ (SPXKeywordRecognitionResult *srresult) {
        asyncResult = srresult;
        [expectation fulfill];
    }keywordModel:self.keywordRecognitionModel];

    NSLog(@"Waiting for keyword recognition async callback");
    [self waitForExpectationsWithTimeout:timeoutInSeconds handler:nil];

    [self expectationForPredicate:self->recognizedKeywordCountPred evaluatedWithObject:self->result handler:nil];
    [self waitForExpectationsWithTimeout:self->timeoutInSeconds handler:nil];
    
    BOOL doesContainKeyword = ([asyncResult.text rangeOfString:@"Computer"].location != NSNotFound);
    XCTAssertTrue(doesContainKeyword);
    XCTAssertEqual(asyncResult.reason, SPXResultReason_RecognizedKeyword);
    
    SPXAudioDataStream *stream = [[SPXAudioDataStream alloc] initFromKeywordRecognitionResult:asyncResult];
    NSMutableData *data = [[NSMutableData alloc]initWithCapacity:100];
    BOOL canReadData = [stream canReadData:100];
    XCTAssertTrue(canReadData);
    NSUInteger readSize = [stream readData:data length:100];
    XCTAssertEqual(readSize, 100);
    [stream detachInput];
}

@end
