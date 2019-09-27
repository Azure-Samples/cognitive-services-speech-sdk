//
// Copyright (c) Microsoft.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "ViewController.h"
#import <AVFoundation/AVFoundation.h>
#import <MicrosoftCognitiveServicesSpeech/SPXSpeechApi.h>


@interface ViewController () {
    NSString *speechKey;
    NSString *serviceRegion;
}

@property (strong, nonatomic) IBOutlet UIButton *recognizeFromMP3FileButton;

@property (strong, nonatomic) IBOutlet UILabel *recognitionResultLabel;

- (IBAction)recognizeFromMP3FileButtonTapped:(UIButton *)sender;
@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    speechKey = @"YourSubscriptionKey";
    serviceRegion = @"YourServiceRegion";

    [self.view setBackgroundColor:[UIColor whiteColor]];

    self.recognizeFromMP3FileButton = [UIButton buttonWithType:UIButtonTypeSystem];
    [self.recognizeFromMP3FileButton addTarget:self action:@selector(recognizeFromMP3FileButtonTapped:) forControlEvents:UIControlEventTouchUpInside];
    [self.recognizeFromMP3FileButton setTitle:@"Recognize from MP3 file" forState:UIControlStateNormal];
    [self.recognizeFromMP3FileButton setFrame:CGRectMake(50.0, 100.0, 300.0, 50.0)];
    self.recognizeFromMP3FileButton.accessibilityIdentifier = @"recognize_mic_button";
    [self.view addSubview:self.recognizeFromMP3FileButton];

    self.recognitionResultLabel = [[UILabel alloc] initWithFrame:CGRectMake(50.0, 350.0, 300.0, 400.0)];
    self.recognitionResultLabel.lineBreakMode = NSLineBreakByWordWrapping;
    self.recognitionResultLabel.numberOfLines = 0;
    self.recognitionResultLabel.accessibilityIdentifier = @"result_label";
    [self.recognitionResultLabel setText:@"Press a button!"];

    [self.view addSubview:self.recognitionResultLabel];
}

- (IBAction)recognizeFromMP3FileButtonTapped:(UIButton *)sender {
    dispatch_async(dispatch_get_global_queue(QOS_CLASS_DEFAULT, 0), ^{
        [self recognizeFromMP3File];
    });
}

/*
 * Performs speech recognition from a MP3 file.
 */
- (void)recognizeFromMP3File {
    NSBundle *mainBundle = [NSBundle mainBundle];
    NSString *weatherFile = [mainBundle pathForResource: @"whatstheweatherlike" ofType:@"mp3"];
    if (!weatherFile) {
        NSLog(@"Cannot find audio file!");
        [self updateRecognitionErrorText:(@"Cannot find audio file")];
        return;
    }

    // <setup-stream>
    SPXAudioStreamContainerFormat compressedStreamFormat = SPXAudioStreamContainerFormat_MP3;
    SPXAudioStreamFormat *audioFormat = [[SPXAudioStreamFormat alloc] initUsingCompressedFormat:compressedStreamFormat];
    SPXPushAudioInputStream* stream = [[SPXPushAudioInputStream alloc] initWithAudioFormat:audioFormat];

    SPXAudioConfiguration* audioConfig = [[SPXAudioConfiguration alloc] initWithStreamInput:stream];
    if (!audioConfig) {
        NSLog(@"Error creating stream!");
        [self updateRecognitionErrorText:(@"Error creating stream!")];
        return;
    }
    // </setup-stream>

    SPXSpeechConfiguration *speechConfig = [[SPXSpeechConfiguration alloc] initWithSubscription:speechKey region:serviceRegion];
    if (!speechConfig) {
        NSLog(@"Could not load speech config");
        [self updateRecognitionErrorText:(@"Speech Config Error")];
        return;
    }

    SPXSpeechRecognizer* speechRecognizer = [[SPXSpeechRecognizer alloc] initWithSpeechConfiguration:speechConfig audioConfiguration:audioConfig];
    if (!speechRecognizer) {
        NSLog(@"Could not create speech recognizer");
        [self updateRecognitionResultText:(@"Speech Recognition Error")];
        return;
    }

    // connect callbacks
    [speechRecognizer addRecognizingEventHandler: ^ (SPXSpeechRecognizer *recognizer, SPXSpeechRecognitionEventArgs *eventArgs) {
        NSLog(@"Received intermediate result event. SessionId: %@, recognition result:%@. Status %ld. offset %llu duration %llu resultid:%@", eventArgs.sessionId, eventArgs.result.text, (long)eventArgs.result.reason, eventArgs.result.offset, eventArgs.result.duration, eventArgs.result.resultId);
        [self updateRecognitionStatusText:eventArgs.result.text];
    }];


    [speechRecognizer addRecognizedEventHandler: ^ (SPXSpeechRecognizer *recognizer, SPXSpeechRecognitionEventArgs *eventArgs) {
        NSLog(@"Received final result event. SessionId: %@, recognition result:%@. Status %ld. offset %llu duration %llu resultid:%@", eventArgs.sessionId, eventArgs.result.text, (long)eventArgs.result.reason, eventArgs.result.offset, eventArgs.result.duration, eventArgs.result.resultId);
        [self updateRecognitionResultText:eventArgs.result.text];
    }];

    // <push-compressed-stream>
    NSInputStream *compressedStream = [[NSInputStream alloc] initWithFileAtPath:weatherFile];
    [compressedStream open];
    NSLog(@"result of opening stream: %@, %lu", compressedStream.streamError, (unsigned long)compressedStream.streamStatus);
    if (nil == compressedStream)
    {
        NSLog(@"Error while opening file");
        audioConfig = nil;
        return;
    }

    // start recognizing
    [self updateRecognitionStatusText:(@"Recognizing from push stream...")];
    [speechRecognizer startContinuousRecognition];

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
            [stream close];
            break;
        }
        else if (0 > nBytesRead) {
            NSLog(@"error reading stream (%ld): %@ (%lu)", nBytesRead, compressedStream.streamError, compressedStream.streamStatus);
            [stream close];
            break;
        }
        else
        {
            NSLog(@"Read %lu bytes from file", nBytesRead);
            NSData *data = [NSData dataWithBytesNoCopy:buffer length:nBytesRead freeWhenDone:NO];

            [stream write:data];
            NSLog(@"Wrote %lu bytes to stream", [data length]);
        }

        [NSThread sleepForTimeInterval:0.1f];
    }

    [speechRecognizer stopContinuousRecognition];
    // </push-compressed-stream>
}

- (void)updateRecognitionResultText:(NSString *) resultText {
    dispatch_async(dispatch_get_main_queue(), ^{
        self.recognitionResultLabel.textColor = UIColor.blackColor;
        self.recognitionResultLabel.text = resultText;
    });
}

- (void)updateRecognitionErrorText:(NSString *) errorText {
    dispatch_async(dispatch_get_main_queue(), ^{
        self.recognitionResultLabel.textColor = UIColor.redColor;
        self.recognitionResultLabel.text = errorText;
    });
}

- (void)updateRecognitionStatusText:(NSString *) statusText {
    dispatch_async(dispatch_get_main_queue(), ^{
        self.recognitionResultLabel.textColor = UIColor.grayColor;
        self.recognitionResultLabel.text = statusText;
    });
}

@end
