//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "ViewController.h"

#import "../../shared/test.h"
#import "../../shared/test_audio.h"
#import "../../shared/microphone_test.h"
#import "../../shared/dummytest.h"

extern NSString *speechKey;

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
    [AudioStreamTest runPullTest:speechKey];
    [AudioStreamTest runPushTest:speechKey];
}


- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}


@end
