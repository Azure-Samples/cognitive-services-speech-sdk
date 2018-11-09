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
}
    @property (nonatomic, assign) NSString * speechKey;
    @property (nonatomic, assign) NSString * serviceRegion;
    @property (nonatomic, assign) SPXTranslationRecognizer* translationRecognizer;
@end

@implementation SPXTranslationRecognitionTest

- (void)setUp {
    [super setUp];
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
    __block bool end = false;
    
    [self.translationRecognizer addRecognizedEventHandler: ^ (SPXTranslationRecognizer *recognizer, SPXTranslationRecognitionEventArgs *eventArgs)
     {
         translationDictionary = eventArgs.result.translations;
         end = true;
     }];
    
    [self.translationRecognizer startContinuousRecognition];
    while (end == false)
        [NSThread sleepForTimeInterval:1.0f];
    [self.translationRecognizer stopContinuousRecognition];
    
    id germanTranslation = [translationDictionary valueForKey:@"de"];
    id chineseTranslation = [translationDictionary valueForKey:@"zh-Hans"];
    
    XCTAssertTrue([germanTranslation isEqualToString:weatherTextGerman], "German translation does not match");
    XCTAssertTrue([chineseTranslation isEqualToString:weatherTextChinese], "Chinese translation does not match");
}

- (void)testTranslateAsync {
    __block int finalResultCount = 0;
    __block NSDictionary* translationDictionary = @{};
    
    [self.translationRecognizer addRecognizedEventHandler:^(SPXTranslationRecognizer * recognizer, SPXTranslationRecognitionEventArgs * eventArgs) {
         finalResultCount++;
     }];
    
    [self.translationRecognizer recognizeOnceAsync: ^ (SPXTranslationRecognitionResult *result) {
         translationDictionary = result.translations;
         id germanTranslation = [translationDictionary valueForKey:@"de"];
         id chineseTranslation = [translationDictionary valueForKey:@"zh-Hans"];
         
         XCTAssertTrue([germanTranslation isEqualToString:weatherTextGerman], "German translation does not match");
         XCTAssertTrue([chineseTranslation isEqualToString:weatherTextChinese], "Chinese translation does not match");
     }];
}


@end
