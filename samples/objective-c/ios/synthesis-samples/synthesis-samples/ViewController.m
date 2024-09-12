//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "ViewController.h"
#import <MicrosoftCognitiveServicesSpeech/SPXSpeechApi.h>

@interface ViewController () {
    NSString *speechKey;
    NSString *serviceRegion;

    NSString *inputText;
    SPXSpeechSynthesizer *synthesizerForEvents;
}

@property (strong, nonatomic) IBOutlet UITextField *inputField;

@property (strong, nonatomic) IBOutlet UIButton *synthesisDefaultButton;
@property (strong, nonatomic) IBOutlet UIButton *synthesisToFileButton;
@property (strong, nonatomic) IBOutlet UIButton *synthesisToPullAudioOutputStreamButton;
@property (strong, nonatomic) IBOutlet UIButton *synthesisToPushAudioOutputStreamButton;
@property (strong, nonatomic) IBOutlet UIButton *synthesisToAudioDataStreamButton;
@property (strong, nonatomic) IBOutlet UIButton *synthesisEventButton;

@property (strong, nonatomic) IBOutlet UILabel *resultLabel;

- (IBAction)synthesisDefaultButtonTapped:(UIButton *)sender;
- (IBAction)synthesisToFileButtonTapped:(UIButton *)sender;
- (IBAction)synthesisToPullAudioOutputStreamButtonTapped:(UIButton *)sender;
- (IBAction)synthesisToPushAudioOutputStreamButtonTapped:(UIButton *)sender;
- (IBAction)synthesisToAudioDataStreamButtonTapped:(UIButton *)sender;
- (IBAction)synthesisEventButtonTapped:(UIButton *)sender;
@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    speechKey = @"YourSubscriptionKey";
    serviceRegion = @"YourServiceRegion";

    [self.view setBackgroundColor:[UIColor whiteColor]];

    self.inputField = [[UITextField alloc] initWithFrame:CGRectMake(50.0, 80.0, 300.0, 50.0)];
    self.inputField.placeholder = @"Input something to synthesize...";
    self.inputField.borderStyle = UITextBorderStyleRoundedRect;
    self.inputField.delegate = self;
    self.inputField.accessibilityIdentifier = @"input_text_field";
    [self.view addSubview:self.inputField];

    self.synthesisDefaultButton = [UIButton buttonWithType:UIButtonTypeSystem];
    [self.synthesisDefaultButton addTarget:self action:@selector(synthesisDefaultButtonTapped:) forControlEvents:UIControlEventTouchUpInside];
    [self.synthesisDefaultButton setTitle:@"Start synth to speaker" forState:UIControlStateNormal];
    [self.synthesisDefaultButton setFrame:CGRectMake(50.0, 150.0, 300.0, 50.0)];
    self.synthesisDefaultButton.accessibilityIdentifier = @"synthesis_default_button";
    [self.view addSubview:self.synthesisDefaultButton];

    self.synthesisToFileButton = [UIButton buttonWithType:UIButtonTypeSystem];
    [self.synthesisToFileButton addTarget:self action:@selector(synthesisToFileButtonTapped:) forControlEvents:UIControlEventTouchUpInside];
    [self.synthesisToFileButton setTitle:@"Start synth to file" forState:UIControlStateNormal];
    [self.synthesisToFileButton setFrame:CGRectMake(50.0, 200.0, 300.0, 50.0)];
    self.synthesisToFileButton.accessibilityIdentifier = @"synthesis_file_button";
    [self.view addSubview:self.synthesisToFileButton];

    self.synthesisToPullAudioOutputStreamButton = [UIButton buttonWithType:UIButtonTypeSystem];
    [self.synthesisToPullAudioOutputStreamButton addTarget:self action:@selector(synthesisToPullAudioOutputStreamButtonTapped:) forControlEvents:UIControlEventTouchUpInside];
    [self.synthesisToPullAudioOutputStreamButton setTitle:@"Start synth to file with pull stream" forState:UIControlStateNormal];
    [self.synthesisToPullAudioOutputStreamButton setFrame:CGRectMake(50.0, 250.0, 300.0, 50.0)];
    self.synthesisToPullAudioOutputStreamButton.accessibilityIdentifier = @"synthesis_pull_stream_button";
    [self.view addSubview:self.synthesisToPullAudioOutputStreamButton];

    self.synthesisToPushAudioOutputStreamButton = [UIButton buttonWithType:UIButtonTypeSystem];
    [self.synthesisToPushAudioOutputStreamButton addTarget:self action:@selector(synthesisToPushAudioOutputStreamButtonTapped:) forControlEvents:UIControlEventTouchUpInside];
    [self.synthesisToPushAudioOutputStreamButton setTitle:@"Start synth to file with push stream" forState:UIControlStateNormal];
    [self.synthesisToPushAudioOutputStreamButton setFrame:CGRectMake(50.0, 300.0, 300.0, 50.0)];
    self.synthesisToPushAudioOutputStreamButton.accessibilityIdentifier = @"synthesis_push_stream_button";
    [self.view addSubview:self.synthesisToPushAudioOutputStreamButton];

    self.synthesisToAudioDataStreamButton = [UIButton buttonWithType:UIButtonTypeSystem];
    [self.synthesisToAudioDataStreamButton addTarget:self action:@selector(synthesisToAudioDataStreamButtonTapped:) forControlEvents:UIControlEventTouchUpInside];
    [self.synthesisToAudioDataStreamButton setTitle:@"Start synth to file with audio data stream" forState:UIControlStateNormal];
    [self.synthesisToAudioDataStreamButton setFrame:CGRectMake(50.0, 350.0, 300.0, 50.0)];
    self.synthesisToAudioDataStreamButton.accessibilityIdentifier = @"synthesis_audio_data_stream_button";
    [self.view addSubview:self.synthesisToAudioDataStreamButton];

    self.synthesisEventButton = [UIButton buttonWithType:UIButtonTypeSystem];
    [self.synthesisEventButton addTarget:self action:@selector(synthesisEventButtonTapped:) forControlEvents:UIControlEventTouchUpInside];
    [self.synthesisEventButton setTitle:@"Start synth with events" forState:UIControlStateNormal];
    [self.synthesisEventButton setFrame:CGRectMake(50.0, 400.0, 300.0, 50.0)];
    self.synthesisEventButton.accessibilityIdentifier = @"synthesis_event_button";
    [self.view addSubview:self.synthesisEventButton];

    self.resultLabel = [[UILabel alloc] initWithFrame:CGRectMake(50.0, 400.0, 300.0, 200.0)];
    self.resultLabel.lineBreakMode = NSLineBreakByWordWrapping;
    self.resultLabel.numberOfLines = 0;
    self.resultLabel.accessibilityIdentifier = @"result_label";
    [self.view addSubview:self.resultLabel];
}

- (IBAction)synthesisDefaultButtonTapped:(UIButton *)sender {
    dispatch_async(dispatch_get_global_queue(QOS_CLASS_DEFAULT, 0), ^{
        [self synthesisToSpeaker];
    });
}

- (IBAction)synthesisToFileButtonTapped:(UIButton *)sender {
    dispatch_async(dispatch_get_global_queue(QOS_CLASS_DEFAULT, 0), ^{
        [self synthesisToFile];
    });
}

- (IBAction)synthesisToPullAudioOutputStreamButtonTapped:(UIButton *)sender {
    dispatch_async(dispatch_get_global_queue(QOS_CLASS_DEFAULT, 0), ^{
        [self synthesisToPullAudioOutputStream];
    });
}

- (IBAction)synthesisToPushAudioOutputStreamButtonTapped:(UIButton *)sender {
    dispatch_async(dispatch_get_global_queue(QOS_CLASS_DEFAULT, 0), ^{
        [self synthesisToPushAudioOutputStream];
    });
}

- (IBAction)synthesisToAudioDataStreamButtonTapped:(UIButton *)sender {
    dispatch_async(dispatch_get_global_queue(QOS_CLASS_DEFAULT, 0), ^{
        [self synthesisToAudioDataStream];
    });
}

- (IBAction)synthesisEventButtonTapped:(UIButton *)sender {
    dispatch_async(dispatch_get_global_queue(QOS_CLASS_DEFAULT, 0), ^{
        [self synthesisEvent];
    });
}

/*
 * Performs speech synthesis to a result, and plays the synthesized audio.
 */
- (void)synthesisToSpeaker {
    SPXSpeechConfiguration *speechConfig = [[SPXSpeechConfiguration alloc] initWithSubscription:speechKey region:serviceRegion];
    if (!speechConfig) {
        NSLog(@"Could not load speech config");
        [self updateErrorText:(@"Speech Config Error")];
        return;
    }
    // Sets the synthesis language.
    // The full list of supported language can be found here:
    // https://docs.microsoft.com/azure/cognitive-services/speech-service/language-support#text-to-speech
    speechConfig.speechSynthesisLanguage = @"en-GB";
    // Sets the voice name
    // e.g. "Microsoft Server Speech Text to Speech Voice (en-US, JennyNeural)".
    // The full list of supported voices can be found here:
    // https://aka.ms/csspeech/voicenames
    // And, you can try getVoices method to get all available voices.
    speechConfig.speechSynthesisVoiceName = @"Microsoft Server Speech Text to Speech Voice (en-GB, RyanNeural)";
    // Sets the synthesis output format.
    // The full list of supported format can be found here:
    // https://docs.microsoft.com/azure/cognitive-services/speech-service/rest-text-to-speech#audio-outputs
    [speechConfig setSpeechSynthesisOutputFormat:SPXSpeechSynthesisOutputFormat_Riff16Khz16BitMonoPcm];
    // If you are using Custom Voice (https://aka.ms/customvoice),
    // uncomment the following line to set the endpoint id of your Custom Voice model.
    // speechConfig.EndpointId = @"YourEndpointId";

    [self updateStatusText:(@"Synthesizing...")];

    SPXSpeechSynthesizer *synthesizer = [[SPXSpeechSynthesizer alloc] init:speechConfig];
    if (!synthesizer) {
        NSLog(@"Could not create speech synthesizer");
        [self updateResultText:(@"Speech Synthesis Error")];
        return;
    }

    SPXSpeechSynthesisResult *speechResult = [synthesizer speakText:inputText];

    // Checks result.
    if (SPXResultReason_Canceled == speechResult.reason) {
        SPXSpeechSynthesisCancellationDetails *details = [[SPXSpeechSynthesisCancellationDetails alloc] initFromCanceledSynthesisResult:speechResult];
        NSLog(@"Speech synthesis was canceled: %@. Did you pass the correct key/region combination?", details.errorDetails);
        [self updateErrorText:([NSString stringWithFormat:@"Canceled: %@", details.errorDetails])];
    } else if (SPXResultReason_SynthesizingAudioCompleted == speechResult.reason) {
        NSLog(@"Speech synthesis was completed");
        [self updateResultText:@"Speech synthesis was completed."];
    } else {
        NSLog(@"There was an error.");
        [self updateErrorText:(@"Speech synthesis error.")];
    }
}

/*
 * Performs speech synthesis to a file.
 */
- (void)synthesisToFile {

    NSString *filePath = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
    NSString *fileName = @"/wavefile.wav";
    NSString *fileAtPath = [filePath stringByAppendingString:fileName];
    NSLog(@"The file path is %@.", fileAtPath);
    if (![[NSFileManager defaultManager] fileExistsAtPath:fileAtPath]) {
        [[NSFileManager defaultManager] createFileAtPath:fileAtPath contents:nil attributes:nil];
    }

    SPXAudioConfiguration *audioConfig = [[SPXAudioConfiguration alloc] initWithWavFileOutput:fileAtPath];

    SPXSpeechConfiguration *speechConfig = [[SPXSpeechConfiguration alloc] initWithSubscription:speechKey region:serviceRegion];
    if (!speechConfig) {
        NSLog(@"Could not load speech config");
        [self updateErrorText:(@"Speech Config Error")];
        return;
    }

    [self updateStatusText:(@"Synthesizing...")];

    SPXSpeechSynthesizer *synthesizer = [[SPXSpeechSynthesizer alloc] initWithSpeechConfiguration:speechConfig audioConfiguration:audioConfig];
    if (!synthesizer) {
        NSLog(@"Could not create speech synthesizer");
        [self updateResultText:(@"Speech Synthesis Error")];
        return;
    }

    SPXSpeechSynthesisResult *speechResult = [synthesizer speakText:inputText];

    // Checks result.
    if (SPXResultReason_Canceled == speechResult.reason) {
        SPXSpeechSynthesisCancellationDetails *details = [[SPXSpeechSynthesisCancellationDetails alloc] initFromCanceledSynthesisResult:speechResult];
        NSLog(@"Speech synthesis was canceled: %@. Did you pass the correct key/region combination?", details.errorDetails);
        [self updateErrorText:([NSString stringWithFormat:@"Canceled: %@", details.errorDetails])];
    } else if (SPXResultReason_SynthesizingAudioCompleted == speechResult.reason) {
        NSLog(@"Speech synthesis was completed");
        [self updateResultText:@"Speech synthesis was completed."];
    } else {
        NSLog(@"There was an error.");
        [self updateErrorText:(@"Speech synthesis error.")];
    }
}

/*
 * Performs speech synthesis using pull audio output stream.
 */
- (void)synthesisToPullAudioOutputStream {
    NSString *filePath = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
    NSString *fileName = @"/pullStream.wav";
    NSString *fileAtPath = [filePath stringByAppendingString:fileName];
    if (![[NSFileManager defaultManager] fileExistsAtPath:fileAtPath]) {
        [[NSFileManager defaultManager] createFileAtPath:fileAtPath contents:nil attributes:nil];
    }

    SPXPullAudioOutputStream *stream = [[SPXPullAudioOutputStream alloc] init];
    SPXAudioConfiguration *audioConfig = [[SPXAudioConfiguration alloc] initWithStreamOutput:stream];

    SPXSpeechConfiguration *speechConfig = [[SPXSpeechConfiguration alloc] initWithSubscription:speechKey region:serviceRegion];
    if (!speechConfig) {
        NSLog(@"Could not load speech config");
        [self updateErrorText:(@"Speech Config Error")];
        return;
    }

    [self updateStatusText:(@"Synthesizing...")];

    SPXSpeechSynthesizer *synthesizer = [[SPXSpeechSynthesizer alloc] initWithSpeechConfiguration:speechConfig audioConfiguration:audioConfig];
    if (!synthesizer) {
        NSLog(@"Could not create speech synthesizer");
        [self updateResultText:(@"Speech Synthesis Error")];
        return;
    }

    SPXSpeechSynthesisResult *speechResult = [synthesizer speakText:inputText];

    // Checks result.
    if (SPXResultReason_Canceled == speechResult.reason) {
        SPXSpeechSynthesisCancellationDetails *details = [[SPXSpeechSynthesisCancellationDetails alloc] initFromCanceledSynthesisResult:speechResult];
        NSLog(@"Speech synthesis was canceled: %@. Did you pass the correct key/region combination?", details.errorDetails);
        [self updateErrorText:([NSString stringWithFormat:@"Canceled: %@", details.errorDetails])];
    } else if (SPXResultReason_SynthesizingAudioCompleted == speechResult.reason) {
        NSLog(@"Speech synthesis was completed");
        [self updateResultText:@"Speech synthesis was completed."];
    } else {
        NSLog(@"There was an error.");
        [self updateErrorText:(@"Speech synthesis error.")];
    }
    synthesizer = nil;
    NSFileHandle *file = [NSFileHandle fileHandleForWritingAtPath:fileAtPath];
    if (file == nil) {
        NSLog(@"failed to open file");
        [self updateErrorText:[NSString stringWithFormat:@"failed to open file at %@", fileAtPath]];
    }
    NSMutableData *data = [[NSMutableData alloc] initWithCapacity:1024];
    while ([stream read:data length:1024] > 0) {
        [file writeData:data];
        [file seekToEndOfFile];
    }
    [file closeFile];
}

/*
 * Performs speech synthesis using push audio output stream.
 */
- (void)synthesisToPushAudioOutputStream {
    // create file
    NSString *filePath = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
    NSString *fileName = @"/pushStream.wav";
    NSString *fileAtPath = [filePath stringByAppendingString:fileName];
    if (![[NSFileManager defaultManager] fileExistsAtPath:fileAtPath]) {
        [[NSFileManager defaultManager] createFileAtPath:fileAtPath contents:nil attributes:nil];
    }
    __block NSFileHandle *file = [NSFileHandle fileHandleForWritingAtPath:fileAtPath];
    if (file == nil) {
        NSLog(@"failed to open file");
        [self updateErrorText:[NSString stringWithFormat:@"failed to open file at %@", fileAtPath]];
    }

    // set up the stream with the push callback
    SPXPushAudioOutputStream* stream = [[SPXPushAudioOutputStream alloc]
                                        initWithWriteHandler:
                                        ^NSUInteger(NSData *data) {
                                            [file writeData:data];
                                            [file seekToEndOfFile];
                                            return [data length];
                                        }
                                        closeHandler:
                                        ^(void) {
                                            [file closeFile];
                                        }];

    SPXAudioConfiguration *audioConfig = [[SPXAudioConfiguration alloc] initWithStreamOutput:stream];

    SPXSpeechConfiguration *speechConfig = [[SPXSpeechConfiguration alloc] initWithSubscription:speechKey region:serviceRegion];
    if (!speechConfig) {
        NSLog(@"Could not load speech config");
        [self updateErrorText:(@"Speech Config Error")];
        return;
    }

    [self updateStatusText:(@"Synthesizing...")];

    SPXSpeechSynthesizer *synthesizer = [[SPXSpeechSynthesizer alloc] initWithSpeechConfiguration:speechConfig audioConfiguration:audioConfig];
    if (!synthesizer) {
        NSLog(@"Could not create speech synthesizer");
        [self updateResultText:(@"Speech Synthesis Error")];
        return;
    }

    SPXSpeechSynthesisResult *speechResult = [synthesizer speakText:inputText];

    // Checks result.
    if (SPXResultReason_Canceled == speechResult.reason) {
        SPXSpeechSynthesisCancellationDetails *details = [[SPXSpeechSynthesisCancellationDetails alloc] initFromCanceledSynthesisResult:speechResult];
        NSLog(@"Speech synthesis was canceled: %@. Did you pass the correct key/region combination?", details.errorDetails);
        [self updateErrorText:([NSString stringWithFormat:@"Canceled: %@", details.errorDetails])];
    } else if (SPXResultReason_SynthesizingAudioCompleted == speechResult.reason) {
        NSLog(@"Speech synthesis was completed");
        [self updateResultText:@"Speech synthesis was completed."];
    } else {
        NSLog(@"There was an error.");
        [self updateErrorText:(@"Speech synthesis error.")];
    }
    synthesizer = nil;
}

/*
 * Performs speech synthesis using audio data stream.
 */
- (void)synthesisToAudioDataStream {
    SPXSpeechConfiguration *speechConfig = [[SPXSpeechConfiguration alloc] initWithSubscription:speechKey region:serviceRegion];
    if (!speechConfig) {
        NSLog(@"Could not load speech config");
        [self updateErrorText:(@"Speech Config Error")];
        return;
    }

    [self updateStatusText:(@"Synthesizing...")];

    SPXSpeechSynthesizer *synthesizer = [[SPXSpeechSynthesizer alloc] initWithSpeechConfiguration:speechConfig audioConfiguration:nil];
    if (!synthesizer) {
        NSLog(@"Could not create speech synthesizer");
        [self updateResultText:(@"Speech Synthesis Error")];
        return;
    }

    SPXSpeechSynthesisResult *speechResult = [synthesizer startSpeakingText:inputText];
    SPXAudioDataStream *stream = [[SPXAudioDataStream alloc] initFromSynthesisResult:speechResult];

    // create file
    NSString *filePath = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
    NSString *fileName = @"/dataStream.wav";
    NSString *fileAtPath = [filePath stringByAppendingString:fileName];
    if (![[NSFileManager defaultManager] fileExistsAtPath:fileAtPath]) {
        [[NSFileManager defaultManager] createFileAtPath:fileAtPath contents:nil attributes:nil];
    }
    __block NSFileHandle *file = [NSFileHandle fileHandleForWritingAtPath:fileAtPath];
    if (file == nil) {
        NSLog(@"failed to open file");
        [self updateErrorText:[NSString stringWithFormat:@"failed to open file at %@", fileAtPath]];
    }

    NSMutableData* data = [[NSMutableData alloc]initWithCapacity:1024];
    while ([stream readData:data length:1024] > 0) {
        [file writeData:data];
        [file seekToEndOfFile];
    }
    [file closeFile];
    [self updateResultText:@"Speech synthesis was completed."];
}

- (void)synthesisEvent {
    if (!synthesizerForEvents) {
        SPXSpeechConfiguration *speechConfig = [[SPXSpeechConfiguration alloc] initWithSubscription:speechKey region:serviceRegion];
        if (!speechConfig) {
            NSLog(@"Could not load speech config");
            [self updateErrorText:(@"Speech Config Error")];
            return;
        }

        synthesizerForEvents = [[SPXSpeechSynthesizer alloc] initWithSpeechConfiguration:speechConfig audioConfiguration:nil];
        if (!synthesizerForEvents) {
            NSLog(@"Could not create speech synthesizer");
            [self updateResultText:(@"Speech Synthesis Error")];
            return;
        }

        // connet callbacks
        [synthesizerForEvents addSynthesisStartedEventHandler: ^ (SPXSpeechSynthesizer *synthesizer, SPXSpeechSynthesisEventArgs *eventArgs) {
            NSLog(@"Synthesis started");
            [self updateStatusText:@"Synthesis started."];
        }];

        [synthesizerForEvents addSynthesizingEventHandler: ^ (SPXSpeechSynthesizer *synthesizer, SPXSpeechSynthesisEventArgs *eventArgs) {
            NSLog(@"Synthesizing");
            [self updateStatusText:@"Synthesizing."];
        }];

        [synthesizerForEvents addSynthesisCompletedEventHandler: ^ (SPXSpeechSynthesizer *synthesizer, SPXSpeechSynthesisEventArgs *eventArgs) {
            [self updateResultText:@"Speech synthesis was completed."];
        }];

        [synthesizerForEvents addSynthesisCanceledEventHandler: ^ (SPXSpeechSynthesizer *synthesizer, SPXSpeechSynthesisEventArgs *eventArgs) {
            SPXSpeechSynthesisCancellationDetails *details = [[SPXSpeechSynthesisCancellationDetails alloc] initFromCanceledSynthesisResult:eventArgs.result];
            NSLog(@"Speech synthesis was canceled: %@. Did you pass the correct key/region combination?", details.errorDetails);
            [self updateErrorText:([NSString stringWithFormat:@"Canceled: %@", details.errorDetails])];
        }];

        [synthesizerForEvents addSynthesisWordBoundaryEventHandler: ^ (SPXSpeechSynthesizer *synthesizer, SPXSpeechSynthesisWordBoundaryEventArgs *eventArgs) {
            // The unit of AudioOffset is tick (1 tick = 100 nanoseconds), divide by 10,000 to converted to milliseconds.
            NSLog(@"Word boundary event received. Audio offset: %fms, text offset %lu, word length: %lu.", eventArgs.audioOffset/10000., eventArgs.textOffset, eventArgs.wordLength);
            [self updateStatusText:[NSString stringWithFormat:@"Word boundary event received. Audio offset: %fms, text offset %lu, word length: %lu.", eventArgs.audioOffset/10000., eventArgs.textOffset, eventArgs.wordLength]];
        }];

        [synthesizerForEvents addVisemeReceivedEventHandler: ^ (SPXSpeechSynthesizer *synthesizer, SPXSpeechSynthesisVisemeEventArgs *eventArgs) {
            // The unit of AudioOffset is tick (1 tick = 100 nanoseconds), divide by 10,000 to converted to milliseconds.
            NSLog(@"Viseme event received. Audio offset: %fms, viseme id: %lu.", eventArgs.audioOffset/10000., eventArgs.visemeId);
            [self updateStatusText:[NSString stringWithFormat:@"Viseme event received. Audio offset: %fms, viseme id: %lu.", eventArgs.audioOffset/10000., eventArgs.visemeId]];
        }];

        // To trigger a bookmark event, bookmark tags are required in the SSML, e.g.
        // "<speak version='1.0' xml:lang='en-US' xmlns='http://www.w3.org/2001/10/synthesis' xmlns:mstts='http://www.w3.org/2001/mstts'><voice name='Microsoft Server Speech Text to Speech Voice (en-US, AriaNeural)'><bookmark mark='bookmark_one'/> one. <bookmark mark='bookmark_two'/> two. three. four.</voice></speak>"
        [synthesizerForEvents addBookmarkReachedEventHandler: ^ (SPXSpeechSynthesizer *synthesizer, SPXSpeechSynthesisBookmarkEventArgs *eventArgs) {
            // The unit of AudioOffset is tick (1 tick = 100 nanoseconds), divide by 10,000 to converted to milliseconds.
            NSLog(@"Bookmark reached. Audio offset: %fms, bookmark text: %@.", eventArgs.audioOffset/10000., eventArgs.text);
            [self updateStatusText:[NSString stringWithFormat:@"Bookmark reached. Audio offset: %fms, bookmark text: %@.", eventArgs.audioOffset/10000., eventArgs.text]];
        }];
    }

    [synthesizerForEvents startSpeakingText:inputText];
}

- (BOOL)textField:(UITextField *)textField shouldChangeCharactersInRange:(NSRange)range replacementString:(NSString *)string {
    inputText = [textField.text stringByReplacingCharactersInRange:range withString:string];
    return true;
}

- (void)updateResultText:(NSString *) resultText {
    dispatch_async(dispatch_get_main_queue(), ^{
        self.resultLabel.textColor = UIColor.blackColor;
        self.resultLabel.text = resultText;
    });
}

- (void)updateErrorText:(NSString *) errorText {
    dispatch_async(dispatch_get_main_queue(), ^{
        self.resultLabel.textColor = UIColor.redColor;
        self.resultLabel.text = errorText;
    });
}

- (void)updateStatusText:(NSString *) statusText {
    dispatch_async(dispatch_get_main_queue(), ^{
        self.resultLabel.textColor = UIColor.grayColor;
        self.resultLabel.text = statusText;
    });
}

@end

