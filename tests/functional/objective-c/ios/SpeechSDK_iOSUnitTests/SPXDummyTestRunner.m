//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import <XCTest/XCTest.h>
#import "dummytest.h"

@interface SPXDummyTestRunner : XCTestCase

@end

@implementation SPXDummyTestRunner

- (void)setUp {
    [super setUp];
}

- (void)tearDown {
    [super tearDown];
}

- (void)testAPI {
    [DummyTest runTest];
}

@end
