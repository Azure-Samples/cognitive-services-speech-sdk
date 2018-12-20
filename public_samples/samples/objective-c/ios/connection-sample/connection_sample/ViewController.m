//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "ViewController.h"
#import <MicrosoftCognitiveServicesSpeech/SPXSpeechApi.h>

@interface ViewController () {
    NSString *speechKey;
    NSString *serviceRegion;
    SPXSpeechRecognizer* speechRecognizer;
}

@property (weak, nonatomic) IBOutlet UIButton *recognizeFromMicButton;
@property (weak, nonatomic) IBOutlet UILabel *recognitionResultLabel;
@end

@implementation ViewController

- (void)viewDidLoad {
    speechKey = @"YourSubscriptionKey";
    serviceRegion = @"YourServiceRegion";
    
    NSLog(@"Starting...");
    self.recognizeFromMicButton.enabled = NO;

    // setup recognizer
    SPXSpeechConfiguration *speechConfig = [[SPXSpeechConfiguration alloc] initWithSubscription:speechKey region:serviceRegion];
    if (!speechConfig) {
        NSLog(@"Could not load speech config");
        [self updateRecognitionErrorText:(@"Speech Config Error")];
        return;
    }
    
    speechRecognizer = [[SPXSpeechRecognizer alloc] init:speechConfig];
    if (!speechRecognizer) {
        NSLog(@"Could not create speech recognizer");
        [self updateRecognitionResultText:(@"Speech Recognition Error")];
        return;
    }
    
    SPXConnection* connection = [[SPXConnection alloc] initFromRecognizer:speechRecognizer];
    
    [connection addConnectedEventHandler: ^ (SPXConnection *connection, SPXConnectionEventArgs *eventArgs) {
        NSLog(@"Received connected event. SessionId: %@", eventArgs.sessionId);
        dispatch_async(dispatch_get_main_queue(), ^{
            self.recognizeFromMicButton.enabled = YES;
        });
    }];
    
    [connection addDisconnectedEventHandler: ^ (SPXConnection *connection, SPXConnectionEventArgs *eventArgs) {
        NSLog(@"Received disconnected event. SessionId: %@", eventArgs.sessionId);
        dispatch_async(dispatch_get_main_queue(), ^{
            self.recognizeFromMicButton.enabled = NO;
        });
    }];
    
    [connection open:NO];
}

- (IBAction)recognizeFromMicButtonTapped:(UIButton *)sender {
    self.recognizeFromMicButton.enabled = NO;
    
    dispatch_async(dispatch_get_global_queue(QOS_CLASS_DEFAULT, 0), ^{
        [self recognizeFromMicrophone];
    });
}

- (void)recognizeFromMicrophone {
    [self updateRecognitionStatusText:(@"Recognizing...")];
    
    NSLog(@"Speech recognition started");
    
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

    dispatch_async(dispatch_get_main_queue(), ^{
        self.recognizeFromMicButton.enabled = YES;
    });

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
