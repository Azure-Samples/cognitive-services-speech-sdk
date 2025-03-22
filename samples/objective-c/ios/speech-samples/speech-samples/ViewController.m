//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "ViewController.h"
#import "AudioRecorder.h"
#import <AVFoundation/AVFoundation.h>
#import <MicrosoftCognitiveServicesSpeech/SPXSpeechApi.h>
#import <Foundation/Foundation.h>

@interface ViewController () {
    NSString *speechKey;
    NSString *serviceRegion;
    NSString *pronunciationAssessmentReferenceText;
    AudioRecorder *recorder;
}

NSString *GetChatCompletionWithResourceName(NSString *oaiResourceName,
                                             NSString *oaiDeploymentName,
                                             NSString *oaiApiVersion,
                                             NSString *oaiApiKey,
                                             NSString *sendText);

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
@property (strong, nonatomic) IBOutlet UIButton *recognizeFromFileWithLoggingButton;

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
- (IBAction)recognizeFromFileWithLoggingButtonTapped:(UIButton *)sender;
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
    
    self.recognizeFromFileWithLoggingButton = [UIButton buttonWithType:UIButtonTypeSystem];
    [self.recognizeFromFileWithLoggingButton addTarget:self action:@selector(recognizeFromFileWithLoggingButtonTapped:) forControlEvents:UIControlEventTouchUpInside];
    [self.recognizeFromFileWithLoggingButton setTitle:@"Start rec from file with logging" forState:UIControlStateNormal];
    [self.recognizeFromFileWithLoggingButton setFrame:CGRectMake(50.0, 510.0, 300.0, 40.0)];
    self.recognizeFromFileWithLoggingButton.accessibilityIdentifier = @"recognize_file_with_logging_button";
    [self.view addSubview:self.recognizeFromFileWithLoggingButton];

    self.recognitionResultLabel = [[UILabel alloc] initWithFrame:CGRectMake(50.0, 540.0, 300.0, 300.0)];
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

- (IBAction)recognizeFromFileWithLoggingButtonTapped:(UIButton *)sender {
    dispatch_async(dispatch_get_global_queue(QOS_CLASS_DEFAULT, 0), ^{
        [self recognizeFromFileWithLogging];
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
    
    [speechRecognizer addSessionStartedEventHandler: ^ (SPXRecognizer *sender, SPXSessionEventArgs *eventArgs) {
        NSLog(@"SESSION ID: %@", eventArgs.sessionId);
    }];
    
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
    
    [speechRecognizer addSessionStartedEventHandler: ^ (SPXRecognizer *sender, SPXSessionEventArgs *eventArgs) {
        NSLog(@"SESSION ID: %@", eventArgs.sessionId);
    }];
    
    [pronunicationConfig applyToRecognizer:speechRecognizer];
    [self updateRecognitionStatusText:(@"Assessing...")];

    // connect callbacks
    __block double sumProsody = 0;
    __block int sumWords = 0;
    __block int countProsody = 0;
    NSMutableArray *recognizedWords = [NSMutableArray array];
    
    [speechRecognizer addRecognizedEventHandler: ^ (SPXSpeechRecognizer *recognizer, SPXSpeechRecognitionEventArgs *eventArgs) {
        NSLog(@"Received final result event. SessionId: %@, recognition result:%@. Status %ld. offset %llu duration %llu resultid:%@", eventArgs.sessionId, eventArgs.result.text, (long)eventArgs.result.reason, eventArgs.result.offset, eventArgs.result.duration, eventArgs.result.resultId);
        SPXPronunciationAssessmentResult *pronunciationResult = [[SPXPronunciationAssessmentResult alloc]init:eventArgs.result];
        NSString *resultText = [NSString stringWithFormat:@"Received final result event. \nrecognition result: %@, Accuracy score: %.2f, Prosody score: %.2f, Pronunciation score: %.2f, Completeness Score: %.2f, Fluency score: %.2f.", eventArgs.result.text, pronunciationResult.accuracyScore, pronunciationResult.prosodyScore, pronunciationResult.pronunciationScore, pronunciationResult.completenessScore, pronunciationResult.fluencyScore];
        sumProsody += pronunciationResult.prosodyScore;
        countProsody += 1;
        [self updateRecognitionResultText:resultText];
        NSArray *words = [eventArgs.result.text componentsSeparatedByString:@" "];
        NSUInteger wordCount = [words count];
        sumWords += wordCount;
        
        [recognizedWords addObjectsFromArray:pronunciationResult.words];
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
        // Accuracy score
        double totalAccurayScore = 0;
        int accuracyCount = 0;
        int validCount = 0;
        double durationSum = 0.0;
        
        for (SPXWordLevelTimingResult *word in recognizedWords) {
            if (![word.errorType isEqualToString:@"Insertion"]) {
                totalAccurayScore += word.accuracyScore;
                accuracyCount += 1;
            }
            
            if ([word.errorType isEqualToString:@"None"]) {
                durationSum += word.duration + 0.01;
                validCount += 1;
            }
            
        }
        double accurayScore = (accuracyCount > 0) ? (totalAccurayScore) / accuracyCount : NAN;
        
        // Fluency score
        SPXWordLevelTimingResult *firstWord = [recognizedWords firstObject];
        double startOffset = firstWord.offset;

        SPXWordLevelTimingResult *lastWord = [recognizedWords lastObject];
        double endOffset = lastWord.offset + lastWord.duration + 0.01;

        double fluencyScore = durationSum / (endOffset - startOffset) * 100.0;
        
        // Completeness score
        double completenessScore = (double)validCount / (double)accuracyCount * 100.0;
        if (completenessScore > 100) {
            completenessScore = 100;
        }
        
        // Prosody score
        double prosodyScore = sumProsody / countProsody;
        
        double minScore = MIN(accurayScore, MIN(prosodyScore, MIN(completenessScore, fluencyScore)));
        
        // Pronunciation score
        double pronunciationScore = 0.2 * (accurayScore + prosodyScore + completenessScore + fluencyScore) + 0.2 * minScore;
        
        // Overall scores.
        NSString *resultText = [NSString stringWithFormat:@"Assessment finished. \nOverall accuracy score: %.2f, prosody score: %.2f, fluency score: %.2f, completeness score: %.2f, pronunciation score: %.2f", accurayScore, prosodyScore, fluencyScore, completenessScore, pronunciationScore];
        [self updateRecognitionResultText:resultText];
        
        for (NSInteger idx = 0; idx < recognizedWords.count; idx++) {
            SPXWordLevelTimingResult *word = recognizedWords[idx];
            NSLog(@"    %ld: word: %@\taccuracy score: %.2f\terror type: %@",
                  (long)(idx + 1), word.word, word.accuracyScore, word.errorType);
        }
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
    
    dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
    [speechRecognizer addSessionStartedEventHandler: ^ (SPXRecognizer *sender, SPXSessionEventArgs *eventArgs) {
        NSLog(@"SESSION ID: %@", eventArgs.sessionId);
    }];
    
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
        dispatch_semaphore_signal(semaphore);
    }];
    dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
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
    
    dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
    [speechRecognizer addSessionStartedEventHandler: ^ (SPXRecognizer *sender, SPXSessionEventArgs *eventArgs) {
        NSLog(@"SESSION ID: %@", eventArgs.sessionId);
    }];
    
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
        dispatch_semaphore_signal(semaphore);
    }];
    dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
}

NSString *GetChatCompletionWithResourceName(NSString *oaiResourceName,
                                             NSString *oaiDeploymentName,
                                             NSString *oaiApiVersion,
                                             NSString *oaiApiKey,
                                             NSString *sendText) {
    // Set up the URL string
    NSString *urlString = [NSString stringWithFormat:@"https://%@.openai.azure.com/openai/deployments/%@/chat/completions?api-version=%@", oaiResourceName, oaiDeploymentName, oaiApiVersion];
    NSURL *url = [NSURL URLWithString:urlString];
    
    // Create a mutable request
    NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:url];
    [request setHTTPMethod:@"POST"];
    [request setValue:@"application/json" forHTTPHeaderField:@"Content-Type"];
    
    // Set the request header with the API key
    [request setValue:oaiApiKey forHTTPHeaderField:@"api-key"];
    
    // Declare the sentences as variables
    NSString *sampleSentence1 = @"OK the movie i like to talk about is the cove it is very say phenomenal sensational documentary about adopting hunting practices in japan i think the director is called well i think the name escapes me anyway but well let's talk about the movie basically it's about dolphin hunting practices in japan there's a small village where where villagers fisherman Q almost twenty thousand dolphins on a yearly basis which is brutal and just explain massacre this book has influenced me a lot i still remember the first time i saw this movie i think it was in middle school one of my teachers showed it to all the class or the class and i remember we were going through some really boring topics like animal protection at that time it was really boring to me but right before everyone was going to just sleep in the class the teacher decided to put the textbook down and show us a clear from this document documentary we were shocked speechless to see the has of the dolphins chopped off and left on the beach and the C turning bloody red with their blood which is i felt sick i couldn't need fish for a whole week and it was lasting impression if not scarring impression and i think this movie is still very meaningful and it despite me a lot especially on wildlife protection dolphins or search beautiful intelligent animals of the sea and why do villagers and fishermen in japan killed it i assume there was a great benefit to its skin or some scientific research but the ironic thing is that they only kill them for the meat because the meat taste great that sickens me for awhile and i think the book inspired me to do a lot of different to do a lot of things about well i protection i follow news like";
    NSString *sampleSentence2 = @"yes i can speak how to this movie is it is worth young wolf young man this is this movie from korea it's a crime movies the movies on the movies speaker speaker or words of young man love hello a cow are you saying they end so i have to go to the go to the america or ha ha ha lots of years a go on the woman the woman is very old he talk to korea he carpool i want to go to the this way this whole home house this house is a is hey so what's your man and at the end the girl cause so there's a woman open open hum finally finds other wolf so what's your young man so the young man don't so yeah man the young man remember he said here's a woman also so am i it's very it's very very sad she is she is a crack credit thank you ";
    NSString *sampleSentence3 = @"yes i want i want to talk about the TV series are enjoying watching a discount name is a friends and it's uh accommodate in the third decades decades an it come out the third decades and its main characters about a six friends live in the NYC but i watched it a long time ago i can't remember the name of them and the story is about what they are happening in their in their life and there are many things treating them and how the friendship are hard friendship and how the french how the strong strongly friendship they obtain them and they always have some funny things happen they only have happened something funny things and is a comedy so that was uh so many and i like this be cause of first adult cause it has a funding it has a farming serious and it can improve my english english words and on the other hand it can i can know about a lot of cultures about the united states and i i first hear about death TV series it's come out of a website and i took into and i watch it after my after my finish my studies and when i was a bad mood when i when i'm in a bad mood or i ";
    
    NSString *topic = @"the season of the fall";
    
    NSDictionary *message1 = @{
        @"role": @"system",
        @"content": @"You are an English teacher and please help to grade a student's essay from vocabulary and grammar and topic relevance on how well the essay aligns with the title, and output format as: {\"vocabulary\": *.*(0-100), \"grammar\": *.*(0-100), \"topic\": *.*(0-100)}."
    };

    NSDictionary *message2 = @{
        @"role": @"user",
        @"content": [NSString stringWithFormat:@"Example1: this essay: \"%@\" has vocabulary and grammar scores of 8 and 8, respectively. Example2: this essay: \"%@\" has vocabulary and grammar scores of 4 and 4.3, respectively. Example3: this essay: \"%@\" has vocabulary and grammar scores of 5 and 5, respectively. The essay for you to score is \"%@\", and the title is \"%@\". The script is from speech recognition so that please first add punctuations when needed, remove duplicates and unnecessary un uh from oral speech, then find all the misuse of words and grammar errors in this essay, find advanced words and grammar usages, and finally give scores based on this information. Please only response as this format {\"vocabulary\": *.*(0-100), \"grammar\": *.*(0-100)}, \"topic\": *.*(0-100)}.", sampleSentence1, sampleSentence2, sampleSentence3, sendText, topic]
    };

    NSArray *messages = @[message1, message2];
    NSDictionary *jsonBody = @{
        @"messages": messages,
        @"temperature": @0,
        @"top_p": @1
    };
    
    // Serialize the body data to JSON
    NSData *bodyData = [NSJSONSerialization dataWithJSONObject:jsonBody options:0 error:nil];
    [request setHTTPBody:bodyData];
    
    // Initialize semaphore for synchronization (blocking behavior)
    dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
    __block NSString *resultString = nil;
    
    // Perform the asynchronous request using NSURLSession
    NSURLSessionDataTask *dataTask = [[NSURLSession sharedSession] dataTaskWithRequest:request
                                                                    completionHandler:^(NSData *data, NSURLResponse *response, NSError *error) {
        if (error) {
            // Log error if the request fails
            NSLog(@"Request failed: %@", error.localizedDescription);
            dispatch_semaphore_signal(semaphore); // Signal semaphore to unblock the main thread
            return;
        }
        
        // Parse the response data to JSON
        NSError *jsonError = nil;
        NSDictionary *jsonResponse = [NSJSONSerialization JSONObjectWithData:data options:0 error:&jsonError];
        if (jsonError) {
            // Log JSON parsing error
            NSLog(@"JSON parsing failed: %@", jsonError.localizedDescription);
            dispatch_semaphore_signal(semaphore); // Signal semaphore to unblock the main thread
            return;
        }
        
        // Extract the 'choices' array from the JSON response
        NSArray *choices = jsonResponse[@"choices"];
        if (choices.count > 0) {
            NSDictionary *firstChoice = choices[0];
            NSDictionary *message = firstChoice[@"message"];
            resultString = message[@"content"];
        }
        
        // Signal the semaphore indicating the request has finished
        dispatch_semaphore_signal(semaphore);
    }];
    
    // Start the network request
    [dataTask resume];
    
    // Wait for the semaphore to be signaled (blocking the main thread until the request completes)
    dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
    
    // Return the result
    return resultString;
}

/*
 * Performs pronunciation assessment with content assessment.
 */
- (void)pronunciationAssessWithContentAssessment {
    NSString *oaiResourceName = @"YourAoaiResourceName";
    NSString *oaiDeploymentName = @"YourAoaiDeploymentName";
    NSString *oaiApiVersion = @"YourAoaiApiVersion";
    NSString *oaiApiKey = @"YourAoaiApiKey";

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
    
    [speechRecognizer addSessionStartedEventHandler: ^ (SPXRecognizer *sender, SPXSessionEventArgs *eventArgs) {
        NSLog(@"SESSION ID: %@", eventArgs.sessionId);
    }];

    // Create pronunciation assessment config, set grading system, granularity
    SPXPronunciationAssessmentConfiguration *pronunicationConfig =
    [[SPXPronunciationAssessmentConfiguration alloc] init:@""
                                            gradingSystem:SPXPronunciationAssessmentGradingSystem_HundredMark
                                              granularity:SPXPronunciationAssessmentGranularity_Phoneme
                                             enableMiscue:false];
    
    [pronunicationConfig enableProsodyAssessment];
    
    [pronunicationConfig applyToRecognizer:speechRecognizer];
    [self updateRecognitionStatusText:(@"Assessing...")];

    // Connect callbacks
    NSMutableArray *recognizedStrings = [NSMutableArray array];
    __block bool end = false;

    [speechRecognizer addRecognizedEventHandler: ^ (SPXSpeechRecognizer *recognizer, SPXSpeechRecognitionEventArgs *eventArgs) {
        NSLog(@"Received final result event. SessionId: %@, recognition result:%@. Status %ld. offset %llu duration %llu resultid:%@", eventArgs.sessionId, eventArgs.result.text, (long)eventArgs.result.reason, eventArgs.result.offset, eventArgs.result.duration, eventArgs.result.resultId);
        
        if([eventArgs.result.text length] != 0 && ![eventArgs.result.text isEqualToString:@"."]){
            [recognizedStrings addObject:eventArgs.result.text];
        }
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
        NSString *connectedString = [recognizedStrings componentsJoinedByString:@" "];
        NSString *result = GetChatCompletionWithResourceName(oaiResourceName, oaiDeploymentName, oaiApiVersion, oaiApiKey, connectedString);
        
        NSError *jsonError;
        NSData *data = [result dataUsingEncoding:NSUTF8StringEncoding];
        NSDictionary *jsonDict = [NSJSONSerialization JSONObjectWithData:data options:0 error:&jsonError];

        if (jsonError) {
            NSLog(@"JSON Parsing Error: %@", jsonError.localizedDescription);
        } else {
            NSNumber *vocabulary = jsonDict[@"vocabulary"];
            NSNumber *grammar = jsonDict[@"grammar"];
            NSNumber *topic = jsonDict[@"topic"];
            
            NSString *pronResult = [NSString stringWithFormat:@"Assessment Result: grammar score: %@, vocabulary  score: %@, topic score: %@.", grammar, vocabulary, topic];
            resultText = [resultText stringByAppendingString:pronResult];
        }
    }
    else {
        resultText = [resultText stringByAppendingString:@"The recognized Text is empty"];
    }
    [self updateRecognitionResultText:resultText];
}

/*
 * Performs speech recognition from a RIFF wav file with logging.
 */
- (void)recognizeFromFileWithLogging {
    BOOL enableFilter = true;
    [self recognizeFromFileWithFileLogging:enableFilter];

    enableFilter = false;
    [self recognizeFromFileWithMemoryLogging:enableFilter];

    enableFilter = true;
    [self recognizeFromFileWithEventLogging:enableFilter];
}

/*
 * Performs speech recognition from a RIFF wav file with file logging.
 */
- (void)recognizeFromFileWithFileLogging:(bool)enableFilter {
    // Setup logging
    NSString *logFileName = @"speech_sdk.log";
    NSString *filteredLogFileName = @"speech_sdk_filtered.log";

    if (enableFilter) {
        NSArray *filters = @[@"SPX_DBG_TRACE_SCOPE_ENTER", @"SPX_DBG_TRACE_SCOPE_EXIT"];
        [SPXFileLogger setFilters:filters];
        logFileName = filteredLogFileName;
    }

    NSString *logFile = [[NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) firstObject]
                         stringByAppendingPathComponent:logFileName];
    if (!logFile) {
        NSLog(@"Get logfile path failed");
        return;
    }

    // Start file logging
    NSLog(@"Start logging to file %@", logFile);
    [SPXFileLogger start:logFile];
    
    // Perform speech recognition
    [self recognizeFromFile];

    // Stop file logging
    NSLog(@"Stop logging to file");
    [SPXFileLogger stop];

    // Clear filters
    if (enableFilter) {
        [SPXFileLogger setFilters:nil];
    }
}

/*
 * Performs speech recognition from a RIFF wav file with memory logging.
 */
- (void)recognizeFromFileWithMemoryLogging:(bool)enableFilter {
    // Setup logging
    NSString *logFileName = @"speech_sdk_memory.log";
    NSString *filteredLogFileName = @"speech_sdk_memory_filtered.log";

    if (enableFilter) {
        NSArray *filters = @[@"audio_stream_session", @"recognizer"];
        [SPXMemoryLogger setFilters:filters];
        logFileName = filteredLogFileName;
    }

    NSString *logFile = [[NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) firstObject]
                         stringByAppendingPathComponent:logFileName];
    if (!logFile) {
        NSLog(@"Get logfile path failed");
        return;
    }

    // Start memory logging
    NSLog(@"Start memory logging");
    [SPXMemoryLogger start];
    
    // Perform speech recognition
    [self recognizeFromFile];

    // Stop memory logging
    NSLog(@"Stop memory logging");
    [SPXMemoryLogger stop];

    // Clear filters
    if (enableFilter) {
        [SPXMemoryLogger setFilters:nil];
    }

    // Dump memory log to a file
    [SPXMemoryLogger dumpToFile:logFile];
}

/*
 * Performs speech recognition from a RIFF wav file with event logging.
 */
- (void)recognizeFromFileWithEventLogging:(bool)enableFilter {
    // Setup logging
    if (enableFilter) {
        NSArray *filters = @[@"usp_reco_engine_adapter", @"web_socket"];
        [SPXEventLogger setFilters:filters];
    }

    // Start event logging
    NSLog(@"Start event logging");
    __block NSMutableArray *eventMsgs = [NSMutableArray array];
    __block int eventCount = 0;

    [SPXEventLogger setCallback:^(NSString *message) {
        @synchronized(self) {
            eventCount++;
            [eventMsgs addObject:message];
        }
    }];
    
    // Perform speech recognition
    [self recognizeFromFile];

    // Stop event logging
    NSLog(@"Stop event logging");
    [SPXEventLogger setCallback:nil];

    // Clear filters
    if (enableFilter) {
        [SPXEventLogger setFilters:nil];
    }

    // Dump event log
    NSString *eventMsg = [eventMsgs componentsJoinedByString:@""];
    NSLog(@"Event messages:");
    NSLog(@"%@", eventMsg);
    NSLog(@"Event message count: %d", eventCount);
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

