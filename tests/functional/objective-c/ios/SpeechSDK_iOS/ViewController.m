//
//  ViewController.m
//  SpeechSDK_iOS
//
//  Created by carbon on 11.08.18.
//  Copyright © 2018 MSFT. All rights reserved.
//

#import "ViewController.h"

#import "../../shared/test.h"
#import "../../shared/test_audio.h"
#import "../../shared/microphone_test.h"
#import "../../shared/dummytest.h"
@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    [DummyTest runTest];
    [EndToEndTests runTest];
    [MicrophoneTest runAsync];
    [MicrophoneTest runTranslation];
    [MicrophoneTest runContinuous];
    [AudioStreamTest runPullTest];
    [AudioStreamTest runPushTest];
}


- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}


@end
