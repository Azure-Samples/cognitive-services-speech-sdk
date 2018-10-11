//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import <XCTest/XCTest.h>
#import "test_audio.h"


@interface SPXAudioTestRunner : XCTestCase
@property (nonatomic, assign) NSString * speechKey;
@property (nonatomic, assign) NSString * serviceRegion;

@end

@implementation SPXAudioTestRunner

- (void)setUp {
    [super setUp];
    self.speechKey = [[[NSProcessInfo processInfo] environment] objectForKey:@"subscriptionKey"];
}

- (void)tearDown {
    [super tearDown];
}

- (void)testPullAudio {
    [AudioStreamTest runPullTest:self.speechKey];
}

- (void)testPushAudio {
    [AudioStreamTest runPushTest:self.speechKey];
}

@end
