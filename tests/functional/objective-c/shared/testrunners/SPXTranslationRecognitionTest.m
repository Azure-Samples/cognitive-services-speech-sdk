//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import <XCTest/XCTest.h>
#import <MicrosoftCognitiveServicesSpeech/SPXSpeechApi.h>
#import "SPXTestHelpers.h"

@interface SPXTranslationRecognitionEndToEndTest : XCTestCase {
    NSString *weatherTextEnglish;
    NSString *weatherTextGerman;
    NSString *weatherTextChinese;
    NSString *normalizedWeatherTextEnglish;
    NSString *normalizedWeatherTextGerman;
    NSString *normalizedWeatherTextChinese;

    NSString *weatherFileName;
    NSMutableDictionary *result;

    NSPredicate *sessionStoppedCountPred;

    int connectedEventCount;
    int disconnectedEventCount;


    double timeoutInSeconds;
}
    @property (nonatomic, assign) NSString* speechKey;
    @property (nonatomic, assign) NSString* serviceRegion;
    @property (nonatomic, retain) SPXTranslationRecognizer* translationRecognizer;
    @property (nonatomic, retain) SPXConnection* connection;
@end

@implementation SPXTranslationRecognitionEndToEndTest

- (void)setUp {
    [super setUp];
    timeoutInSeconds = 20.;
    weatherTextEnglish = @"What's the weather like?";
    weatherTextGerman = @"Wie ist das Wetter?";
    weatherTextChinese =  @"天气怎么样?";
    normalizedWeatherTextGerman = [SPXTestHelpers normalizeText:weatherTextGerman];
    normalizedWeatherTextGerman = [SPXTestHelpers normalizeText:weatherTextGerman];
    normalizedWeatherTextEnglish = [SPXTestHelpers normalizeText:weatherTextEnglish];
    normalizedWeatherTextChinese = [SPXTestHelpers normalizeText:weatherTextChinese];

    weatherFileName = @"whatstheweatherlike";
    connectedEventCount = 0;
    disconnectedEventCount = 0;

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

    [self.connection addConnectedEventHandler: ^ (SPXConnection* connection, SPXConnectionEventArgs* eventArgs) {
        weakSelf->connectedEventCount++;
    }];

    [self.connection addDisconnectedEventHandler: ^ (SPXConnection* connection, SPXConnectionEventArgs* eventArgs) {
        weakSelf->disconnectedEventCount++;
    }];

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
        NSLog(@"Received session stopped event. SessionId: %@", eventArgs.sessionId);
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

    SPXTranslationRecognitionResult *result = [self.translationRecognizer recognizeOnce];

    translationDictionary = result.translations;

    id germanTranslation = [SPXTestHelpers normalizeText:[translationDictionary valueForKey:@"de"]];
    id chineseTranslation = [SPXTestHelpers normalizeText:[translationDictionary valueForKey:@"zh-Hans"]];

    NSLog(@"German Translation: %@", germanTranslation);
    NSLog(@"Chinese Translation: %@", chineseTranslation);

    XCTAssertEqualObjects(germanTranslation, normalizedWeatherTextGerman);
    XCTAssertEqualObjects(chineseTranslation, normalizedWeatherTextChinese);
    XCTAssertTrue(connectedEventCount > 0, @"The connected event count must be greater than 0. connectedEventCount=%d", connectedEventCount);
    XCTAssertTrue(connectedEventCount == disconnectedEventCount + 1 || connectedEventCount == disconnectedEventCount, @"The connected event count (%d) does not match the disconnected event count (%d)", connectedEventCount, disconnectedEventCount);
}

- (void) testContinuousTranslation {
    __block NSDictionary* translationDictionary = nil;

    [self.translationRecognizer addRecognizedEventHandler: ^ (SPXTranslationRecognizer *recognizer, SPXTranslationRecognitionEventArgs *eventArgs)
     {
         translationDictionary = eventArgs.result.translations;
     }];

    [self.translationRecognizer startContinuousRecognition];

    [self expectationForPredicate:sessionStoppedCountPred evaluatedWithObject:self->result handler:nil];
    [self waitForExpectationsWithTimeout:timeoutInSeconds handler:nil];

    [self.translationRecognizer stopContinuousRecognition];

    id germanTranslation = [SPXTestHelpers normalizeText:[translationDictionary valueForKey:@"de"]];
    id chineseTranslation = [SPXTestHelpers normalizeText:[translationDictionary valueForKey:@"zh-Hans"]];
    NSLog(@"German Translation: %@", germanTranslation);
    NSLog(@"Chinese Translation: %@", chineseTranslation);

    XCTAssertEqualObjects(germanTranslation, normalizedWeatherTextGerman);
    XCTAssertEqualObjects(chineseTranslation, normalizedWeatherTextChinese);
    XCTAssertTrue(connectedEventCount > 0, @"The connected event count must be greater than 0. connectedEventCount=%d", connectedEventCount);
    XCTAssertTrue(connectedEventCount == disconnectedEventCount + 1 || connectedEventCount == disconnectedEventCount, @"The connected event count (%d) does not match the disconnected event count (%d)", connectedEventCount, disconnectedEventCount);
}

- (void)testTranslateAsync {
    __block SPXTranslationRecognitionResult * asyncResult;

    [self.translationRecognizer recognizeOnceAsync: ^ (SPXTranslationRecognitionResult *result) {
        asyncResult = result;
     }];

    [self expectationForPredicate:sessionStoppedCountPred evaluatedWithObject:result handler:nil];
    [self waitForExpectationsWithTimeout:timeoutInSeconds handler:nil];
    id germanTranslation = [SPXTestHelpers normalizeText:[asyncResult.translations valueForKey:@"de"]];
    id chineseTranslation = [SPXTestHelpers normalizeText:[asyncResult.translations valueForKey:@"zh-Hans"]];
    NSLog(@"German Translation: %@", germanTranslation);
    NSLog(@"Chinese Translation: %@", chineseTranslation);

    XCTAssertEqualObjects(germanTranslation, normalizedWeatherTextGerman);
    XCTAssertEqualObjects(chineseTranslation, normalizedWeatherTextChinese);

    XCTAssertTrue(self->connectedEventCount > 0, @"The connected event count must be greater than 0. connectedEventCount=%d", self->connectedEventCount);
    XCTAssertTrue(self->connectedEventCount == self->disconnectedEventCount + 1 || self->connectedEventCount == self->disconnectedEventCount, @"The connected event count (%d) does not match the disconnected event count (%d)", self->connectedEventCount, self->disconnectedEventCount);
}

- (void)testContinuousTranslationWithError {
    __block NSDictionary* translationDictionary = nil;

    [self.translationRecognizer addRecognizedEventHandler: ^ (SPXTranslationRecognizer *recognizer, SPXTranslationRecognitionEventArgs *eventArgs)
     {
         translationDictionary = eventArgs.result.translations;
     }];

    NSError * err = nil;
    BOOL success = [self.translationRecognizer startContinuousRecognition:&err];
    XCTAssertTrue(success);
    XCTAssertNil(err);

    [self expectationForPredicate:sessionStoppedCountPred evaluatedWithObject:result handler:nil];
    [self waitForExpectationsWithTimeout:timeoutInSeconds handler:nil];

    XCTAssertEqualObjects([self->result valueForKey:@"finalText"], self->weatherTextEnglish);
    XCTAssertEqualObjects([self->result valueForKey:@"finalResultCount"], @1);

    XCTAssertEqualObjects([SPXTestHelpers normalizeText:[translationDictionary valueForKey:@"de"]], normalizedWeatherTextGerman);
    XCTAssertEqualObjects([SPXTestHelpers normalizeText:[translationDictionary valueForKey:@"zh-Hans"]], normalizedWeatherTextChinese);

    XCTAssertGreaterThan(connectedEventCount, 0);
    XCTAssertTrue(connectedEventCount == disconnectedEventCount + 1 || connectedEventCount == disconnectedEventCount,
                  @"The connected event count ( %d ) does not match the disconnected event count ( %d )", connectedEventCount, disconnectedEventCount);

    err = nil;
    success = [self.translationRecognizer stopContinuousRecognition:&err];
    XCTAssertTrue(success);
    XCTAssertNil(err);

}

- (void)testRecognizeAsyncWithError {
    __block SPXTranslationRecognitionResult * asyncResult;
    NSError * err = nil;
    BOOL success = [self.translationRecognizer recognizeOnceAsync: ^ (SPXTranslationRecognitionResult *srresult) {
        asyncResult = srresult;
    } error:&err];
    XCTAssertTrue(success);
    XCTAssertNil(err);

    [self expectationForPredicate:sessionStoppedCountPred evaluatedWithObject:result handler:nil];
    [self waitForExpectationsWithTimeout:timeoutInSeconds handler:nil];

    id germanTranslation = [SPXTestHelpers normalizeText:[asyncResult.translations valueForKey:@"de"]];
    id chineseTranslation = [SPXTestHelpers normalizeText:[asyncResult.translations valueForKey:@"zh-Hans"]];

    XCTAssertEqualObjects(germanTranslation, normalizedWeatherTextGerman);
    XCTAssertEqualObjects(chineseTranslation, normalizedWeatherTextChinese);
}

- (void)testRecognizeOnceWithError {
    NSError * err = nil;
    SPXTranslationRecognitionResult *result = [self.translationRecognizer recognizeOnce:&err];
    XCTAssertNotNil(result);
    XCTAssertNil(err);

    NSLog(@"recognition result:%@. Status %ld. offset %llu duration %llu resultid:%@",
          result.text, (long)result.reason, result.offset, result.duration, result.resultId);

    XCTAssertEqualObjects(result.text, weatherTextEnglish);
    XCTAssertEqualObjects([SPXTestHelpers normalizeText:[result.translations valueForKey:@"de"]], normalizedWeatherTextGerman);
    XCTAssertEqualObjects([SPXTestHelpers normalizeText:[result.translations valueForKey:@"zh-Hans"]], normalizedWeatherTextChinese);
    XCTAssertEqual(result.reason, SPXResultReason_TranslatedSpeech);
    XCTAssertGreaterThan(result.duration, 0);
    XCTAssertGreaterThan(result.offset, 0);
    XCTAssertGreaterThan([result.resultId length], 0);
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
    XCTAssertThrowsSpecificNamed([[SPXSpeechTranslationConfiguration alloc] initWithSubscription:@"InvalidSubscriptionKey" region:@""], NSException, @"SPXException");
}

- (void)testEmptyRegionWithError {
    NSError * err = nil;
    SPXSpeechTranslationConfiguration *speechConfig = [[SPXSpeechTranslationConfiguration alloc] initWithSubscription:@"InvalidSubscriptionKey" region:@"" error:&err];
    NSLog(@"Received Error: %@", err);
    XCTAssertNil(speechConfig);
    XCTAssertEqual(err.code, 0x5);
}

- (void)testEmptyKey {
    XCTAssertThrowsSpecificNamed([[SPXSpeechTranslationConfiguration alloc] initWithSubscription:@"" region:@"westus"], NSException, @"SPXException");
}

- (void)testEmptyKeyWithError {
    NSError * err = nil;
    SPXSpeechTranslationConfiguration *speechConfig = [[SPXSpeechTranslationConfiguration alloc] initWithSubscription:@"" region:@"westus" error:&err];
    XCTAssertNil(speechConfig);
    NSLog(@"Received Error: %@", err);
    XCTAssert([err.description containsString:@"SPXERR_INVALID_ARG"]);
    XCTAssertEqual(err.code, 0x5);
}

- (void)testInvalidWavefileName {
    SPXAudioConfiguration* invalidAudioSource = [[SPXAudioConfiguration alloc] initWithWavFileInput:@"invalidFilename"];

    SPXSpeechTranslationConfiguration *speechConfig = [[SPXSpeechTranslationConfiguration alloc] initWithSubscription:@"InvalidKey" region:@"westus"];
    XCTAssertNotNil(speechConfig);

    XCTAssertThrows([[SPXTranslationRecognizer alloc] initWithSpeechTranslationConfiguration:speechConfig audioConfiguration:invalidAudioSource]);
}

- (void)testInvalidWavefileNameWithError {
    SPXAudioConfiguration* invalidAudioSource = [[SPXAudioConfiguration alloc] initWithWavFileInput:@"invalidFilename"];

    SPXSpeechTranslationConfiguration *speechConfig = [[SPXSpeechTranslationConfiguration alloc] initWithSubscription:@"InvalidKey" region:@"westus"];
    XCTAssertNotNil(speechConfig);

    NSError * err = nil;
    SPXTranslationRecognizer * speechRecognizer = [[SPXTranslationRecognizer alloc] initWithSpeechTranslationConfiguration:speechConfig audioConfiguration:invalidAudioSource error:&err];
    XCTAssertNil(speechRecognizer);
    XCTAssert([err.description containsString:@"SPXERR_FILE_OPEN_FAILED"]);
    XCTAssertEqual(err.code, 0x8);
}

- (void)testFromEndpointWithoutKeyAndToken {
    NSString *weatherFileName = @"whatstheweatherlike";
    NSBundle *bundle = [NSBundle bundleForClass:[self class]];
    NSString *weatherFile = [bundle pathForResource: weatherFileName ofType:@"wav"];

    NSString *endpoint = [NSString stringWithFormat:@"wss://%@.s2s.speech.microsoft.com/speech/translation/cognitiveservices/v1", self.serviceRegion];
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

- (void)testFromHostWithoutKeyAndToken {
    NSString *weatherFileName = @"whatstheweatherlike";
    NSBundle *bundle = [NSBundle bundleForClass:[self class]];
    NSString *weatherFile = [bundle pathForResource: weatherFileName ofType:@"wav"];
    NSString *host = [NSString stringWithFormat:@"wss://%@.s2s.speech.microsoft.com", self.serviceRegion];

    SPXAudioConfiguration* weatherAudioSource = [[SPXAudioConfiguration alloc] initWithWavFileInput:weatherFile];
    SPXSpeechTranslationConfiguration* translationConfig = [[SPXSpeechTranslationConfiguration alloc] initWithHost:host];
    XCTAssertNotNil(translationConfig);
    [translationConfig setSpeechRecognitionLanguage:@"en-us"];
    [translationConfig addTargetLanguage:@"de-DE"];

    SPXTranslationRecognizer *translationRecognizer = [[SPXTranslationRecognizer alloc] initWithSpeechTranslationConfiguration:translationConfig audioConfiguration:weatherAudioSource];
    XCTAssertNotNil(translationRecognizer);
    // We cannot really test whether recognizer works, since there is no non-container test host available which supports no authentication.
    XCTAssertTrue([[translationRecognizer.properties getPropertyById:SPXSpeechServiceConnectionKey] length] == 0);
    XCTAssertTrue([translationRecognizer.authorizationToken length] == 0);
}

- (void)testTranslationFromHost {
    NSString *weatherFileName = @"whatstheweatherlike";
    NSBundle *bundle = [NSBundle bundleForClass:[self class]];
    NSString *weatherFile = [bundle pathForResource: weatherFileName ofType:@"wav"];
    NSString *host = [NSString stringWithFormat:@"wss://%@.s2s.speech.microsoft.com", self.serviceRegion];
    
    SPXAudioConfiguration* weatherAudioSource = [[SPXAudioConfiguration alloc] initWithWavFileInput:weatherFile];
    SPXSpeechTranslationConfiguration *translationConfig = [[SPXSpeechTranslationConfiguration alloc] initWithHost:host subscription:self.speechKey];
    XCTAssertNotNil(translationConfig);
    [translationConfig setSpeechRecognitionLanguage:@"en-us"];
    [translationConfig addTargetLanguage:@"de"];

    SPXTranslationRecognizer *translationRecognizer = [[SPXTranslationRecognizer alloc] initWithSpeechTranslationConfiguration:translationConfig audioConfiguration:weatherAudioSource];
    SPXTranslationRecognitionResult *result = [translationRecognizer recognizeOnce];
    XCTAssertEqual(result.reason, SPXResultReason_TranslatedSpeech);
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
    NSLog(@"German Translation: %@", germanTranslation);
    XCTAssertTrue([germanTranslation isEqualToString:weatherTextGerman], "German translation does not match");
}

- (void)testChangeTargetLanguages {
    NSString *weatherFileName = @"whatstheweatherlike";
    NSString *weatherTextEnglish = @"What's the weather like?";
    NSBundle *bundle = [NSBundle bundleForClass:[self class]];
    NSString *weatherFile = [bundle pathForResource: weatherFileName ofType:@"wav"];
    SPXAudioConfiguration* weatherAudioSource = [[SPXAudioConfiguration alloc] initWithWavFileInput:weatherFile];
    SPXSpeechTranslationConfiguration *translationConfig = [[SPXSpeechTranslationConfiguration alloc] initWithSubscription:self.speechKey region:self.serviceRegion];
    [translationConfig setSpeechRecognitionLanguage:@"en-us"];
    [translationConfig addTargetLanguage:@"de"];
    [translationConfig removeTargetLanguage:@"de"];

    SPXTranslationRecognizer *translationRecognizer = [[SPXTranslationRecognizer alloc] initWithSpeechTranslationConfiguration:translationConfig audioConfiguration:weatherAudioSource];
    NSArray *targetLangList = [translationRecognizer targetLanguages];
    XCTAssertEqual(0, [targetLangList count], @"target language list must be empty.");
    XCTAssertTrue([[[translationRecognizer properties] getPropertyById:SPXSpeechServiceConnectionTranslationToLanguages] isEqualToString:@""], @"target languages in property should be empty.");
    [translationRecognizer addTargetLanguage:@"nl"];
    [translationRecognizer addTargetLanguage:@"fr"];
    [translationRecognizer removeTargetLanguage:@"nl"];
    targetLangList = [translationRecognizer targetLanguages];
    XCTAssertEqual(1, [targetLangList count], @"target language list must have 1 element.");
    XCTAssertTrue([[targetLangList firstObject] isEqualToString:@"fr"], @"target languages does not match.");

    SPXTranslationRecognitionResult *result = [translationRecognizer recognizeOnce];
    XCTAssertTrue(result.reason == SPXResultReason_TranslatedSpeech);
    NSDictionary* translationDictionary = result.translations;
    id frenchTranslation = [translationDictionary valueForKey:@"fr"];
    NSLog(@"French Translation: %@", frenchTranslation);
    XCTAssertTrue([frenchTranslation length] > 0, @"French translation should not be empty");
    XCTAssertTrue([translationDictionary objectForKey:@"de"] == nil, @"German translation must be empty");
    XCTAssertEqual(1, [translationDictionary count], @"Number of translations must be 1.");
}

@end
