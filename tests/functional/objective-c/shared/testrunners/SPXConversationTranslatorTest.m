//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import <XCTest/XCTest.h>
#import <AVFoundation/AVFoundation.h>
#import <MicrosoftCognitiveServicesSpeech/SPXSpeechApi.h>

@interface SPXConversationTranslatorTest : XCTestCase {
    NSString *singleUtteranceTextEnglish;
    NSString *singleUtteranceFileName;

    NSMutableDictionary *result;
    NSPredicate *finalResultCountPred;
    NSPredicate *sessionStoppedCountPred;
    NSPredicate *textMessageReceivedCountPred;
    double timeoutInSeconds;
}

@property (nonatomic, assign) NSString* speechKey;
@property (nonatomic, assign) NSString* serviceRegion;
@property (nonatomic, retain) SPXSpeechTranslationConfiguration* speechConfig;
@property (nonatomic, retain) SPXAudioConfiguration* audioConfig;
@property (nonatomic, retain) SPXConversationTranslator* conversationTranslator;
@property (nonatomic, retain) SPXConversation* conversation;
@property (nonatomic, retain) SPXParticipant* participant1;

- (void)setSpeechConfig;

@end

@implementation SPXConversationTranslatorTest

- (void)setUp {
    [super setUp];
    timeoutInSeconds = 20.;
    singleUtteranceFileName = @"whatstheweatherlike";
    singleUtteranceTextEnglish = @"What's the weather like?";
    
    [self setSpeechConfig];
    
    NSBundle *bundle = [NSBundle bundleForClass:[self class]];
    NSString *targetFile = [bundle pathForResource: singleUtteranceFileName ofType:@"wav"];
    self.audioConfig = [[SPXAudioConfiguration alloc] initWithWavFileInput:targetFile];
    self.conversationTranslator = [[SPXConversationTranslator alloc] initWithAudioConfiguration:self.audioConfig];
    [self setEventHandlers];
}

- (void)setSpeechConfig {
    self.speechKey = [[[NSProcessInfo processInfo] environment] objectForKey:@"subscriptionKey"];
    self.serviceRegion = [[[NSProcessInfo processInfo] environment] objectForKey:@"serviceRegion"];
    self.speechConfig = [[SPXSpeechTranslationConfiguration alloc] initWithSubscription:self.speechKey region:self.serviceRegion];
    self.speechConfig.speechRecognitionLanguage = @"en-US";
    [self.speechConfig addTargetLanguage:@"en-US"];
    [self.speechConfig addTargetLanguage:@"de-DE"];
    [self.speechConfig addTargetLanguage:@"ja-JP"];
}

- (void)setEventHandlers {
    result = [NSMutableDictionary new];
    [result setObject:@0 forKey:@"finalResultCount"];
    [result setObject:@0 forKey:@"sessionStoppedCount"];
    [result setObject:@0 forKey:@"textMessageReceivedCount"];
    [result setObject:@"" forKey:@"finalText"];

    __unsafe_unretained typeof(self) weakSelf = self;

    [self.conversationTranslator addTranscribingEventHandler:^(SPXConversationTranslator * translator, SPXConversationTranslationEventArgs * eventArgs) {
        
    }];
    
    [self.conversationTranslator addTranscribedEventHandler: ^ (SPXConversationTranslator *translator, SPXConversationTranslationEventArgs *eventArgs) {
        NSLog(@"Received final result event. SessionId: %@, transcription result:%@. Status %ld. offset %llu duration %llu resultid:%@", eventArgs.sessionId, eventArgs.result.text, (long)eventArgs.result.reason,
              eventArgs.result.offset, eventArgs.result.duration,
              eventArgs.result.resultId);
        // Do not store empty results
        if ([eventArgs.result.text length] != 0) {
            [weakSelf->result setObject:eventArgs.result.text forKey: @"finalText"];
            [weakSelf->result setObject:[NSNumber numberWithLong:[[weakSelf->result objectForKey:@"finalResultCount"] integerValue] + 1] forKey:@"finalResultCount"];
        }
    }];

    [self.conversationTranslator addSessionStartedEventHandler: ^ (SPXConversationTranslator *translator, SPXSessionEventArgs *eventArgs) {
        NSLog(@"Received session started event. SessionId: %@", eventArgs.sessionId);
    }];

    [self.conversationTranslator addSessionStoppedEventHandler: ^ (SPXConversationTranslator *translator, SPXSessionEventArgs *eventArgs) {
        NSLog(@"Received session stopped event. SessionId: %@", eventArgs.sessionId);
        [weakSelf->result setObject:[NSNumber numberWithLong:[[weakSelf->result objectForKey:@"sessionStoppedCount"] integerValue] + 1] forKey:@"sessionStoppedCount"];
    }];
    
    [self.conversationTranslator addTextMessageReceivedEventHandler:^(SPXConversationTranslator * ct, SPXConversationTranslationEventArgs * eventArgs) {
        NSLog(@"Received text message received event: %@", eventArgs.result.text);
        [weakSelf->result setObject:[NSNumber numberWithLong:[[weakSelf->result objectForKey:@"textMessageReceivedCount"] integerValue] + 1] forKey:@"textMessageReceivedCount"];
    }];
    
    sessionStoppedCountPred = [NSPredicate predicateWithBlock: ^BOOL (id  _Nullable evaluatedObject, NSDictionary<NSString *,id> * _Nullable bindings){
        return [[evaluatedObject valueForKey:@"sessionStoppedCount"] isEqualToNumber:@1];
    }];
    
    finalResultCountPred = [NSPredicate predicateWithBlock: ^BOOL (id  _Nullable evaluatedObject, NSDictionary<NSString *,id> * _Nullable bindings){
        return [[evaluatedObject valueForKey:@"finalResultCount"] isEqualToNumber:@1];
    }];
    textMessageReceivedCountPred = [NSPredicate predicateWithBlock: ^BOOL (id  _Nullable evaluatedObject, NSDictionary<NSString *,id> * _Nullable bindings){
        return [[evaluatedObject valueForKey:@"textMessageReceivedCount"] isEqualToNumber:@1];
    }];
}

- (void)tearDown {
    [super tearDown];
}

- (void)testCreateConversationTranslator {
    SPXConversationTranslator* conversationTranslator = [[SPXConversationTranslator alloc] init];
    XCTAssertNotNil(conversationTranslator);

    SPXConversationTranslator* conversationTranslator2 = [[SPXConversationTranslator alloc] initWithAudioConfiguration:self.audioConfig];
    XCTAssertNotNil(conversationTranslator2);
}

- (void)testJoinConversationFailure {
    __block BOOL asyncResult = false;
    __block NSError* asyncError = nil;
    XCTestExpectation *expectation = [self expectationWithDescription:@"async request"];
    @try {
        [self.conversationTranslator joinConversationAsync:^(BOOL joined, NSError * err) {
            asyncResult = joined;
            asyncError = err;
            NSLog(@"joinConversationAsync completed");
            [expectation fulfill];
        } conversationId:@"_" nickname:@"testJoinFailure" language:@"en-US"];
    }
    @catch (NSException* e) {
        NSLog(@"%@ Exception occured in joinConversationAsync", e.reason);
    }
    NSLog(@"Waiting for joining");
    [self waitForExpectationsWithTimeout:timeoutInSeconds handler:nil];
    XCTAssertEqual(asyncResult, NO);
    XCTAssertNotNil(asyncError);
}

- (void)testStartTranscribeFailure {
    __block BOOL asyncResult = false;
    __block NSError* asyncError = nil;
    XCTestExpectation *expectation = [self expectationWithDescription:@"async request"];
    @try {
        [self.conversationTranslator startTranscribingAsync:^(BOOL started, NSError * err) {
            asyncResult = started;
            asyncError = err;
            NSLog(@"startTranscribingAsync completed");
            [expectation fulfill];
        }];
    }
    @catch (NSException* e) {
        NSLog(@"%@ Exception occured in startTranscribingAsync", e.reason);
    }
    NSLog(@"Waiting for starting");
    [self waitForExpectationsWithTimeout:timeoutInSeconds handler:nil];
    XCTAssertEqual(asyncResult, NO);
    XCTAssertNotNil(asyncError);
}

- (void)testStopTranscribeFailure {
    __block BOOL asyncResult = false;
    __block NSError* asyncError = nil;
    XCTestExpectation *expectation = [self expectationWithDescription:@"async request"];
    @try {
        [self.conversationTranslator stopTranscribingAsync:^(BOOL stopped, NSError * err) {
            asyncResult = stopped;
            asyncError = err;
            NSLog(@"stopTranscribingAsync completed");
            [expectation fulfill];
        }];
    }
    @catch (NSException* e) {
        NSLog(@"%@ Exception occured in stopTranscribingAsync", e.reason);
    }
    NSLog(@"Waiting for stopping");
    [self waitForExpectationsWithTimeout:timeoutInSeconds handler:nil];
    XCTAssertEqual(asyncResult, NO);
    XCTAssertNotNil(asyncError);
}

- (void)testCreateSendTextTranscribe {
    self.conversation = [[SPXConversation alloc] init:self.speechConfig];
    XCTAssertNotNil(self.conversation);
    NSLog(@"conversation created successfully");
    
    __block BOOL asyncResult = false;
    __block NSError* asyncError = nil;
    XCTestExpectation *expectation = [self expectationWithDescription:@"async request"];
    [self.conversation startConversationAsync:^(BOOL started, NSError * err) {
        asyncResult = started;
        asyncError = err;
        [expectation fulfill];
    }];
    NSLog(@"Waiting for starting conversation");
    [self waitForExpectationsWithTimeout:timeoutInSeconds handler:nil];
    XCTAssertEqual(asyncResult, YES);
    XCTAssertNil(asyncError);

    expectation = [self expectationWithDescription:@"async request"];
    [self.conversationTranslator joinConversationAsync:^(BOOL joined, NSError * err) {
        asyncResult = joined;
        asyncError = err;
        [expectation fulfill];
    } conversation:self.conversation nickname:@"IamHost"];
     
    NSLog(@"Waiting for joining conversation");
    [self waitForExpectationsWithTimeout:timeoutInSeconds handler:nil];
    XCTAssertEqual(asyncResult, YES);
    XCTAssertNil(asyncError);
    
    expectation = [self expectationWithDescription:@"async request"];
    
    [self.conversationTranslator sendTextMessageAsync:^(BOOL sent, NSError * err) {
        asyncResult = sent;
        asyncError = err;
        [expectation fulfill];
    } message:@"Can you see this text message?"];
    
    NSLog(@"Waiting for sending text message");
    [self waitForExpectationsWithTimeout:timeoutInSeconds handler:nil];
    XCTAssertEqual(asyncResult, YES);
    XCTAssertNil(asyncError);

    [self expectationForPredicate:self->textMessageReceivedCountPred evaluatedWithObject:self->result handler:nil];
    [self waitForExpectationsWithTimeout:self->timeoutInSeconds handler:nil];
    
    expectation = [self expectationWithDescription:@"async request"];
    [self.conversationTranslator startTranscribingAsync:^(BOOL started, NSError * err) {
        asyncResult = started;
        asyncError = err;
        [expectation fulfill];
    }];
    
    NSLog(@"Waiting for start transcribing");
    [self waitForExpectationsWithTimeout:timeoutInSeconds handler:nil];
    XCTAssertEqual(asyncResult, YES);
    XCTAssertNil(asyncError);

    [self expectationForPredicate:self->finalResultCountPred evaluatedWithObject:self->result handler:nil];
    [self waitForExpectationsWithTimeout:self->timeoutInSeconds handler:nil];
    
    expectation = [self expectationWithDescription:@"async request"];
    [self.conversationTranslator stopTranscribingAsync: ^ (BOOL stopped, NSError * err) {
        NSLog(@"Conversation translation stopped");
        [expectation fulfill];
    }];

    NSLog(@"Waiting for conversation translation stopping");
    [self waitForExpectationsWithTimeout:timeoutInSeconds handler:nil];
    
    expectation = [self expectationWithDescription:@"async request"];
    [self.conversationTranslator leaveConversationAsync:^(BOOL left, NSError * err) {
        NSLog(@"Left from conversation translation");
        [expectation fulfill];
    }];

    NSLog(@"Waiting for leaving conversation translation");
    [self waitForExpectationsWithTimeout:timeoutInSeconds handler:nil];
    
    [self expectationForPredicate:self->sessionStoppedCountPred evaluatedWithObject:self->result handler:nil];
    [self waitForExpectationsWithTimeout:self->timeoutInSeconds handler:nil];
    
    NSString *finalResultText = [[[[self->result valueForKey:@"finalText"] componentsSeparatedByCharactersInSet:[[NSCharacterSet letterCharacterSet] invertedSet]] componentsJoinedByString:@""] lowercaseString];
    NSString *normalizedDesiredResultText = [[[self->singleUtteranceTextEnglish componentsSeparatedByCharactersInSet:[[NSCharacterSet letterCharacterSet] invertedSet]] componentsJoinedByString:@""] lowercaseString];

    NSLog(@"final result: \n%@\ndesired:\n%@", finalResultText, normalizedDesiredResultText);
    XCTAssertTrue([finalResultText isEqualToString: normalizedDesiredResultText]);
}

- (void)testStartAndJoinConversationWithErrorVariant {
    NSError *syncError = nil;
    self.conversation = [[SPXConversation alloc] init:self.speechConfig error:&syncError];
    XCTAssertNotNil(self.conversation);
    NSLog(@"conversation created successfully");
    
    __block BOOL asyncResult = false;
    __block NSError* asyncError = nil;
    XCTestExpectation *expectation = [self expectationWithDescription:@"async request"];
    [self.conversation startConversationAsync:^(BOOL started, NSError * err) {
        asyncResult = started;
        asyncError = err;
        [expectation fulfill];
    } error:&syncError];
    
    NSLog(@"Waiting for starting conversation");
    [self waitForExpectationsWithTimeout:timeoutInSeconds handler:nil];
    XCTAssertEqual(asyncResult, YES);
    asyncResult = NO;
    XCTAssertNil(asyncError);
    
    expectation = [self expectationWithDescription:@"async request"];
    [self.conversationTranslator joinConversationAsync:^(BOOL joined, NSError * err) {
        asyncResult = joined;
        asyncError = err;
        [expectation fulfill];
    } conversation:self.conversation nickname:@"IamHost" error:&syncError];
    
    NSLog(@"Waiting for joining conversation");
    [self waitForExpectationsWithTimeout:timeoutInSeconds handler:nil];
    XCTAssertEqual(asyncResult, YES);
    XCTAssertNil(asyncError);
}

@end
