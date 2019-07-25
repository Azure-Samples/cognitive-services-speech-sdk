//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "ViewController.h"

#import "../../shared/test.h"
#import "../../shared/microphone_test.h"
#import "../../shared/dummytest.h"

extern NSString *speechKey;
extern NSString *serviceRegion;
extern NSString *intentKey;
extern NSString *intentRegion;


@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    [DummyTest runTest:speechKey withRegion:serviceRegion];
    [EndToEndTests runTest:speechKey withRegion:serviceRegion withIntentKey:intentKey withIntentRegion:intentRegion];
    [self compressedStream];
}

- (void)microphoneTests {
    [MicrophoneTest runAsync:speechKey withRegion:serviceRegion];
    [MicrophoneTest runTranslation:speechKey withRegion:serviceRegion];
    [MicrophoneTest runContinuous:speechKey withRegion:serviceRegion];
}

- (void)compressedStream {
    SPXSpeechConfiguration *speechConfig = [[SPXSpeechConfiguration alloc] initWithSubscription:speechKey region:serviceRegion];
    NSAssert(speechConfig != nil, @"speech config is nil");
    
    NSBundle *bundle = [NSBundle bundleForClass:[self class]];
    NSString *weatherFileExtension = @"mp3";
    NSString *weatherFileName = @"whatstheweatherlike";
    NSString *weatherFile = [bundle pathForResource: weatherFileName ofType:weatherFileExtension];
    
    NSLog(@"weatherfile: %@", weatherFile);
    NSInputStream *compressedStream = [[NSInputStream alloc] initWithFileAtPath:weatherFile];
    
    [compressedStream open];
    NSLog(@"result of opening stream: %@, %lu", compressedStream.streamError, (unsigned long)compressedStream.streamStatus);
    
    SPXAudioConfiguration *audioConfig;
    if (nil == compressedStream)
    {
        NSLog(@"Error while opening file");
        return;
    }
    
    SPXAudioStreamFormat *audioFormat = [[SPXAudioStreamFormat alloc] initUsingCompressedFormat:SPXAudioStreamContainerFormat_MP3];
    
    SPXPullAudioInputStream* stream;
    stream = [[SPXPullAudioInputStream alloc]
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
    
    
    audioConfig = [[SPXAudioConfiguration alloc] initWithStreamInput:stream];
    
    SPXSpeechRecognizer* speechRecognizer;
    speechRecognizer = [[SPXSpeechRecognizer alloc] initWithSpeechConfiguration:speechConfig audioConfiguration:audioConfig];
    NSAssert(speechRecognizer != nil, @"speech recognizer is nil");
    
    __block bool end = false;
    __block NSString *finalResult;
    [speechRecognizer addRecognizedEventHandler: ^ (SPXSpeechRecognizer *recognizer, SPXSpeechRecognitionEventArgs *eventArgs) {
        NSLog(@"Received final result event. SessionId: %@, recognition result:%@. Status %ld. offset %llu duration %llu resultid:%@", eventArgs.sessionId, eventArgs.result.text, (long)eventArgs.result.reason, eventArgs.result.offset, eventArgs.result.duration, eventArgs.result.resultId);
        NSLog(@"Received JSON: %@", [eventArgs.result.properties getPropertyById:SPXSpeechServiceResponseJsonResult]);
        finalResult = eventArgs.result.text;
    }];
    [speechRecognizer addSessionStoppedEventHandler: ^ (SPXRecognizer *recognizer, SPXSessionEventArgs *eventArgs) {
        NSLog(@"Received SessionStopped event. SessionId: %@", eventArgs.sessionId);
        end = true;
    }];
    
    [speechRecognizer startContinuousRecognition];
    
    while (end == false)
        [NSThread sleepForTimeInterval:1.0f];
    [speechRecognizer stopContinuousRecognition];
    
    NSAssert([@"What's the weather like?" isEqualToString:finalResult], @"bad final result");
}

@end
