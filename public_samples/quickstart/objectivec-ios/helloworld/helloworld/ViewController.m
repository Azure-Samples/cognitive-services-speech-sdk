//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

// <code>
#import "ViewController.h"
#import <MicrosoftCognitiveServicesSpeech/speechapi.h>

@interface ViewController ()
@property (weak, nonatomic) IBOutlet UIButton *recognizeButton;
@property (weak, nonatomic) IBOutlet UILabel *recognitionResultLabel;
- (IBAction)recognizeButtonTapped:(UIButton *)sender;
@end

@implementation ViewController

- (IBAction)recognizeButtonTapped:(UIButton *)sender {
    __block bool end = false;
    NSBundle *mainBundle = [NSBundle mainBundle];
    NSString *weatherFile = [mainBundle pathForResource: @"whatstheweatherlike" ofType:@"wav"];
    NSLog(@"Main bundle path: %@", mainBundle);
    NSLog(@"weatherFile path: %@", weatherFile);
    AudioConfig* weatherAudioSource = [AudioConfig fromWavFileInput:weatherFile];

    NSString *speechKey = @"YourSubscriptionKey";
    NSString *serviceRegion = @"YourServiceRegion";

    SpeechConfig *speechConfig = [SpeechConfig fromSubscription:speechKey andRegion:serviceRegion];
    SpeechRecognizer* speechRecognizer = [SpeechRecognizer fromConfig:speechConfig usingAudio:weatherAudioSource];

    [speechRecognizer addSessionEventListener: ^ (Recognizer * recognizer, SessionEventArgs *eventArgs) {
        NSLog(@"Received Session event. Type:%@(%d) SessionId: %@", eventArgs.eventType == SessionStartedEvent? @"SessionStart" : @"SessionStop", (int)eventArgs.eventType, eventArgs.sessionId);
        if (eventArgs.eventType == SessionStoppedEvent)
            end = true;
    }];

    [speechRecognizer addRecognizedEventListener: ^ (SpeechRecognizer * recognizer, SpeechRecognitionEventArgs *eventArgs) {
        NSLog(@"Received final result event. SessionId: %@, recognition result:%@. Status %ld.", eventArgs.sessionId, eventArgs.result.text, (long)eventArgs.result.reason);
        dispatch_async(dispatch_get_main_queue(), ^{
            [self updateRecognitionResultText:(eventArgs.result.text)];
        });
    }];

    [speechRecognizer startContinuousRecognition];
    while (end == false)
        [NSThread sleepForTimeInterval:1.0f];
    [speechRecognizer stopContinuousRecognition];

    [speechRecognizer close];
}

- (void)updateRecognitionResultText:(NSString *) resultText {
    self.recognitionResultLabel.text = resultText;
}

@end
// </code>

