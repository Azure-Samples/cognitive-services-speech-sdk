//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import <XCTest/XCTest.h>
#import <MicrosoftCognitiveServicesSpeech/SPXSpeechApi.h>


@interface SPXTranslationRecognitionEndToEndTest : XCTestCase{
    NSString *weatherTextEnglish;
    NSString *weatherTextGerman;
    NSString *weatherTextChinese;
    NSString *weatherFileName;
    NSMutableDictionary *result;

    NSPredicate *sessionStoppedCountPred;

    double timeoutInSeconds;
}
    @property (nonatomic, assign) NSString* speechKey;
    @property (nonatomic, assign) NSString* serviceRegion;
    @property (nonatomic, assign) SPXTranslationRecognizer* translationRecognizer;
    @property (nonatomic, assign) SPXConnection* connection;
@end

@implementation SPXTranslationRecognitionEndToEndTest

- (void)setUp {
    [super setUp];
    timeoutInSeconds = 20.;
    weatherTextEnglish = @"What's the weather like?";
    weatherTextGerman = @"Wie ist das Wetter?";
    weatherTextChinese =  @"天气怎么样？";
    weatherFileName = @"whatstheweatherlike";
    
    self.speechKey = [[[NSProcessInfo processInfo] environment] objectForKey:@"subscriptionKey"] ;
    self.serviceRegion = [[[NSProcessInfo processInfo] environment] objectForKey:@"serviceRegion"] ;
    
    NSBundle *bundle = [NSBundle bundleForClass:[self class]];
    NSString *weatherFile = [bundle pathForResource: weatherFileName ofType:@"wav"];
    SPXAudioConfiguration* weatherAudioSource = [[SPXAudioConfiguration alloc] initWithWavFileInput:weatherFile];
    SPXSpeechTranslationConfiguration *translationConfig = [[SPXSpeechTranslationConfiguration alloc] initWithSubscription:self.speechKey region:self.serviceRegion];
    
    [translationConfig setSpeechRecognitionLanguage:@"en-us"];
    [translationConfig addTargetLanguage:@"de"];
    [translationConfig addTargetLanguage:@"zh-Hans"];
    [translationConfig setVoiceName:@"Microsoft Server Speech Text to Speech Voice (de-DE, Hedda)"];
    
    self.translationRecognizer = [[SPXTranslationRecognizer alloc] initWithSpeechTranslationConfiguration:translationConfig audioConfiguration:weatherAudioSource];
    self.connection = [[SPXConnection alloc] initFromRecognizer:self.translationRecognizer];

    __unsafe_unretained typeof(self) weakSelf = self;

    result = [NSMutableDictionary new];
    [result setObject:@0 forKey:@"finalResultCount"];
    [result setObject:@0 forKey:@"sessionStoppedCount"];
    [result setObject:@"" forKey:@"finalText"];

    [self.translationRecognizer addRecognizedEventHandler: ^ (SPXTranslationRecognizer *recognizer, SPXTranslationRecognitionEventArgs *eventArgs) {
        NSLog(@"Received final result event. SessionId: %@, recognition result:%@. Status %ld. offset %llu duration %llu resultid:%@", eventArgs.sessionId, eventArgs.result.text, (long)eventArgs.result.reason, eventArgs.result.offset, eventArgs.result.duration, eventArgs.result.resultId);
        NSLog(@"Received JSON: %@", [eventArgs.result.properties getPropertyById:SPXSpeechServiceResponseJsonResult]);
        [weakSelf->result setObject:eventArgs.result.text forKey: @"finalText"];
        [weakSelf->result setObject:[NSNumber numberWithLong:[[weakSelf->result objectForKey:@"finalResultCount"] integerValue] + 1] forKey:@"finalResultCount"];
    }];

    [self.translationRecognizer addSessionStartedEventHandler: ^ (SPXRecognizer *recognizer, SPXSessionEventArgs *eventArgs) {
        NSLog(@"Received session started event. SessionId: %@", eventArgs.sessionId);
    }];

    [self.translationRecognizer addSessionStoppedEventHandler: ^ (SPXRecognizer *recognizer, SPXSessionEventArgs *eventArgs) {
        [weakSelf->result setObject:[NSNumber numberWithLong:[[weakSelf->result objectForKey:@"sessionStoppedCount"] integerValue] + 1] forKey:@"sessionStoppedCount"];
    }];

    sessionStoppedCountPred = [NSPredicate predicateWithBlock: ^BOOL (id  _Nullable evaluatedObject, NSDictionary<NSString *,id> * _Nullable bindings){
        return [[evaluatedObject valueForKey:@"sessionStoppedCount"] isEqualToNumber:@1];
    }];
}

- (void)tearDown {
    [super tearDown];
}

- (void) testTranslateOnce {
    __block NSDictionary* translationDictionary = nil;
    __block int connectedEventCount = 0;
    __block int disconnectedEventCount = 0;

    [self.connection addConnectedEventHandler: ^ (SPXConnection* connection, SPXConnectionEventArgs* eventArgs) {
        connectedEventCount++;
    }];

    [self.connection addDisconnectedEventHandler: ^ (SPXConnection* connection, SPXConnectionEventArgs* eventArgs) {
        disconnectedEventCount++;
    }];

    SPXTranslationRecognitionResult *result = [self.translationRecognizer recognizeOnce];

    translationDictionary = result.translations;
    id germanTranslation = [translationDictionary valueForKey:@"de"];
    id chineseTranslation = [translationDictionary valueForKey:@"zh-Hans"];

    XCTAssertTrue([germanTranslation isEqualToString:weatherTextGerman], "German translation does not match");
    XCTAssertTrue([chineseTranslation isEqualToString:weatherTextChinese], "Chinese translation does not match");
    XCTAssertTrue(connectedEventCount > 0, @"The connected event count must be greater than 0. connectedEventCount=%d", connectedEventCount);
    XCTAssertTrue(connectedEventCount == disconnectedEventCount + 1 || connectedEventCount == disconnectedEventCount, @"The connected event count (%d) does not match the disconnected event count (%d)", connectedEventCount, disconnectedEventCount);
}

- (void) testContinuousTranslation {
    __block NSDictionary* translationDictionary = nil;
    __block bool end = false;
    __block int connectedEventCount = 0;
    __block int disconnectedEventCount = 0;

    [self.connection addConnectedEventHandler: ^ (SPXConnection* connection, SPXConnectionEventArgs* eventArgs)  {
        connectedEventCount++;
    }];

    [self.connection addDisconnectedEventHandler: ^ (SPXConnection* connection, SPXConnectionEventArgs* eventArgs) {
        disconnectedEventCount++;
    }];

    [self.translationRecognizer addRecognizedEventHandler: ^ (SPXTranslationRecognizer *recognizer, SPXTranslationRecognitionEventArgs *eventArgs)
     {
         translationDictionary = eventArgs.result.translations;
         end = true;
     }];

    [self.translationRecognizer startContinuousRecognition];

    [self expectationForPredicate:sessionStoppedCountPred evaluatedWithObject:self->result handler:nil];
    [self waitForExpectationsWithTimeout:timeoutInSeconds handler:nil];

    [self.translationRecognizer stopContinuousRecognition];
    
    id germanTranslation = [translationDictionary valueForKey:@"de"];
    id chineseTranslation = [translationDictionary valueForKey:@"zh-Hans"];

    XCTAssertTrue([germanTranslation isEqualToString:weatherTextGerman], "German translation does not match");
    XCTAssertTrue([chineseTranslation isEqualToString:weatherTextChinese], "Chinese translation does not match");
    XCTAssertTrue(connectedEventCount > 0, @"The connected event count must be greater than 0. connectedEventCount=%d", connectedEventCount);
    XCTAssertTrue(connectedEventCount == disconnectedEventCount + 1 || connectedEventCount == disconnectedEventCount, @"The connected event count (%d) does not match the disconnected event count (%d)", connectedEventCount, disconnectedEventCount);
}

- (void)testTranslateAsync {
    __block NSDictionary* translationDictionary = @{};
    __block int connectedEventCount = 0;
    __block int disconnectedEventCount = 0;

    [self.connection addConnectedEventHandler: ^ (SPXConnection* connection, SPXConnectionEventArgs* eventArgs) {
        connectedEventCount++;
    }];

    [self.connection addDisconnectedEventHandler: ^ (SPXConnection* connection, SPXConnectionEventArgs* eventArgs) {
        disconnectedEventCount++;
    }];

    [self.translationRecognizer recognizeOnceAsync: ^ (SPXTranslationRecognitionResult *result) {
        translationDictionary = result.translations;
        id germanTranslation = [translationDictionary valueForKey:@"de"];
        id chineseTranslation = [translationDictionary valueForKey:@"zh-Hans"];

        XCTAssertTrue(connectedEventCount > 0, @"The connected event count must be greater than 0. connectedEventCount=%d", connectedEventCount);
        XCTAssertTrue(connectedEventCount == disconnectedEventCount + 1 || connectedEventCount == disconnectedEventCount, @"The connected event count (%d) does not match the disconnected event count (%d)", connectedEventCount, disconnectedEventCount);
        XCTAssertTrue([germanTranslation isEqualToString:self->weatherTextGerman], "German translation does not match");
        XCTAssertTrue([chineseTranslation isEqualToString:self->weatherTextChinese], "Chinese translation does not match");
     }];
}
@end


@interface SPXSpeechTranslationTest : XCTestCase {
}
@property (nonatomic, assign) NSString * speechKey;
@property (nonatomic, assign) NSString * serviceRegion;
@end


@implementation SPXSpeechTranslationTest

- (void)setUp {
    [super setUp];
    
    self.speechKey = [[[NSProcessInfo processInfo] environment] objectForKey:@"subscriptionKey"];
    self.serviceRegion = [[[NSProcessInfo processInfo] environment] objectForKey:@"serviceRegion"];
}

- (void)testInvalidSubscriptionKey {
    NSString *weatherFileName = @"whatstheweatherlike";
    
    NSBundle *bundle = [NSBundle bundleForClass:[self class]];
    NSString *weatherFile = [bundle pathForResource: weatherFileName ofType:@"wav"];
    
    SPXAudioConfiguration* weatherAudioSource = [[SPXAudioConfiguration alloc] initWithWavFileInput:weatherFile];
    
    SPXSpeechTranslationConfiguration *translationConfig = [[SPXSpeechTranslationConfiguration alloc] initWithSubscription:@"YourSubscriptionKey" region:@"westus"];
    XCTAssertNotNil(translationConfig);
    [translationConfig setSpeechRecognitionLanguage:@"en-us"];
    [translationConfig addTargetLanguage:@"de-DE"];
    
    // this shouldn't throw, but fail on opening the connection
    SPXTranslationRecognizer *translationRecognizer = [[SPXTranslationRecognizer alloc] initWithSpeechTranslationConfiguration:translationConfig audioConfiguration:weatherAudioSource];
    XCTAssertNotNil(translationRecognizer);
}

- (void)testInvalidRegion {
    NSString *weatherFileName = @"whatstheweatherlike";
    
    NSBundle *bundle = [NSBundle bundleForClass:[self class]];
    NSString *weatherFile = [bundle pathForResource: weatherFileName ofType:@"wav"];
    
    SPXAudioConfiguration* weatherAudioSource = [[SPXAudioConfiguration alloc] initWithWavFileInput:weatherFile];
    
    SPXSpeechTranslationConfiguration *translationConfig = [[SPXSpeechTranslationConfiguration alloc] initWithSubscription:self.speechKey region:@"YourServiceRegion"];
    XCTAssertNotNil(translationConfig);
    [translationConfig setSpeechRecognitionLanguage:@"en-us"];
    [translationConfig addTargetLanguage:@"de-DE"];
    
    // this shouldn't throw, but fail on opening the connection
    SPXTranslationRecognizer *translationRecognizer = [[SPXTranslationRecognizer alloc] initWithSpeechTranslationConfiguration:translationConfig audioConfiguration:weatherAudioSource];
    XCTAssertNotNil(translationRecognizer);
}

- (void)testEmptyRegion {
    SPXSpeechTranslationConfiguration *translationConfig = [[SPXSpeechTranslationConfiguration alloc] initWithSubscription:@"YourSubscriptionKey" region:@""];
    XCTAssertNil(translationConfig);
}

- (void)testEmptyKey {
    SPXSpeechTranslationConfiguration *translationConfig = [[SPXSpeechTranslationConfiguration alloc] initWithSubscription:@"" region:@"westus"];
    XCTAssertNil(translationConfig);
}

- (void)testInvalidWavefileName {
    SPXAudioConfiguration* invalidAudioSource = [[SPXAudioConfiguration alloc] initWithWavFileInput:@"invalidFilename"];
    
    SPXSpeechTranslationConfiguration *translationConfig = [[SPXSpeechTranslationConfiguration alloc] initWithSubscription:@"InvalidKey" region:@"westus"];
    XCTAssertNotNil(translationConfig);
    
    SPXTranslationRecognizer *translationRecognizer = [[SPXTranslationRecognizer alloc] initWithSpeechTranslationConfiguration:translationConfig audioConfiguration:invalidAudioSource];
    XCTAssertNil(translationRecognizer);
}

- (void)testFromEndpointWithoutKeyAndToken {
    NSString *weatherFileName = @"whatstheweatherlike";
    NSBundle *bundle = [NSBundle bundleForClass:[self class]];
    NSString *weatherFile = [bundle pathForResource: weatherFileName ofType:@"wav"];

    NSString *endpoint = [NSString stringWithFormat:@"wss://%@.s2s.speech.microsoft.com/speech/translationition/cognitiveservices/v1", self.serviceRegion];
    SPXAudioConfiguration* weatherAudioSource = [[SPXAudioConfiguration alloc] initWithWavFileInput:weatherFile];
    SPXSpeechTranslationConfiguration* translationConfig = [[SPXSpeechTranslationConfiguration alloc] initWithEndpoint:endpoint];
    XCTAssertNotNil(translationConfig);
    [translationConfig setSpeechRecognitionLanguage:@"en-us"];
    [translationConfig addTargetLanguage:@"de-DE"];

    SPXTranslationRecognizer *translationRecognizer = [[SPXTranslationRecognizer alloc] initWithSpeechTranslationConfiguration:translationConfig audioConfiguration:weatherAudioSource];
    XCTAssertNotNil(translationRecognizer);
    // We cannot really test whether recognizer works, since there is no test endpoint available which supports no authentication.
    XCTAssertTrue([[translationRecognizer.properties getPropertyById:SPXSpeechServiceConnectionKey] length] == 0);
    XCTAssertTrue([translationRecognizer.authorizationToken length] == 0);
}

- (void)testSetServicePropertyTranslation {
    NSString *weatherFileName = @"whatstheweatherlike";
    NSString *weatherTextEnglish = @"What's the weather like?";
    NSString *weatherTextGerman = @"Wie ist das Wetter?";
    NSBundle *bundle = [NSBundle bundleForClass:[self class]];
    NSString *weatherFile = [bundle pathForResource: weatherFileName ofType:@"wav"];
    SPXAudioConfiguration* weatherAudioSource = [[SPXAudioConfiguration alloc] initWithWavFileInput:weatherFile];
    SPXSpeechTranslationConfiguration *translationConfig = [[SPXSpeechTranslationConfiguration alloc] initWithSubscription:self.speechKey region:self.serviceRegion];
    translationConfig.speechRecognitionLanguage = @"en-us";
    [translationConfig setServicePropertyTo:@"de" byName:@"to" usingChannel:SPXServicePropertyChannel_UriQueryParameter];

    SPXTranslationRecognizer *translationRecognizer = [[SPXTranslationRecognizer alloc] initWithSpeechTranslationConfiguration:translationConfig audioConfiguration:weatherAudioSource];
    SPXTranslationRecognitionResult *result = [translationRecognizer recognizeOnce];
    XCTAssertTrue(result.reason == SPXResultReason_TranslatedSpeech);
    XCTAssertTrue([result.text isEqualToString:weatherTextEnglish], "Final Result Text does not match");
    NSDictionary* translationDictionary = result.translations;
    id germanTranslation = [translationDictionary valueForKey:@"de"];
    XCTAssertTrue([germanTranslation isEqualToString:weatherTextGerman], "German translation does not match");
}
@end
