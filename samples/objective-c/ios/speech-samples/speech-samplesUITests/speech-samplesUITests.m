//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import <XCTest/XCTest.h>

@interface helloworldUITests : XCTestCase {
    XCUIApplication *app;
    NSString *weatherText;
    NSString *beachText;
    NSString *keywordText;
}
@end

@implementation helloworldUITests

- (void)setUp {
    [super setUp];

    weatherText = @"What's the weather like?";
    beachText = @"Wreck a nice beach.";
    keywordText = @"Computer";
    // Put setup code here. This method is called before the invocation of each test method in the class.

    // In UI tests it is usually best to stop immediately when a failure occurs.
    self.continueAfterFailure = NO;
    // UI tests must launch the application that they test. Doing this in setup will make sure it happens for each test method.
    app = [[XCUIApplication alloc] init];
    [app launch];

    // In UI tests it’s important to set the initial state - such as interface orientation - required for your tests before they run. The setUp method is a good place to do this.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (void)testRecognizeFromWavFile {
    // sleep to make sure elements are there
    [NSThread sleepForTimeInterval:1];
    XCUIElement * reco_button = app.buttons[@"recognize_file_button"];
    XCTAssert(reco_button.exists);

    XCUIElement * result_label = app.staticTexts[@"result_label"];
    XCTAssert(result_label.exists);

    NSPredicate *pred = [NSPredicate predicateWithFormat:@"label == %@", weatherText];

    [self expectationForPredicate:pred evaluatedWithObject:result_label handler:nil];

    [reco_button tap];

    [self waitForExpectationsWithTimeout:20 handler:nil];
}

- (void)testRecognizeWithPhraseHint {
    // sleep to make sure elements are there
    [NSThread sleepForTimeInterval:1];
    XCUIElement * reco_button = app.buttons[@"recognize_phrase_hint_button"];
    XCTAssert(reco_button.exists);

    XCUIElement * result_label = app.staticTexts[@"result_label"];
    XCTAssert(result_label.exists);

    NSPredicate *pred = [NSPredicate predicateWithFormat:@"label == %@", beachText];

    [self expectationForPredicate:pred evaluatedWithObject:result_label handler:nil];

    [reco_button tap];

    [self waitForExpectationsWithTimeout:20 handler:nil];
}

- (void)testRecognizeFromPushStream {
    // sleep to make sure elements are there
    [NSThread sleepForTimeInterval:1];
    XCUIElement * reco_button = app.buttons[@"recognize_push_stream_button"];
    XCTAssert(reco_button.exists);

    XCUIElement * result_label = app.staticTexts[@"result_label"];
    XCTAssert(result_label.exists);

    NSPredicate *pred = [NSPredicate predicateWithFormat:@"label == %@", weatherText];

    [self expectationForPredicate:pred evaluatedWithObject:result_label handler:nil];

    [reco_button tap];

    [self waitForExpectationsWithTimeout:20 handler:nil];
}

- (void)testRecognizeFromPullStream {
    // sleep to make sure elements are there
    [NSThread sleepForTimeInterval:1];
    XCUIElement * reco_button = app.buttons[@"recognize_pull_stream_button"];
    XCTAssert(reco_button.exists);

    XCUIElement * result_label = app.staticTexts[@"result_label"];
    XCTAssert(result_label.exists);

    NSPredicate *pred = [NSPredicate predicateWithFormat:@"label == %@", weatherText];

    [self expectationForPredicate:pred evaluatedWithObject:result_label handler:nil];

    [reco_button tap];

    [self waitForExpectationsWithTimeout:20 handler:nil];
}

- (void)testRecognizeWithLanguageDetection {
    // sleep to make sure elements are there
    [NSThread sleepForTimeInterval:1];
    XCUIElement * reco_button = app.buttons[@"recognize_language_detection_button"];
    XCTAssert(reco_button.exists);

    XCUIElement * result_label = app.staticTexts[@"result_label"];
    XCTAssert(result_label.exists);

    // LID service can be instable for short sentences like "what's the weather like?", commenting following check for now
    NSPredicate *pred = [NSPredicate predicateWithFormat:@"label CONTAINS[c] %@", @"en-US"];
    [self expectationForPredicate:pred evaluatedWithObject:result_label handler:nil];

    [reco_button tap];

    [self waitForExpectationsWithTimeout:20 handler:nil];
}

- (void)testRecognizeKeyword {
    // sleep to make sure elements are there
    [NSThread sleepForTimeInterval:1];
    XCUIElement * reco_button = app.buttons[@"recognize_keyword_button"];
    XCTAssert(reco_button.exists);

    XCUIElement * result_label = app.staticTexts[@"result_label"];
    XCTAssert(result_label.exists);

    NSPredicate *pred = [NSPredicate predicateWithFormat:@"label == %@", keywordText];

    [self expectationForPredicate:pred evaluatedWithObject:result_label handler:nil];

    [reco_button tap];

    [self waitForExpectationsWithTimeout:20 handler:nil];
}

@end
