//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "ViewController.h"
#import "AudioRecorder.h"
#import <AVFoundation/AVFoundation.h>
#import <MicrosoftCognitiveServicesSpeech/SPXSpeechApi.h>

@interface ViewController () {
    NSString *speechKey;
    NSString *serviceRegion;
    NSString *pronunciationAssessmentReferenceText;
    AudioRecorder *recorder;
}

@property (strong, nonatomic) IBOutlet UIButton *recognizeFromFileButton;
@property (strong, nonatomic) IBOutlet UIButton *recognizeFromMicButton;
@property (strong, nonatomic) IBOutlet UIButton *recognizeWithPhraseHintButton;
@property (strong, nonatomic) IBOutlet UIButton *recognizeWithPushStreamButton;
@property (strong, nonatomic) IBOutlet UIButton *recognizeWithPullStreamButton;
@property (strong, nonatomic) IBOutlet UIButton *recognizeWithAutoLanguageDetectionButton;
@property (strong, nonatomic) IBOutlet UIButton *pronunciationAssessFromMicButton;
@property (strong, nonatomic) IBOutlet UIButton *pronunciationAssessFromFileButton;
@property (strong, nonatomic) IBOutlet UIButton *pronunciationAssessFromStreamButton;
@property (strong, nonatomic) IBOutlet UIButton *pronunciationAssessWithContentAssessmentButton;
@property (strong, nonatomic) IBOutlet UIButton *recognizeKeywordFromFileButton;

@property (strong, nonatomic) IBOutlet UILabel *recognitionResultLabel;

- (IBAction)recognizeFromFileButtonTapped:(UIButton *)sender;
- (IBAction)recognizeFromMicButtonTapped:(UIButton *)sender;
- (IBAction)recognizeWithPhraseHintButtonTapped:(UIButton *)sender;
- (IBAction)recognizeWithPushStreamButtonTapped:(UIButton *)sender;
- (IBAction)recognizeWithPullStreamButtonTapped:(UIButton *)sender;
- (IBAction)recognizeWithAutoLanguageDetectionButtonTapped:(UIButton *)sender;
- (IBAction)recognizeKeywordFromFileButtonTapped:(UIButton *)sender;
- (IBAction)pronunciationAssessFromMicButtonTapped:(UIButton *)sender;
- (IBAction)pronunciationAssessFromFileButtonTapped:(UIButton *)sender;
- (IBAction)pronunciationAssessFromStreamButtonTapped:(UIButton *)sender;
- (IBAction)pronunciationAssessWithContentAssessmentButtonTapped:(UIButton *)sender;
@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    speechKey = @"YourSubscriptionKey";
    serviceRegion = @"YourServiceRegion";
    pronunciationAssessmentReferenceText = @"Hello world.";

    [self.view setBackgroundColor:[UIColor whiteColor]];

    self.recognizeFromMicButton = [UIButton buttonWithType:UIButtonTypeSystem];
    [self.recognizeFromMicButton addTarget:self action:@selector(recognizeFromMicButtonTapped:) forControlEvents:UIControlEventTouchUpInside];
    [self.recognizeFromMicButton setTitle:@"Start rec from mic" forState:UIControlStateNormal];
    [self.recognizeFromMicButton setFrame:CGRectMake(50.0, 70.0, 300.0, 40.0)];
    self.recognizeFromMicButton.accessibilityIdentifier = @"recognize_mic_button";
    [self.view addSubview:self.recognizeFromMicButton];

    self.recognizeFromFileButton = [UIButton buttonWithType:UIButtonTypeSystem];
    [self.recognizeFromFileButton addTarget:self action:@selector(recognizeFromFileButtonTapped:) forControlEvents:UIControlEventTouchUpInside];
    [self.recognizeFromFileButton setTitle:@"Start rec from file" forState:UIControlStateNormal];
    [self.recognizeFromFileButton setFrame:CGRectMake(50.0, 110.0, 300.0, 40.0)];
    self.recognizeFromFileButton.accessibilityIdentifier = @"recognize_file_button";
    [self.view addSubview:self.recognizeFromFileButton];

    self.recognizeWithPhraseHintButton = [UIButton buttonWithType:UIButtonTypeSystem];
    [self.recognizeWithPhraseHintButton addTarget:self action:@selector(recognizeWithPhraseHintButtonTapped:) forControlEvents:UIControlEventTouchUpInside];
    [self.recognizeWithPhraseHintButton setTitle:@"Start rec from file with PhraseHint" forState:UIControlStateNormal];
    [self.recognizeWithPhraseHintButton setFrame:CGRectMake(50.0, 150.0, 300.0, 40.0)];
    self.recognizeWithPhraseHintButton.accessibilityIdentifier = @"recognize_phrase_hint_button";
    [self.view addSubview:self.recognizeWithPhraseHintButton];

    self.recognizeWithPushStreamButton = [UIButton buttonWithType:UIButtonTypeSystem];
    [self.recognizeWithPushStreamButton addTarget:self action:@selector(recognizeWithPushStreamButtonTapped:) forControlEvents:UIControlEventTouchUpInside];
    [self.recognizeWithPushStreamButton setTitle:@"Start rec from file with push stream" forState:UIControlStateNormal];
    [self.recognizeWithPushStreamButton setFrame:CGRectMake(50.0, 190.0, 300.0, 40.0)];
    self.recognizeWithPushStreamButton.accessibilityIdentifier = @"recognize_push_stream_button";
    [self.view addSubview:self.recognizeWithPushStreamButton];

    self.recognizeWithPullStreamButton = [UIButton buttonWithType:UIButtonTypeSystem];
    [self.recognizeWithPullStreamButton addTarget:self action:@selector(recognizeWithPullStreamButtonTapped:) forControlEvents:UIControlEventTouchUpInside];
    [self.recognizeWithPullStreamButton setTitle:@"Start rec from file with pull stream" forState:UIControlStateNormal];
    [self.recognizeWithPullStreamButton setFrame:CGRectMake(50.0, 230.0, 300.0, 40.0)];
    self.recognizeWithPullStreamButton.accessibilityIdentifier = @"recognize_pull_stream_button";
    [self.view addSubview:self.recognizeWithPullStreamButton];

    self.recognizeWithAutoLanguageDetectionButton = [UIButton buttonWithType:UIButtonTypeSystem];
    [self.recognizeWithAutoLanguageDetectionButton addTarget:self action:@selector(recognizeWithAutoLanguageDetectionButtonTapped:) forControlEvents:UIControlEventTouchUpInside];
    [self.recognizeWithAutoLanguageDetectionButton setTitle:@"Start rec with auto language detection" forState:UIControlStateNormal];
    [self.recognizeWithAutoLanguageDetectionButton setFrame:CGRectMake(50.0, 270.0, 300.0, 40.0)];
    self.recognizeWithAutoLanguageDetectionButton.accessibilityIdentifier = @"recognize_language_detection_button";
    [self.view addSubview:self.recognizeWithAutoLanguageDetectionButton];

    self.recognizeKeywordFromFileButton = [UIButton buttonWithType:UIButtonTypeSystem];
    [self.recognizeKeywordFromFileButton addTarget:self action:@selector(recognizeKeywordFromFileButtonTapped:) forControlEvents:UIControlEventTouchUpInside];
    [self.recognizeKeywordFromFileButton setTitle:@"Recognize keyword" forState:UIControlStateNormal];
    [self.recognizeKeywordFromFileButton setFrame:CGRectMake(50.0, 310.0, 300.0, 40.0)];
    self.recognizeKeywordFromFileButton.accessibilityIdentifier = @"recognize_keyword_button";
    [self.view addSubview:self.recognizeKeywordFromFileButton];

    self.pronunciationAssessFromMicButton = [UIButton buttonWithType:UIButtonTypeSystem];
    [self.pronunciationAssessFromMicButton addTarget:self action:@selector(pronunciationAssessFromMicButtonTapped:) forControlEvents:UIControlEventTouchUpInside];
    [self.pronunciationAssessFromMicButton setTitle:[NSString stringWithFormat:@"Start pronunciation assessment \n (Read out \"%@\")", pronunciationAssessmentReferenceText] forState:UIControlStateNormal];
    [self.pronunciationAssessFromMicButton titleLabel].lineBreakMode = NSLineBreakByWordWrapping;
    [self.pronunciationAssessFromMicButton titleLabel].textAlignment = NSTextAlignmentCenter;
    [self.pronunciationAssessFromMicButton setFrame:CGRectMake(50.0, 350.0, 300.0, 40.0)];
    self.pronunciationAssessFromMicButton.accessibilityIdentifier = @"pronunciation_assessment_button";
    [self.view addSubview:self.pronunciationAssessFromMicButton];

    self.pronunciationAssessFromFileButton = [UIButton buttonWithType:UIButtonTypeSystem];
    [self.pronunciationAssessFromFileButton addTarget:self action:@selector(pronunciationAssessFromFileButtonTapped:) forControlEvents:UIControlEventTouchUpInside];
    [self.pronunciationAssessFromFileButton setTitle:@"Start continuous pronunciation assessment from file." forState:UIControlStateNormal];
    [self.pronunciationAssessFromFileButton titleLabel].lineBreakMode = NSLineBreakByWordWrapping;
    [self.pronunciationAssessFromFileButton titleLabel].textAlignment = NSTextAlignmentCenter;
    [self.pronunciationAssessFromFileButton setFrame:CGRectMake(50.0, 390.0, 300.0, 40.0)];
    self.pronunciationAssessFromFileButton.accessibilityIdentifier = @"pronunciation_assessment_file_button";
    [self.view addSubview:self.pronunciationAssessFromFileButton];

    self.pronunciationAssessFromStreamButton = [UIButton buttonWithType:UIButtonTypeSystem];
    [self.pronunciationAssessFromStreamButton addTarget:self action:@selector(pronunciationAssessFromStreamButtonTapped:) forControlEvents:UIControlEventTouchUpInside];
    [self.pronunciationAssessFromStreamButton setTitle:@"Start pronunciation assessment from stream." forState:UIControlStateNormal];
    [self.pronunciationAssessFromStreamButton titleLabel].lineBreakMode = NSLineBreakByWordWrapping;
    [self.pronunciationAssessFromStreamButton titleLabel].textAlignment = NSTextAlignmentCenter;
    [self.pronunciationAssessFromStreamButton setFrame:CGRectMake(50.0, 430.0, 300.0, 40.0)];
    self.pronunciationAssessFromStreamButton.accessibilityIdentifier = @"pronunciation_assessment_stream_button";
    [self.view addSubview:self.pronunciationAssessFromStreamButton];
    
    self.pronunciationAssessWithContentAssessmentButton = [UIButton buttonWithType:UIButtonTypeSystem];
    [self.pronunciationAssessWithContentAssessmentButton addTarget:self action:@selector(pronunciationAssessWithContentAssessmentButtonTapped:) forControlEvents:UIControlEventTouchUpInside];
    [self.pronunciationAssessWithContentAssessmentButton setTitle:@"Start pronunciation assessment with content assessment." forState:UIControlStateNormal];
    [self.pronunciationAssessWithContentAssessmentButton titleLabel].lineBreakMode = NSLineBreakByWordWrapping;
    [self.pronunciationAssessWithContentAssessmentButton titleLabel].textAlignment = NSTextAlignmentCenter;
    [self.pronunciationAssessWithContentAssessmentButton setFrame:CGRectMake(50.0, 470.0, 300.0, 40.0)];
    self.pronunciationAssessWithContentAssessmentButton.accessibilityIdentifier = @"pronunciation_assessment_content_button";
    [self.view addSubview:self.pronunciationAssessWithContentAssessmentButton];
    
    self.recognitionResultLabel = [[UILabel alloc] initWithFrame:CGRectMake(50.0, 500.0, 300.0, 300.0)];
    self.recognitionResultLabel.lineBreakMode = NSLineBreakByWordWrapping;
    self.recognitionResultLabel.numberOfLines = 0;
    self.recognitionResultLabel.accessibilityIdentifier = @"result_label";
    [self.recognitionResultLabel setText:@"Press a button!"];

    [self.view addSubview:self.recognitionResultLabel];
}

- (IBAction)recognizeFromFileButtonTapped:(UIButton *)sender {
    dispatch_async(dispatch_get_global_queue(QOS_CLASS_DEFAULT, 0), ^{
        [self recognizeFromFile];
    });
}

- (IBAction)recognizeFromMicButtonTapped:(UIButton *)sender {
    dispatch_async(dispatch_get_global_queue(QOS_CLASS_DEFAULT, 0), ^{
        [self recognizeFromMicrophone];
    });
}

- (IBAction)recognizeWithPhraseHintButtonTapped:(UIButton *)sender {
    dispatch_async(dispatch_get_global_queue(QOS_CLASS_DEFAULT, 0), ^{
        [self recognizeWithPhraseHint];
    });
}

- (IBAction)recognizeWithPushStreamButtonTapped:(UIButton *)sender {
    dispatch_async(dispatch_get_global_queue(QOS_CLASS_DEFAULT, 0), ^{
        [self recognizeWithPushStream];
    });
}

- (IBAction)recognizeWithPullStreamButtonTapped:(UIButton *)sender {
    dispatch_async(dispatch_get_global_queue(QOS_CLASS_DEFAULT, 0), ^{
        [self recognizeWithPullStream];
    });
}

- (IBAction)recognizeWithAutoLanguageDetectionButtonTapped:(UIButton *)sender {
    dispatch_async(dispatch_get_global_queue(QOS_CLASS_DEFAULT, 0), ^{
        [self recognizeWithAutoLanguageDetection];
    });
}

- (IBAction)recognizeKeywordFromFileButtonTapped:(UIButton *)sender {
    dispatch_async(dispatch_get_global_queue(QOS_CLASS_DEFAULT, 0), ^{
        [self recognizeKeywordFromFile];
    });
}

- (IBAction)pronunciationAssessFromMicButtonTapped:(UIButton *)sender {
    if ([[(UIButton *)sender currentTitle]isEqualToString:@"Stop recording"])
    {
        [self->recorder stop];
        [self.pronunciationAssessFromMicButton setTitle:[NSString stringWithFormat:@"Start pronunciation assessment \n (Read out \"%@\")", self->pronunciationAssessmentReferenceText] forState:UIControlStateNormal];
    }
    else
    {
        [self.pronunciationAssessFromMicButton setTitle:@"Stop recording" forState:UIControlStateNormal];
        dispatch_async(dispatch_get_global_queue(QOS_CLASS_DEFAULT, 0), ^{
            [self pronunciationAssessFromMicrophone];
        });
    }
}

- (IBAction)pronunciationAssessFromFileButtonTapped:(UIButton *)sender {
    dispatch_async(dispatch_get_global_queue(QOS_CLASS_DEFAULT, 0), ^{
        [self pronunciationAssessFromFile];
    });
}

- (IBAction)pronunciationAssessFromStreamButtonTapped:(UIButton *)sender {
    dispatch_async(dispatch_get_global_queue(QOS_CLASS_DEFAULT, 0), ^{
        [self pronunciationAssessFromStream];
    });
}

- (IBAction)pronunciationAssessWithContentAssessmentButtonTapped:(UIButton *)sender {
    dispatch_async(dispatch_get_global_queue(QOS_CLASS_DEFAULT, 0), ^{
        [self pronunciationAssessWithContentAssessment];
    });
}

/*
 * Performs speech recognition from a RIFF wav file.
 */
- (void)recognizeFromFile {
    NSBundle *mainBundle = [NSBundle mainBundle];
    NSString *weatherFile = [mainBundle pathForResource: @"whatstheweatherlike" ofType:@"wav"];
    NSLog(@"weatherFile path: %@", weatherFile);
    if (!weatherFile) {
        NSLog(@"Cannot find audio file!");
        [self updateRecognitionErrorText:(@"Cannot find audio file")];
        return;
    }

    SPXAudioConfiguration* weatherAudioSource = [[SPXAudioConfiguration alloc] initWithWavFileInput:weatherFile];
    if (!weatherAudioSource) {
        NSLog(@"Loading audio file failed!");
        [self updateRecognitionErrorText:(@"Audio Error")];
        return;
    }

    SPXSpeechConfiguration *speechConfig = [[SPXSpeechConfiguration alloc] initWithSubscription:speechKey region:serviceRegion];
    if (!speechConfig) {
        NSLog(@"Could not load speech config");
        [self updateRecognitionErrorText:(@"Speech Config Error")];
        return;
    }

    [self updateRecognitionStatusText:(@"Recognizing...")];

    SPXSpeechRecognizer* speechRecognizer = [[SPXSpeechRecognizer alloc] initWithSpeechConfiguration:speechConfig audioConfiguration:weatherAudioSource];
    if (!speechRecognizer) {
        NSLog(@"Could not create speech recognizer");
        [self updateRecognitionResultText:(@"Speech Recognition Error")];
        return;
    }

    SPXSpeechRecognitionResult *speechResult = [speechRecognizer recognizeOnce];
    if (SPXResultReason_Canceled == speechResult.reason) {
        SPXCancellationDetails *details = [[SPXCancellationDetails alloc] initFromCanceledRecognitionResult:speechResult];
        NSLog(@"Speech recognition was canceled: %@. Did you pass the correct key/region combination?", details.errorDetails);
        [self updateRecognitionErrorText:([NSString stringWithFormat:@"Canceled: %@", details.errorDetails ])];
    } else if (SPXResultReason_RecognizedSpeech == speechResult.reason) {
        NSLog(@"Speech recognition result received: %@", speechResult.text);
        [self updateRecognitionResultText:(speechResult.text)];
    } else {
        NSLog(@"There was an error.");
        [self updateRecognitionErrorText:(@"Speech Recognition Error")];
    }
}

/*
 * Performs speech recognition on audio data from the default microphone.
 */
- (void)recognizeFromMicrophone {
    SPXSpeechConfiguration *speechConfig = [[SPXSpeechConfiguration alloc] initWithSubscription:speechKey region:serviceRegion];
    if (!speechConfig) {
        NSLog(@"Could not load speech config");
        [self updateRecognitionErrorText:(@"Speech Config Error")];
        return;
    }

    [self updateRecognitionStatusText:(@"Recognizing...")];

    SPXSpeechRecognizer* speechRecognizer = [[SPXSpeechRecognizer alloc] init:speechConfig];
    if (!speechRecognizer) {
        NSLog(@"Could not create speech recognizer");
        [self updateRecognitionResultText:(@"Speech Recognition Error")];
        return;
    }

    SPXSpeechRecognitionResult *speechResult = [speechRecognizer recognizeOnce];
    if (SPXResultReason_Canceled == speechResult.reason) {
        SPXCancellationDetails *details = [[SPXCancellationDetails alloc] initFromCanceledRecognitionResult:speechResult];
        NSLog(@"Speech recognition was canceled: %@. Did you pass the correct key/region combination?", details.errorDetails);
        [self updateRecognitionErrorText:([NSString stringWithFormat:@"Canceled: %@", details.errorDetails ])];
    } else if (SPXResultReason_RecognizedSpeech == speechResult.reason) {
        NSLog(@"Speech recognition result received: %@", speechResult.text);
        [self updateRecognitionResultText:(speechResult.text)];
    } else {
        NSLog(@"There was an error.");
        [self updateRecognitionErrorText:(@"Speech Recognition Error")];
    }
}

/*
 * Performs speech recognition on audio data from a wav file, with recognition hints in the form of a text phrase.
 */
- (void)recognizeWithPhraseHint {
    NSBundle *mainBundle = [NSBundle mainBundle];
    NSString *beachFile = [mainBundle pathForResource: @"wreck-a-nice-beach" ofType:@"wav"];
    if (!beachFile) {
        NSLog(@"Cannot find audio file!");
        [self updateRecognitionErrorText:(@"Cannot find audio file")];
        return;
    }

    SPXAudioConfiguration* weatherAudioSource = [[SPXAudioConfiguration alloc] initWithWavFileInput:beachFile];
    if (!weatherAudioSource) {
        NSLog(@"Loading audio file failed!");
        [self updateRecognitionErrorText:(@"Audio Error")];
        return;
    }

    SPXSpeechConfiguration *speechConfig = [[SPXSpeechConfiguration alloc] initWithSubscription:speechKey region:serviceRegion];
    if (!speechConfig) {
        NSLog(@"Could not load speech config");
        [self updateRecognitionErrorText:(@"Speech Config Error")];
        return;
    }

    SPXSpeechRecognizer* speechRecognizer = [[SPXSpeechRecognizer alloc] initWithSpeechConfiguration:speechConfig audioConfiguration:weatherAudioSource];
    if (!speechRecognizer) {
        NSLog(@"Could not create speech recognizer");
        [self updateRecognitionResultText:(@"Speech Recognition Error")];
        return;
    }

    // add a phrase hint to the recognizer's grammar
    SPXPhraseListGrammar * phraseListGrammar = [[SPXPhraseListGrammar alloc] initWithRecognizer:speechRecognizer];
    [phraseListGrammar addPhrase:@"Wreck a nice beach"];

    [self updateRecognitionStatusText:(@"Recognizing...")];

    SPXSpeechRecognitionResult *speechResult = [speechRecognizer recognizeOnce];
    if (SPXResultReason_Canceled == speechResult.reason) {
        SPXCancellationDetails *details = [[SPXCancellationDetails alloc] initFromCanceledRecognitionResult:speechResult];
        NSLog(@"Speech recognition was canceled: %@. Did you pass the correct key/region combination?", details.errorDetails);
        [self updateRecognitionErrorText:([NSString stringWithFormat:@"Canceled: %@", details.errorDetails ])];
    } else if (SPXResultReason_RecognizedSpeech == speechResult.reason) {
        NSLog(@"Speech recognition result received: %@", speechResult.text);
        [self updateRecognitionResultText:(speechResult.text)];
    } else {
        NSLog(@"There was an error.");
        [self updateRecognitionErrorText:(@"Speech Recognition Error")];
    }
}

/*
 * Performs continuous speech recognition using a push stream for audio data.
 */
- (void)recognizeWithPushStream {
    NSBundle *mainBundle = [NSBundle mainBundle];
    NSString *weatherFile = [mainBundle pathForResource: @"whatstheweatherlike" ofType:@"wav"];
    NSLog(@"weatherFile path: %@", weatherFile);
    if (!weatherFile) {
        NSLog(@"Cannot find audio file!");
        [self updateRecognitionErrorText:(@"Cannot find audio file")];
        return;
    }

    NSURL *targetUrl = [NSURL URLWithString:weatherFile];
    NSError *error = nil;

    AVAudioFile *audioFile = [[AVAudioFile alloc] initForReading:targetUrl commonFormat:AVAudioPCMFormatInt16 interleaved:NO error:&error];
    if (error)
    {
        NSLog(@"Error while opening file: %@", error);
        [self updateRecognitionErrorText:(@"Error opening audio file")];
        return;
    }

    // check the format of the file
    NSAssert(1 == audioFile.fileFormat.channelCount, @"Bad channel count");
    NSAssert(16000 == audioFile.fileFormat.sampleRate, @"Unexpected sample rate");

    // set up the stream
    SPXAudioStreamFormat *audioFormat = [[SPXAudioStreamFormat alloc] initUsingPCMWithSampleRate:audioFile.fileFormat.sampleRate bitsPerSample:16 channels:1];
    SPXPushAudioInputStream* stream;
    stream = [[SPXPushAudioInputStream alloc] initWithAudioFormat:audioFormat];

    SPXAudioConfiguration* audioConfig = [[SPXAudioConfiguration alloc] initWithStreamInput:stream];
    if (!audioConfig) {
        NSLog(@"Error creating stream!");
        [self updateRecognitionErrorText:(@"Error creating stream!")];
        return;
    }

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

    // start recognizing
    [self updateRecognitionStatusText:(@"Recognizing from push stream...")];
    [speechRecognizer startContinuousRecognition];

    // set up the buffer fo push data into the stream
    const AVAudioFrameCount nBytesToRead = 5000;
    const NSInteger bytesPerFrame = audioFile.fileFormat.streamDescription->mBytesPerFrame;
    AVAudioPCMBuffer *buffer = [[AVAudioPCMBuffer alloc] initWithPCMFormat:audioFile.fileFormat frameCapacity:nBytesToRead / bytesPerFrame];

    NSAssert(1 == buffer.stride, @"only one channel allowed");
    NSAssert(nil != buffer.int16ChannelData, @"assure correct format");

    // push data to stream
    while (1)
    {
        NSError *bufferError = nil;
        bool success = [audioFile readIntoBuffer:buffer error:&bufferError];
        if (!success) {
            NSLog(@"Read error on stream: %@", bufferError);
            [stream close];
            break;
        }
        else
        {
            NSInteger nBytesRead = [buffer frameLength] * bytesPerFrame;
            if (0 == nBytesRead)
            {
                [stream close];
                break;
            }

            NSLog(@"Read %d bytes from file", (int)nBytesRead);

            NSData *data = [NSData dataWithBytesNoCopy:buffer.int16ChannelData[0] length:nBytesRead freeWhenDone:NO];
            NSLog(@"%d bytes data returned", (int)[data length]);

            [stream write:data];
            NSLog(@"Wrote %d bytes to stream", (int)[data length]);
        }

        [NSThread sleepForTimeInterval:0.05f];
    }

    [speechRecognizer stopContinuousRecognition];
}

/*
 * Performs continuous speech recognition using a pull stream for audio data.
 */
- (void)recognizeWithPullStream {
    NSBundle *mainBundle = [NSBundle mainBundle];
    NSString *weatherFile = [mainBundle pathForResource: @"whatstheweatherlike" ofType:@"wav"];
    NSLog(@"weatherFile path: %@", weatherFile);
    if (!weatherFile) {
        NSLog(@"Cannot find audio file!");
        [self updateRecognitionErrorText:(@"Cannot find audio file")];
        return;
    }

    NSURL *targetUrl = [NSURL URLWithString:weatherFile];
    NSError *error = nil;

    AVAudioFile *audioFile = [[AVAudioFile alloc] initForReading:targetUrl commonFormat:AVAudioPCMFormatInt16 interleaved:NO error:&error];
    if (error)
    {
        NSLog(@"Error while opening file: %@", error);
        [self updateRecognitionErrorText:(@"Error opening audio file")];
        return;
    }

    const NSInteger bytesPerFrame = audioFile.fileFormat.streamDescription->mBytesPerFrame;

    // check the format of the file
    NSAssert(1 == audioFile.fileFormat.channelCount, @"Bad channel count");
    NSAssert(16000 == audioFile.fileFormat.sampleRate, @"Unexpected sample rate");

    // set up the stream with the pull callback
    SPXPullAudioInputStream* stream = [[SPXPullAudioInputStream alloc]
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

    SPXAudioConfiguration* audioConfig = [[SPXAudioConfiguration alloc] initWithStreamInput:stream];
    if (!audioConfig) {
        NSLog(@"Error creating stream!");
        [self updateRecognitionErrorText:(@"Error creating stream!")];
        return;
    }

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

    // session stopped callback to recognize stream has ended
    __block bool end = false;
    [speechRecognizer addSessionStoppedEventHandler: ^ (SPXRecognizer *recognizer, SPXSessionEventArgs *eventArgs) {
        NSLog(@"Received session stopped event. SessionId: %@", eventArgs.sessionId);
        end = true;
    }];

    // start recognizing
    [self updateRecognitionStatusText:(@"Recognizing from pull stream...")];
    [speechRecognizer startContinuousRecognition];

    // wait until a session stopped event has been received
    while (end == false)
        [NSThread sleepForTimeInterval:1.0f];
    [speechRecognizer stopContinuousRecognition];
}

/*
 * Performs speech recognition with auto source language detection
 */
- (void)recognizeWithAutoLanguageDetection {
    NSBundle *mainBundle = [NSBundle mainBundle];
    NSString *weatherFile = [mainBundle pathForResource: @"whatstheweatherlike" ofType:@"wav"];
    NSLog(@"weatherFile path: %@", weatherFile);
    if (!weatherFile) {
        NSLog(@"Cannot find audio file!");
        [self updateRecognitionErrorText:(@"Cannot find audio file")];
        return;
    }

    SPXAudioConfiguration* weatherAudioSource = [[SPXAudioConfiguration alloc] initWithWavFileInput:weatherFile];
    if (!weatherAudioSource) {
        NSLog(@"Loading audio file failed!");
        [self updateRecognitionErrorText:(@"Audio Error")];
        return;
    }

    SPXSpeechConfiguration *speechConfig = [[SPXSpeechConfiguration alloc] initWithSubscription:speechKey region:serviceRegion];
    if (!speechConfig) {
        NSLog(@"Could not load speech config");
        [self updateRecognitionErrorText:(@"Speech Config Error")];
        return;
    }

    NSArray *languages = @[@"zh-CN", @"en-US"];
    SPXAutoDetectSourceLanguageConfiguration* autoDetectSourceLanguageConfig = [[SPXAutoDetectSourceLanguageConfiguration alloc]init:languages];

    [self updateRecognitionStatusText:(@"Recognizing...")];

    SPXSpeechRecognizer* speechRecognizer = [[SPXSpeechRecognizer alloc] initWithSpeechConfiguration:speechConfig
                                                               autoDetectSourceLanguageConfiguration:autoDetectSourceLanguageConfig
                                                                                  audioConfiguration:weatherAudioSource];
    if (!speechRecognizer) {
        NSLog(@"Could not create speech recognizer");
        [self updateRecognitionResultText:(@"Speech Recognition Error")];
        return;
    }

    SPXSpeechRecognitionResult *speechResult = [speechRecognizer recognizeOnce];
    if (SPXResultReason_Canceled == speechResult.reason) {
        SPXCancellationDetails *details = [[SPXCancellationDetails alloc] initFromCanceledRecognitionResult:speechResult];
        NSLog(@"Speech recognition was canceled: %@. Did you pass the correct key/region combination?", details.errorDetails);
        [self updateRecognitionErrorText:([NSString stringWithFormat:@"Canceled: %@", details.errorDetails ])];
    } else if (SPXResultReason_RecognizedSpeech == speechResult.reason) {
        SPXAutoDetectSourceLanguageResult *languageResult = [[SPXAutoDetectSourceLanguageResult alloc] init:speechResult];
        NSLog(@"Speech recognition result received: %@ in language %@", speechResult.text, [languageResult language]);
        NSString *resultText = [NSString stringWithFormat:@"Language: %@, %@", [languageResult language], speechResult.text];
        [self updateRecognitionResultText:(resultText)];
    } else {
        NSLog(@"There was an error.");
        [self updateRecognitionErrorText:(@"Speech Recognition Error")];
    }
}

/*
 * Performs keyword recognition from a wav file using kws.table keyword model
 */
- (void)recognizeKeywordFromFile {
    NSBundle *mainBundle = [NSBundle mainBundle];
    NSString *kwsWeatherFile = [mainBundle pathForResource: @"kws_whatstheweatherlike" ofType:@"wav"];
    NSLog(@"kws_weatherFile path: %@", kwsWeatherFile);
    if (!kwsWeatherFile) {
        NSLog(@"Cannot find audio file!");
        [self updateRecognitionErrorText:(@"Cannot find audio file")];
        return;
    }

    SPXAudioConfiguration* audioFileInput = [[SPXAudioConfiguration alloc] initWithWavFileInput:kwsWeatherFile];
    if (!audioFileInput) {
        NSLog(@"Loading audio file failed!");
        [self updateRecognitionErrorText:(@"Audio Error")];
        return;
    }

    NSString *keywordModelFile = [mainBundle pathForResource: @"kws" ofType:@"table"];
    NSLog(@"keyword model file path: %@", keywordModelFile);
    if (!keywordModelFile) {
        NSLog(@"Cannot find keyword model file!");
        [self updateRecognitionErrorText:(@"Cannot find keyword model file")];
        return;
    }

    SPXKeywordRecognitionModel* keywordRecognitionModel = [[SPXKeywordRecognitionModel alloc] initFromFile:keywordModelFile];

    SPXKeywordRecognizer* keywordRecognizer = [[SPXKeywordRecognizer alloc] init:audioFileInput];
    if (!keywordRecognizer) {
        NSLog(@"Could not create keyword recognizer");
        [self updateRecognitionResultText:(@"Keyword Recognition Error")];
        return;
    }

    dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
    __block SPXKeywordRecognitionResult * keywordResult;
    [keywordRecognizer recognizeOnceAsync: ^ (SPXKeywordRecognitionResult *srresult) {
        keywordResult = srresult;
        dispatch_semaphore_signal(semaphore);
    }keywordModel:keywordRecognitionModel];

    [self updateRecognitionStatusText:(@"Waiting for keyword detected...")];
    dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);

    if (SPXResultReason_Canceled == keywordResult.reason) {
        SPXCancellationDetails *details = [[SPXCancellationDetails alloc] initFromCanceledRecognitionResult:keywordResult];
        NSLog(@"Keyword recognition was canceled: %@.", details.errorDetails);
        [self updateRecognitionErrorText:([NSString stringWithFormat:@"Canceled: %@", details.errorDetails ])];
    } else if (SPXResultReason_RecognizedKeyword == keywordResult.reason) {
        NSLog(@"Keyword recognition result received: %@", keywordResult.text);
        [self updateRecognitionResultText:(keywordResult.text)];
    } else {
        NSLog(@"There was an error.");
        [self updateRecognitionErrorText:(@"Keyword Recognition Error")];
    }
}

/*
 * Performs single-shot pronunciation assessment from microphone.
 */
- (void)pronunciationAssessFromMicrophone {
    SPXSpeechConfiguration *speechConfig = [[SPXSpeechConfiguration alloc] initWithSubscription:speechKey region:serviceRegion];
    if (!speechConfig) {
        NSLog(@"Could not load speech config");
        [self updateRecognitionErrorText:(@"Speech Config Error")];
        return;
    }

    SPXPushAudioInputStream *stream = [[SPXPushAudioInputStream alloc] init];
    self->recorder = [[AudioRecorder alloc]initWithPushStream:stream];
    SPXAudioConfiguration *audioConfig = [[SPXAudioConfiguration alloc]initWithStreamInput:stream];

    SPXSpeechRecognizer* speechRecognizer = [[SPXSpeechRecognizer alloc] initWithSpeechConfiguration:speechConfig language:@"en-US" audioConfiguration:audioConfig];
    if (!speechRecognizer) {
        NSLog(@"Could not create speech recognizer");
        [self updateRecognitionResultText:(@"Speech Recognition Error")];
        return;
    }

    // create pronunciation assessment config, set grading system, granularity and if enable miscue based on your requirement.
    SPXPronunciationAssessmentConfiguration *pronunicationConfig =
    [[SPXPronunciationAssessmentConfiguration alloc] init:pronunciationAssessmentReferenceText
                                            gradingSystem:SPXPronunciationAssessmentGradingSystem_HundredMark
                                              granularity:SPXPronunciationAssessmentGranularity_Phoneme
                                             enableMiscue:true];
    
    [pronunicationConfig enableProsodyAssessment];
    
    [pronunicationConfig applyToRecognizer:speechRecognizer];
    [self updateRecognitionStatusText:(@"Assessing...")];
    [self->recorder record];

    SPXSpeechRecognitionResult *speechResult = [speechRecognizer recognizeOnce];
    if (SPXResultReason_Canceled == speechResult.reason) {
        SPXCancellationDetails *details = [[SPXCancellationDetails alloc] initFromCanceledRecognitionResult:speechResult];
        NSLog(@"Speech recognition was canceled: %@. Did you pass the correct key/region combination?", details.errorDetails);
        [self updateRecognitionErrorText:([NSString stringWithFormat:@"Canceled: %@", details.errorDetails ])];
    } else if (SPXResultReason_RecognizedSpeech == speechResult.reason) {
        NSLog(@"Speech recognition result received: %@", speechResult.text);
        SPXPronunciationAssessmentResult *pronunciationResult = [[SPXPronunciationAssessmentResult alloc]init:speechResult];
        NSString *resultText = [NSString stringWithFormat:@"Assessment finished. \nAccuracy score: %f, Prosody score: %f, Pronunciation score: %f, Completeness Score: %f, Fluency score: %f.", pronunciationResult.accuracyScore, pronunciationResult.prosodyScore,  pronunciationResult.pronunciationScore, pronunciationResult.completenessScore, pronunciationResult.fluencyScore];
        [self updateRecognitionResultText:resultText];
    } else {
        NSLog(@"There was an error.");
        [self updateRecognitionErrorText:(@"Speech Recognition Error")];
    }

    [self->recorder stop];
}

/*
 * Performs continuous pronunciation assessment from file.
 */
- (void)pronunciationAssessFromFile {
    NSBundle *mainBundle = [NSBundle mainBundle];
    NSString *pronFile = [mainBundle pathForResource: @"pronunciation-assessment" ofType:@"wav"];
    NSLog(@"pronFile path: %@", pronFile);
    if (!pronFile) {
        NSLog(@"Cannot find audio file!");
        [self updateRecognitionErrorText:(@"Cannot find audio file")];
        return;
    }

    SPXAudioConfiguration* pronAudioSource = [[SPXAudioConfiguration alloc] initWithWavFileInput:pronFile];
    if (!pronAudioSource) {
        NSLog(@"Loading audio file failed!");
        [self updateRecognitionErrorText:(@"Audio Error")];
        return;
    }

    SPXSpeechConfiguration *speechConfig = [[SPXSpeechConfiguration alloc] initWithSubscription:speechKey region:serviceRegion];
    if (!speechConfig) {
        NSLog(@"Could not load speech config");
        [self updateRecognitionErrorText:(@"Speech Config Error")];
        return;
    }

    SPXSpeechRecognizer* speechRecognizer = [[SPXSpeechRecognizer alloc] initWithSpeechConfiguration:speechConfig language:@"en-US" audioConfiguration:pronAudioSource];
    if (!speechRecognizer) {
        NSLog(@"Could not create speech recognizer");
        [self updateRecognitionResultText:(@"Speech Recognition Error")];
        return;
    }

    // Create pronunciation assessment config, set grading system, granularity and if enable miscue based on your requirement.
    // The audio text is "Hello hello world! Today is a day!"
    SPXPronunciationAssessmentConfiguration *pronunicationConfig =
    [[SPXPronunciationAssessmentConfiguration alloc] init:@"Hello world! Today is a nice day!"
                                            gradingSystem:SPXPronunciationAssessmentGradingSystem_HundredMark
                                              granularity:SPXPronunciationAssessmentGranularity_Phoneme
                                             enableMiscue:true];
    
    [pronunicationConfig enableProsodyAssessment];
    
    [pronunicationConfig applyToRecognizer:speechRecognizer];
    [self updateRecognitionStatusText:(@"Assessing...")];

    // connect callbacks
    __block double sumAccuracy = 0;
    __block double sumProsody = 0;
    __block double sumFluency = 0;
    __block int sumWords = 0;
    __block int countProsody = 0;
    [speechRecognizer addRecognizedEventHandler: ^ (SPXSpeechRecognizer *recognizer, SPXSpeechRecognitionEventArgs *eventArgs) {
        NSLog(@"Received final result event. SessionId: %@, recognition result:%@. Status %ld. offset %llu duration %llu resultid:%@", eventArgs.sessionId, eventArgs.result.text, (long)eventArgs.result.reason, eventArgs.result.offset, eventArgs.result.duration, eventArgs.result.resultId);
        SPXPronunciationAssessmentResult *pronunciationResult = [[SPXPronunciationAssessmentResult alloc]init:eventArgs.result];
        NSString *resultText = [NSString stringWithFormat:@"Received final result event. \nrecognition result: %@, Accuracy score: %.2f, Prosody score: %.2f, Pronunciation score: %.2f, Completeness Score: %.2f, Fluency score: %.2f.", eventArgs.result.text, pronunciationResult.accuracyScore, pronunciationResult.prosodyScore, pronunciationResult.pronunciationScore, pronunciationResult.completenessScore, pronunciationResult.fluencyScore];
        sumProsody += pronunciationResult.prosodyScore;
        countProsody += 1;
        [self updateRecognitionResultText:resultText];
        NSArray *words = [eventArgs.result.text componentsSeparatedByString:@" "];
        NSUInteger wordCount = [words count];
        sumAccuracy += pronunciationResult.accuracyScore * wordCount;
        sumFluency += pronunciationResult.fluencyScore * wordCount;
        sumWords += wordCount;
    }];

    __block bool end = false;
    [speechRecognizer addCanceledEventHandler:^(SPXSpeechRecognizer *recognizer, SPXSpeechRecognitionCanceledEventArgs *eventArgs) {
        SPXCancellationDetails *details = [[SPXCancellationDetails alloc] initFromCanceledRecognitionResult:eventArgs.result];
        NSLog(@"Pronunciation assessment was canceled: %@. Did you pass the correct key/region combination?", details.errorDetails);
        [self updateRecognitionErrorText:([NSString stringWithFormat:@"Canceled: %@", details.errorDetails ])];
        end = true;
    }];

    // session stopped callback to recognize stream has ended
    [speechRecognizer addSessionStoppedEventHandler: ^ (SPXRecognizer *recognizer, SPXSessionEventArgs *eventArgs) {
        NSLog(@"Received session stopped event. SessionId: %@", eventArgs.sessionId);
        end = true;
    }];

    // start recognizing
    [speechRecognizer startContinuousRecognition];

    // wait until a session stopped event has been received
    while (end == false)
        [NSThread sleepForTimeInterval:1.0f];
    [speechRecognizer stopContinuousRecognition];

    if (sumWords > 0) {
        // Overall accuracy and fluency scores are the weighted average of scores of all sentences.
        NSString *resultText = [NSString stringWithFormat:@"Assessment finished. \nOverall accuracy score: %.2f, prosody score: %.2f, fluency score: %.2f.", sumAccuracy / sumWords, sumProsody / countProsody, sumFluency / sumWords];
        [self updateRecognitionResultText:resultText];
    }
}

/*
 * Performs pronunciation assessment from stream.
 */
- (void)pronunciationAssessFromStream {
    [self updateRecognitionResultText:@"Stream Result"];
    // Creates an instance of a speech config with specified subscription key and service region.
    SPXSpeechConfiguration *speechConfig = [[SPXSpeechConfiguration alloc] initWithSubscription:speechKey region:serviceRegion];
    NSBundle *bundle = [NSBundle mainBundle];
    NSString *path = [bundle pathForResource:@"whatstheweatherlike" ofType:@"wav"];
    if (!path) {
        NSLog(@"Cannot find audio file!");
        [self updateRecognitionErrorText:@"Cannot find audio file"];
        return;
    }
    NSLog(@"pronunciation assessment audio file path: %@", path);

    NSData *audioDataWithHeader = [NSData dataWithContentsOfFile:path];
    NSData *audioData = [audioDataWithHeader subdataWithRange:NSMakeRange(46, audioDataWithHeader.length - 46)];

    NSDate *startTime = [NSDate date];

    SPXAudioStreamFormat *audioFormat = [[SPXAudioStreamFormat alloc] initUsingPCMWithSampleRate:16000 bitsPerSample:16 channels:1];
    SPXPushAudioInputStream *audioInputStream = [[SPXPushAudioInputStream alloc] initWithAudioFormat:audioFormat];
    if (!audioInputStream) {
        NSLog(@"Error: Failed to create audio input stream.");
        return;
    }

    SPXAudioConfiguration *audioConfig = [[SPXAudioConfiguration alloc] initWithStreamInput:audioInputStream];
    if (!audioConfig) {
        NSLog(@"Error: audioConfig is Nil");
        return;
    }

    SPXSpeechRecognizer *speechRecognizer = [[SPXSpeechRecognizer alloc] initWithSpeechConfiguration:speechConfig language:@"en-US" audioConfiguration:audioConfig];

    NSString *referenceText = @"what's the weather like";
    SPXPronunciationAssessmentConfiguration *pronAssessmentConfig = [[SPXPronunciationAssessmentConfiguration alloc] init:referenceText gradingSystem:SPXPronunciationAssessmentGradingSystem_HundredMark granularity:SPXPronunciationAssessmentGranularity_Phoneme enableMiscue:true];
    
    [pronAssessmentConfig enableProsodyAssessment];
    
    [pronAssessmentConfig applyToRecognizer:speechRecognizer error:nil];

    [audioInputStream write:audioData];
    [audioInputStream write:[NSData data]];

    [self updateRecognitionResultText:@"Analysising"];

    // Handle the recognition result
    [speechRecognizer recognizeOnceAsync:^(SPXSpeechRecognitionResult * _Nullable result) {
        SPXPronunciationAssessmentResult *pronunciationResult = [[SPXPronunciationAssessmentResult alloc] init:result];
        if (!pronunciationResult) {
            NSLog(@"Error: pronunciationResult is Nil");
            return;
        }
        
        [self updateRecognitionResultText:@"generating result..."];
        NSMutableString *mResult = [[NSMutableString alloc] init];
        NSString *resultText = [NSString stringWithFormat:@"Accuracy score: %.2f, Prosody score: %.2f, Pronunciation score: %.2f, Completeness Score: %.2f, Fluency score: %.2f", pronunciationResult.accuracyScore, pronunciationResult.pronunciationScore, pronunciationResult.prosodyScore, pronunciationResult.completenessScore, pronunciationResult.fluencyScore];
        NSLog(@"%@", resultText);
        [mResult appendString:resultText];
        [mResult appendString:@"\n\nword  accuracyScore   errorType\n"];
        for(SPXWordLevelTimingResult *word in pronunciationResult.words){
            NSString *wordLevel = [NSString stringWithFormat:@"%@   %.2f  %@\n", word.word, word.accuracyScore, word.errorType];
            [mResult appendString:wordLevel];
            NSLog(@"%@", wordLevel);
        }
        NSString *finalResult = [NSString stringWithString:mResult];
        NSLog(@"%@", finalResult);
        [self updateRecognitionResultText:finalResult];
        
        NSDate *endTime = [NSDate date];
        double timeCost = [endTime timeIntervalSinceDate:startTime] * 1000;
        NSLog(@"Time cost: %fms", timeCost);
    }];
}

/*
 * Performs pronunciation assessment configured with json
 */
- (void)pronunciationAssessConfiguredWithJson {
    [self updateRecognitionResultText:@"Result configured with json"];
    // Creates an instance of a speech config with specified subscription key and service region.
    SPXSpeechConfiguration *speechConfig = [[SPXSpeechConfiguration alloc] initWithSubscription:speechKey region:serviceRegion];
    NSBundle *bundle = [NSBundle mainBundle];
    NSString *path = [bundle pathForResource:@"whatstheweatherlike" ofType:@"wav"];
    if (!path) {
        NSLog(@"Cannot find audio file!");
        [self updateRecognitionErrorText:@"Cannot find audio file"];
        return;
    }
    NSLog(@"pronunciation assessment audio file path: %@", path);

    SPXAudioConfiguration *audioConfig = [[SPXAudioConfiguration alloc]initWithWavFileInput:path];

    SPXSpeechRecognizer *speechRecognizer = [[SPXSpeechRecognizer alloc] initWithSpeechConfiguration:speechConfig language:@"en-US" audioConfiguration:audioConfig];

    NSString *referenceText = @"what's the weather like";
    // create pronunciation assessment config, set grading system, granularity and if enable miscue based on your requirement.
    NSString *jsonConfig = @"{\"GradingSystem\":\"HundredMark\",\"Granularity\":\"Phoneme\",\"EnableMiscue\":true,\"ScenarioId\":\"[scenario ID will be assigned by product team]\"}";
    SPXPronunciationAssessmentConfiguration *pronAssessmentConfig = [[SPXPronunciationAssessmentConfiguration alloc] initWithJson:jsonConfig error:nil];
    pronAssessmentConfig.referenceText = referenceText;
    
    [pronAssessmentConfig enableProsodyAssessment];
    
    [pronAssessmentConfig applyToRecognizer:speechRecognizer error:nil];

    [self updateRecognitionResultText:@"Analysising"];

    // Handle the recognition result
    [speechRecognizer recognizeOnceAsync:^(SPXSpeechRecognitionResult * _Nullable result) {
        SPXPronunciationAssessmentResult *pronunciationResult = [[SPXPronunciationAssessmentResult alloc] init:result];
        if (!pronunciationResult) {
            NSLog(@"Error: pronunciationResult is Nil");
            return;
        }
        
        [self updateRecognitionResultText:@"generating result..."];
        NSMutableString *mResult = [[NSMutableString alloc] init];
        NSString *resultText = [NSString stringWithFormat:@"Accuracy score: %.2f, Prosody score: %.2f, Pronunciation score: %.2f, Completeness Score: %.2f, Fluency score: %.2f", pronunciationResult.accuracyScore,  pronunciationResult.prosodyScore, pronunciationResult.pronunciationScore, pronunciationResult.completenessScore, pronunciationResult.fluencyScore];
        NSLog(@"%@", resultText);
        [mResult appendString:resultText];
        [mResult appendString:@"\n\nword  accuracyScore   errorType\n"];
        for(SPXWordLevelTimingResult *word in pronunciationResult.words){
            NSString *wordLevel = [NSString stringWithFormat:@"%@   %.2f  %@\n", word.word, word.accuracyScore, word.errorType];
            [mResult appendString:wordLevel];
            NSLog(@"%@", wordLevel);
        }
        NSString *finalResult = [NSString stringWithString:mResult];
        NSLog(@"%@", finalResult);
        [self updateRecognitionResultText:finalResult];
    }];
}

/*
 * Performs pronunciation assessment with content assessment.
 */
- (void)pronunciationAssessWithContentAssessment {
    NSBundle *mainBundle = [NSBundle mainBundle];
    NSString *pronFile = [mainBundle pathForResource: @"pronunciation_assessment_fall" ofType:@"wav"];
    NSLog(@"pronFile path: %@", pronFile);
    if (!pronFile) {
        NSLog(@"Cannot find audio file!");
        [self updateRecognitionErrorText:(@"Cannot find audio file")];
        return;
    }

    SPXAudioConfiguration* pronAudioSource = [[SPXAudioConfiguration alloc] initWithWavFileInput:pronFile];
    if (!pronAudioSource) {
        NSLog(@"Loading audio file failed!");
        [self updateRecognitionErrorText:(@"Audio Error")];
        return;
    }

    SPXSpeechConfiguration *speechConfig = [[SPXSpeechConfiguration alloc] initWithSubscription:speechKey region:serviceRegion];
    if (!speechConfig) {
        NSLog(@"Could not load speech config");
        [self updateRecognitionErrorText:(@"Speech Config Error")];
        return;
    }

    SPXSpeechRecognizer* speechRecognizer = [[SPXSpeechRecognizer alloc] initWithSpeechConfiguration:speechConfig language:@"en-US" audioConfiguration:pronAudioSource];
    if (!speechRecognizer) {
        NSLog(@"Could not create speech recognizer");
        [self updateRecognitionResultText:(@"Speech Recognition Error")];
        return;
    }

    // Create pronunciation assessment config, set grading system, granularity
    SPXPronunciationAssessmentConfiguration *pronunicationConfig =
    [[SPXPronunciationAssessmentConfiguration alloc] init:@""
                                            gradingSystem:SPXPronunciationAssessmentGradingSystem_HundredMark
                                              granularity:SPXPronunciationAssessmentGranularity_Phoneme
                                             enableMiscue:false];
    
    NSString *theTopic = @"the season of the fall";
    [pronunicationConfig enableProsodyAssessment];
    [pronunicationConfig enableContentAssessmentWithTopic:theTopic];
    
    [pronunicationConfig applyToRecognizer:speechRecognizer];
    [self updateRecognitionStatusText:(@"Assessing...")];

    // Connect callbacks
    NSMutableArray *recognizedStrings = [NSMutableArray array];
    NSMutableArray *pronContents = [NSMutableArray array];
    __block bool end = false;

    [speechRecognizer addRecognizedEventHandler: ^ (SPXSpeechRecognizer *recognizer, SPXSpeechRecognitionEventArgs *eventArgs) {
        NSLog(@"Received final result event. SessionId: %@, recognition result:%@. Status %ld. offset %llu duration %llu resultid:%@", eventArgs.sessionId, eventArgs.result.text, (long)eventArgs.result.reason, eventArgs.result.offset, eventArgs.result.duration, eventArgs.result.resultId);
        
        if([eventArgs.result.text length] != 0 && ![eventArgs.result.text isEqualToString:@"."]){
            [recognizedStrings addObject:eventArgs.result.text];
        }
        
        SPXPronunciationAssessmentResult *pronunciationResult = [[SPXPronunciationAssessmentResult alloc]init:eventArgs.result];
        [pronContents addObject:pronunciationResult.contentAssessmentResult];
    }];

    [speechRecognizer addCanceledEventHandler:^(SPXSpeechRecognizer *recognizer, SPXSpeechRecognitionCanceledEventArgs *eventArgs) {
        SPXCancellationDetails *details = [[SPXCancellationDetails alloc] initFromCanceledRecognitionResult:eventArgs.result];
        NSLog(@"Pronunciation assessment was canceled: %@. Did you pass the correct key/region combination?", details.errorDetails);
        [self updateRecognitionErrorText:([NSString stringWithFormat:@"Canceled: %@", details.errorDetails ])];
        end = true;
    }];

    // Session stopped callback to recognize stream has ended
    [speechRecognizer addSessionStoppedEventHandler: ^ (SPXRecognizer *recognizer, SPXSessionEventArgs *eventArgs) {
        NSLog(@"Received session stopped event. SessionId: %@", eventArgs.sessionId);
        end = true;
    }];

    // Start recognizing
    [speechRecognizer startContinuousRecognition];

    // Wait until a session stopped event has been received
    while (end == false)
        [NSThread sleepForTimeInterval:1.0f];
    [speechRecognizer stopContinuousRecognition];

    NSString *resultText = @"";
    if ([recognizedStrings count] > 0) {
        
        // Un-comment out below codes if want to show the recognized text.
        // For the phone screen, the text is a bit long.
        // resultText = [resultText stringByAppendingString:@"Content Assessment for: "];
        // NSString *recognizedText = [recognizedStrings componentsJoinedByString:@" "];
        // resultText = [resultText stringByAppendingString:recognizedText];
        
        if ([pronContents count] > 0) {
            SPXContentAssessmentResult* pronContent = [pronContents lastObject];
            NSString *pronResult = [NSString stringWithFormat:@"Assessment Result: grammar score: %.2f, vocabulary  score: %.2f, topic score: %.2f.", pronContent.grammarScore, pronContent.vocabularyScore, pronContent.topicScore];
            resultText = [resultText stringByAppendingString:pronResult];
        }
        else {
            resultText = [resultText stringByAppendingString:@"The content result is empty"];
        }
    }
    else {
        resultText = [resultText stringByAppendingString:@"The recognized Text is empty"];
    }
    [self updateRecognitionResultText:resultText];
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

