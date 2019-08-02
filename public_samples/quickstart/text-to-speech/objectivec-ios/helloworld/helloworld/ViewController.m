//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "ViewController.h"
#import <AVFoundation/AVFoundation.h>
#import <MicrosoftCognitiveServicesSpeech/SPXSpeechApi.h>

@interface ViewController () {
    NSString *inputText;
    
    NSString *speechKey;
    NSString *serviceRegion;
}

@property (nonatomic, strong) AVAudioPlayer *player;
- (IBAction)synthesisButtonTapped:(UIButton *)sender;

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    inputText = @"";
    
    speechKey = @"YourSubscriptionKey";
    serviceRegion = @"YourServiceRegion";
}

- (IBAction)synthesisButtonTapped:(UIButton *)sender {
    dispatch_async(dispatch_get_global_queue(QOS_CLASS_DEFAULT, 0), ^{
        [self synthesis];
    });
}

- (void)synthesis {
    SPXSpeechConfiguration *speechConfig = [[SPXSpeechConfiguration alloc] initWithSubscription:speechKey region:serviceRegion];
    [speechConfig setSpeechSynthesisOutputFormat:SPXSpeechSynthesisOutputFormat_Audio16Khz32KBitRateMonoMp3];
    SPXSpeechSynthesizer *speechSynthesizer = [[SPXSpeechSynthesizer alloc] initWithSpeechConfiguration:speechConfig audioConfiguration:nil];
    
    NSLog(@"Start synthesizing...");
    
    SPXSpeechSynthesisResult *speechResult = [speechSynthesizer SpeakText:inputText];
    
    // Checks result.
    if (SPXResultReason_Canceled == speechResult.reason) {
        SPXSpeechSynthesisCancellationDetails *details = [[SPXSpeechSynthesisCancellationDetails alloc] initFromCanceledSynthesisResult:speechResult];
        NSLog(@"Speech synthesis was canceled: %@. Did you pass the correct key/region combination?", details.errorDetails);
    } else if (SPXResultReason_SynthesizingAudioCompleted == speechResult.reason) {
        NSLog(@"Speech synthesis was completed");
        // Play audio.
        self.player = [[AVAudioPlayer alloc] initWithData:[speechResult audioData] error:nil];
        [self.player prepareToPlay];
        [self.player play];
    } else {
        NSLog(@"There was an error.");
    }
}

- (BOOL)textField:(UITextField *)textField shouldChangeCharactersInRange:(NSRange)range replacementString:(NSString *)string {;
    inputText = [textField.text stringByReplacingCharactersInRange:range withString:string];
    return true;
}

@end
