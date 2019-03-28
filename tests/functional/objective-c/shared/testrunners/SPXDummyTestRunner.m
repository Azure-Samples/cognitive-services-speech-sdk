//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import <XCTest/XCTest.h>
#import "dummytest.h"

@interface SPXDummyTestRunner : XCTestCase
@property (nonatomic, assign) NSString * speechKey;
@property (nonatomic, assign) NSString * serviceRegion;

@end

@implementation SPXDummyTestRunner

- (void)setUp {
    [super setUp];
    self.speechKey = [[[NSProcessInfo processInfo] environment] objectForKey:@"subscriptionKey"];
    self.serviceRegion = [[[NSProcessInfo processInfo] environment] objectForKey:@"serviceRegion"];
}

- (void)tearDown {
    [super tearDown];
}

- (void)testAPI {
    [DummyTest runTest:self.speechKey withRegion:self.serviceRegion];
}

@end
