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
extern NSString *serviceRegion;
extern NSString *intentKey;
extern NSString *intentRegion;


@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    [DummyTest runTest:speechKey withRegion:serviceRegion];
    [EndToEndTests  runTest:speechKey withRegion:serviceRegion withIntentKey:intentKey withIntentRegion:intentRegion];
    [MicrophoneTest runAsync:speechKey withRegion:serviceRegion];
    [MicrophoneTest runTranslation:speechKey withRegion:serviceRegion];
    [MicrophoneTest runContinuous:speechKey withRegion:serviceRegion];
    [AudioStreamTest runPullTest:speechKey withRegion:serviceRegion];
    [AudioStreamTest runPushTest:speechKey withRegion:serviceRegion];
}


- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}


@end
