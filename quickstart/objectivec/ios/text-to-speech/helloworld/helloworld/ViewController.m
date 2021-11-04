//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

// <code>
#import "ViewController.h"
#import <AVFoundation/AVFoundation.h>
#import <MicrosoftCognitiveServicesSpeech/SPXSpeechApi.h>

@interface ViewController () {
    NSString *inputText;

    NSString *speechKey;
    NSString *serviceRegion;
}

@property (strong, nonatomic) IBOutlet UITextField *inputField;
@property (strong, nonatomic) IBOutlet UIButton *synthesisButton;
@property (strong, nonatomic) IBOutlet UILabel *resultLabel;

@property (nonatomic, strong) AVAudioPlayer *player;

- (IBAction)synthesisButtonTapped:(UIButton *)sender;

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    inputText = @"";
    speechKey = @"YourSubscriptionKey";
    serviceRegion = @"YourServiceRegion";

    [self.view setBackgroundColor:[UIColor whiteColor]];

    self.inputField = [[UITextField alloc] initWithFrame:CGRectMake(50, 150, 300, 50)];
    self.inputField.placeholder = @"Input something to synthesize...";
    self.inputField.borderStyle = UITextBorderStyleRoundedRect;
    self.inputField.delegate = self;
    self.inputField.accessibilityIdentifier = @"input_text_field";
    [self.view addSubview:self.inputField];

    self.synthesisButton = [UIButton buttonWithType:UIButtonTypeSystem];
    [self.synthesisButton addTarget:self action:@selector(synthesisButtonTapped:) forControlEvents:UIControlEventTouchUpInside];
    [self.synthesisButton setTitle:@"Synthesis" forState:UIControlStateNormal];
    [self.synthesisButton setFrame:CGRectMake(80, 350, 200, 50)];
    self.synthesisButton.titleLabel.font = [UIFont systemFontOfSize:36.0];
    self.synthesisButton.accessibilityIdentifier = @"synthesis_button";
    [self.view addSubview:self.synthesisButton];

    self.resultLabel = [[UILabel alloc] initWithFrame:CGRectMake(50.0, 400.0, 300.0, 200.0)];
    self.resultLabel.lineBreakMode = NSLineBreakByWordWrapping;
    self.resultLabel.numberOfLines = 0;
    self.resultLabel.accessibilityIdentifier = @"result_label";
    [self.view addSubview:self.resultLabel];
}

- (IBAction)synthesisButtonTapped:(UIButton *)sender {
    dispatch_async(dispatch_get_global_queue(QOS_CLASS_DEFAULT, 0), ^{
        [self synthesis];
    });
}

- (void)synthesis {
    SPXSpeechConfiguration *speechConfig = [[SPXSpeechConfiguration alloc] initWithSubscription:speechKey region:serviceRegion];
    SPXSpeechSynthesizer *speechSynthesizer = [[SPXSpeechSynthesizer alloc] init:speechConfig];

    NSLog(@"Start synthesizing...");
    [self updateText:@"Start synthesizing..." color:UIColor.grayColor];

    SPXSpeechSynthesisResult *speechResult = [speechSynthesizer speakText:inputText];

    // Checks result.
    if (SPXResultReason_Canceled == speechResult.reason) {
        SPXSpeechSynthesisCancellationDetails *details = [[SPXSpeechSynthesisCancellationDetails alloc] initFromCanceledSynthesisResult:speechResult];
        NSLog(@"Speech synthesis was canceled: %@. Did you pass the correct key/region combination?", details.errorDetails);
        [self updateText:[NSString stringWithFormat:@"Speech synthesis was canceled: %@. Did you pass the correct key/region combination?", details.errorDetails] color:UIColor.redColor];
    } else if (SPXResultReason_SynthesizingAudioCompleted == speechResult.reason) {
        NSLog(@"Speech synthesis was completed");
        [self updateText:@"The synthesis was completed." color:UIColor.blueColor];
    } else {
        NSLog(@"There was an error.");
        [self updateText:@"Synthesis error" color:UIColor.redColor];
    }
}

- (BOOL)textField:(UITextField *)textField shouldChangeCharactersInRange:(NSRange)range replacementString:(NSString *)string {;
    inputText = [textField.text stringByReplacingCharactersInRange:range withString:string];
    return true;
}

- (BOOL)textFieldShouldReturn:(UITextField *)textField{
    [self.view endEditing:true];
    return true;
}

- (void)updateText:(NSString *) resultText color:(UIColor *)color{
    dispatch_async(dispatch_get_main_queue(), ^{
        self.resultLabel.textColor = color;
        self.resultLabel.text = resultText;
    });
}

@end
// </code>
