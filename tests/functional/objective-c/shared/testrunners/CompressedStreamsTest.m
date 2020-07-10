//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import <XCTest/XCTest.h>
#import <MicrosoftCognitiveServicesSpeech/SPXSpeechApi.h>
#import "SPXSpeechRecognitionEndtoEndTest.h"


@interface CompressedPushStreamTest : SPXSpeechRecognitionEndtoEndTest {
SPXPushAudioInputStream* stream;
SPXAudioStreamContainerFormat compressedStreamFormat;
}
@end


@implementation CompressedPushStreamTest

- (void)setUp {
    [super setUp];
}

- (void)setAudioSource {
    [self setCompressedAudioSource:self->compressedStreamFormat];
}

- (void)setCompressedAudioSource:(SPXAudioStreamContainerFormat)compressedAudioFormat {
    SPXAudioStreamFormat *audioFormat = [[SPXAudioStreamFormat alloc] initUsingCompressedFormat:compressedAudioFormat];
    self->stream = [[SPXPushAudioInputStream alloc] initWithAudioFormat:audioFormat];

    self.audioConfig  = [[SPXAudioConfiguration alloc] initWithStreamInput:self->stream];
}

- (void)skipped_testContinuousRecognitionFromMP3FileWithPushStream {
    self->compressedStreamFormat = SPXAudioStreamContainerFormat_MP3;
    [self speechInit];
    [self pushFile:@"mp3"];
}

- (void)skipped_testContinuousRecognitionFromOpusFileWithPushStream {
    self->compressedStreamFormat = SPXAudioStreamContainerFormat_OGG_OPUS;
    [self speechInit];
    [self pushFile:@"opus"];
}

- (void)skipped_testContinuousRecognitionFromFlacFileWithPushStream {
    self->compressedStreamFormat = SPXAudioStreamContainerFormat_FLAC;
    [self speechInit];
    [self pushFile:@"flac"];
}

- (void)skipped_testContinuousRecognitionFromAlawFileWithPushStream {
    self->compressedStreamFormat = SPXAudioStreamContainerFormat_ALAW;
    [self speechInit];
    [self pushFile:@"alaw"];
}

- (void)skipped_testContinuousRecognitionFromMulawFileWithPushStream {
    self->compressedStreamFormat = SPXAudioStreamContainerFormat_MULAW;
    [self speechInit];
    [self pushFile:@"mulaw"];
}

- (void)pushFile:(NSString *)fileExtension {
    NSBundle *bundle = [NSBundle bundleForClass:[self class]];
    NSString *weatherFile = [bundle pathForResource: weatherFileName ofType:fileExtension];

    NSLog(@"weatherfile: %@", weatherFile);
    NSInputStream *compressedStream = [[NSInputStream alloc] initWithFileAtPath:weatherFile];

    [compressedStream open];
    NSLog(@"result of opening stream: %@, %lu", compressedStream.streamError, (unsigned long)compressedStream.streamStatus);

    if (nil == compressedStream)
    {
        NSLog(@"Error while opening file");
        self.audioConfig = nil;
        return;
    }

    [self.speechRecognizer startContinuousRecognition];

    const NSInteger nBytesToRead = 1000;
    // push data to stream;
    uint8_t *buffer = malloc(nBytesToRead);
    NSInteger nBytesRead = 0;
    while (1)
    {
        // read data
        nBytesRead = [compressedStream read:buffer maxLength:nBytesToRead];
        if (0 == nBytesRead) {
            NSLog(@"end of stream reached");
            [self->stream close];
            break;
        }
        else if (0 > nBytesRead) {
            NSLog(@"error reading stream (%ld): %@ (%lu)", nBytesRead, compressedStream.streamError, compressedStream.streamStatus);
            [self->stream close];
            break;
        }
        else
        {
            NSLog(@"Read %lu bytes from file", nBytesRead);
            NSData *data = [NSData dataWithBytesNoCopy:buffer length:nBytesRead freeWhenDone:NO];

            [self->stream write:data];
            NSLog(@"Wrote %lu bytes to stream", [data length]);
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
    [self.speechRecognizer stopContinuousRecognition];}

@end


@interface SPXSpeechRecognitionCompressedPullStreamEndtoEndTest : SPXSpeechRecognitionEndtoEndTest {
    SPXPullAudioInputStream* stream;
    SPXAudioStreamContainerFormat compressedStreamFormat;
}

- (void)setUp;
- (void)setAudioSource;
@end


@implementation SPXSpeechRecognitionCompressedPullStreamEndtoEndTest
- (void)setUp {
    [super setUp];
}

- (void)setAudioSource {
    [self setCompressedAudioSource:self->compressedStreamFormat];
}

- (void)setCompressedAudioSource:(SPXAudioStreamContainerFormat)compressedAudioFormat {
    NSBundle *bundle = [NSBundle bundleForClass:[self class]];
    NSString *weatherFileExtension;
    switch (self->compressedStreamFormat) {
        case SPXAudioStreamContainerFormat_MP3:
            weatherFileExtension = @"mp3";
            break;
        case SPXAudioStreamContainerFormat_OGG_OPUS:
            weatherFileExtension = @"opus";
            break;
        case SPXAudioStreamContainerFormat_FLAC:
            weatherFileExtension = @"flac";
            break;
        case SPXAudioStreamContainerFormat_ALAW:
            weatherFileExtension = @"alaw";
            break;
        case SPXAudioStreamContainerFormat_MULAW:
            weatherFileExtension = @"mulaw";
            break;
        default:
            break;
    }
    NSString *weatherFile = [bundle pathForResource: weatherFileName ofType:weatherFileExtension];

    NSLog(@"weatherfile: %@", weatherFile);
    NSInputStream *compressedStream = [[NSInputStream alloc] initWithFileAtPath:weatherFile];

    [compressedStream open];
    NSLog(@"result of opening stream: %@, %lu", compressedStream.streamError, (unsigned long)compressedStream.streamStatus);

    if (nil == compressedStream)
    {
        NSLog(@"Error while opening file");
        self.audioConfig = nil;
        return;
    }

    SPXAudioStreamFormat *audioFormat = [[SPXAudioStreamFormat alloc] initUsingCompressedFormat:compressedAudioFormat];
    self->stream = [[SPXPullAudioInputStream alloc]
                    initWithAudioFormat:audioFormat
                    readHandler:
                    ^NSInteger(NSMutableData *data, NSUInteger size) {
                        NSLog(@"Requested %lu bytes", size);
                        uint8_t *buffer = malloc(size);
                        NSInteger nBytesRead = 0;
                        nBytesRead = [compressedStream read:buffer maxLength:size];
                        if (0 == nBytesRead) {
                            NSLog(@"end of stream reached");
                            [compressedStream close];
                            nBytesRead = 0;
                        }
                        else if (0 > nBytesRead) {
                            NSLog(@"error reading stream (%ld): %@ (%lu)", nBytesRead, compressedStream.streamError, compressedStream.streamStatus);
                            [compressedStream close];
                            nBytesRead = 0;
                        }
                        else
                        {
                            NSLog(@"Read %lu bytes from file", nBytesRead);
                            NSRange range;
                            range.location = 0;
                            range.length = nBytesRead;
                            [data replaceBytesInRange:range withBytes:buffer];

                            NSLog(@"Wrote %lu bytes to stream", [data length]);
                        }
                        free(buffer);
                        return nBytesRead;
                    }
                    closeHandler:
                    ^(void) {
                    }];


    self.audioConfig = [[SPXAudioConfiguration alloc] initWithStreamInput:self->stream];
}

- (void) skipped_testRecognizeOnceMP3 {
    self->compressedStreamFormat = SPXAudioStreamContainerFormat_MP3;
    [self speechInit];
    [self _testRecognizeOnce];
}

- (void) skipped_testRecognizeOnceOPUS {
    self->compressedStreamFormat = SPXAudioStreamContainerFormat_OGG_OPUS;
    [self speechInit];
    [self _testRecognizeOnce];
}

- (void) skipped_testRecognizeOnceFLAC {
    self->compressedStreamFormat = SPXAudioStreamContainerFormat_FLAC;
    [self speechInit];
    [self _testRecognizeOnce];
}

- (void) skipped_testRecognizeOnceALAW {
    self->compressedStreamFormat = SPXAudioStreamContainerFormat_ALAW;
    [self speechInit];
    [self _testRecognizeOnce];
}

- (void) skipped_testRecognizeOnceMULAW {
    self->compressedStreamFormat = SPXAudioStreamContainerFormat_MULAW;
    [self speechInit];
    [self _testRecognizeOnce];
}


@end
