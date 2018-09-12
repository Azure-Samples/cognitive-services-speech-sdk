//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

// <code>
#import "ViewController.h"
#import "speech_factory.h"

@interface ViewController ()
@property (weak, nonatomic) IBOutlet UIButton *recognizeButton;
@property (weak, nonatomic) IBOutlet UILabel *recognitionResultLabel;
- (IBAction)recognizeButtonTapped:(UIButton *)sender;
@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (IBAction)recognizeButtonTapped:(UIButton *)sender {
    __block bool end = false;
    SpeechFactory *factory = [SpeechFactory fromSubscription:@"YourSubscriptionKey" AndRegion:@"YourServiceRegion"];

    NSBundle *mainBundle = [NSBundle mainBundle];
    NSString *myFile = [mainBundle pathForResource: @"whatstheweatherlike" ofType: @"wav"];
    NSLog(@"Main bundle path: %@", mainBundle);
    NSLog(@"myFile path: %@", myFile);

    SpeechRecognizer *recognizer = [factory createSpeechRecognizerWithFileInput:myFile];

    [recognizer addFinalResultEventListener: ^ (SpeechRecognizer * recognizer, SpeechRecognitionResultEventArgs *eventArgs) {
        NSLog(@"Received final result event. SessionId: %@, recognition result:%@. Status %ld.", eventArgs.sessionId, eventArgs.result.text, (long)eventArgs.result.recognitionStatus);
        dispatch_async(dispatch_get_main_queue(), ^{
            [self updateRecognitionResultText:(eventArgs.result.text)];
        });
        end = true;
    }];

    [recognizer startContinuousRecognition];
    while (end == false)
        [NSThread sleepForTimeInterval:1.0f];
    [recognizer stopContinuousRecognition];

    [recognizer close];
}

- (void)updateRecognitionResultText:(NSString *) resultText {
    self.recognitionResultLabel.text = resultText;
}

@end
// </code>