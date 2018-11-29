//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import <XCTest/XCTest.h>
#import <MicrosoftCognitiveServicesSpeech/SPXSpeechApi.h>


@interface SPXTranslationRecognitionTest : XCTestCase{
    NSString *weatherTextEnglish;
    NSString *weatherTextGerman;
    NSString *weatherTextChinese;
    NSString *weatherFileName;
    NSMutableDictionary *result;

    NSPredicate *sessionStoppedCountPred;

    double timeoutInSeconds;
}
    @property (nonatomic, assign) NSString * speechKey;
    @property (nonatomic, assign) NSString * serviceRegion;
    @property (nonatomic, assign) SPXTranslationRecognizer* translationRecognizer;
@end

@implementation SPXTranslationRecognitionTest

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

    SPXTranslationRecognitionResult *result = [self.translationRecognizer recognizeOnce];
    
    translationDictionary = result.translations;
    id germanTranslation = [translationDictionary valueForKey:@"de"];
    id chineseTranslation = [translationDictionary valueForKey:@"zh-Hans"];
    
    XCTAssertTrue([germanTranslation isEqualToString:weatherTextGerman], "German translation does not match");
    XCTAssertTrue([chineseTranslation isEqualToString:weatherTextChinese], "Chinese translation does not match");
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
    
    id germanTranslation = [translationDictionary valueForKey:@"de"];
    id chineseTranslation = [translationDictionary valueForKey:@"zh-Hans"];
    
    XCTAssertTrue([germanTranslation isEqualToString:weatherTextGerman], "German translation does not match");
    XCTAssertTrue([chineseTranslation isEqualToString:weatherTextChinese], "Chinese translation does not match");
}

- (void)testTranslateAsync {
    __block NSDictionary* translationDictionary = @{};
    
    [self.translationRecognizer recognizeOnceAsync: ^ (SPXTranslationRecognitionResult *result) {
         translationDictionary = result.translations;
         id germanTranslation = [translationDictionary valueForKey:@"de"];
         id chineseTranslation = [translationDictionary valueForKey:@"zh-Hans"];
         
         XCTAssertTrue([germanTranslation isEqualToString:self->weatherTextGerman], "German translation does not match");
         XCTAssertTrue([chineseTranslation isEqualToString:self->weatherTextChinese], "Chinese translation does not match");
     }];
}


@end
