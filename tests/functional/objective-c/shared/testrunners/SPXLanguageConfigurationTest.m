//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import <XCTest/XCTest.h>
#import <AVFoundation/AVFoundation.h>
#import <MicrosoftCognitiveServicesSpeech/SPXSpeechApi.h>
@interface SPXLanguageConfigurationTest : XCTestCase {
    // the file/transcription to use
    NSString *targetFileName;
    NSString *targetTranscription;
}

@property (nonatomic, assign) NSString * speechKey;
@property (nonatomic, assign) NSString * serviceRegion;
@property (nonatomic, retain) SPXSpeechConfiguration* speechConfig;
@property (nonatomic, retain) SPXAudioConfiguration* audioConfig;
@end

@implementation SPXLanguageConfigurationTest

- (void)setUp {
    self.speechKey = [[[NSProcessInfo processInfo] environment] objectForKey:@"subscriptionKey"];
    self.serviceRegion = [[[NSProcessInfo processInfo] environment] objectForKey:@"serviceRegion"];
    self.speechConfig = [[SPXSpeechConfiguration alloc] initWithSubscription:self.speechKey region:self.serviceRegion];
    
    targetFileName = @"whatstheweatherlike";
    targetTranscription = @"What's the weather like?";
    NSBundle *bundle = [NSBundle bundleForClass:[self class]];
    NSString *targetFile = [bundle pathForResource: targetFileName ofType:@"wav"];
    self.audioConfig = [[SPXAudioConfiguration alloc] initWithWavFileInput:targetFile];
}

- (void)tearDown {
    [super tearDown];
    
}

- (void)testCreateSourceLanguageConfig {
    NSString *language = @"zh-CN"; 
    SPXSpeechRecognizer* speechRecognizer = [[SPXSpeechRecognizer alloc] initWithSpeechConfiguration:self.speechConfig language:language audioConfiguration:self.audioConfig];
    XCTAssertNotNil(speechRecognizer);
    XCTAssertEqualObjects(language, [speechRecognizer.properties getPropertyById:SPXSpeechServiceConnectionRecognitionLanguage]);

    SPXSourceLanguageConfiguration* sourceLanguageConfig = [[SPXSourceLanguageConfiguration alloc]init:language];
    speechRecognizer = [[SPXSpeechRecognizer alloc] initWithSpeechConfiguration:self.speechConfig sourceLanguageConfiguration:sourceLanguageConfig audioConfiguration:self.audioConfig];
    XCTAssertNotNil(speechRecognizer);
    XCTAssertEqualObjects(language, [speechRecognizer.properties getPropertyById:SPXSpeechServiceConnectionRecognitionLanguage]);

    NSString *testEndpointId = @"myendpoint";
    sourceLanguageConfig = [[SPXSourceLanguageConfiguration alloc]initWithLanguage:language endpointId:testEndpointId];
    speechRecognizer = [[SPXSpeechRecognizer alloc] initWithSpeechConfiguration:self.speechConfig sourceLanguageConfiguration:sourceLanguageConfig audioConfiguration:self.audioConfig];
    XCTAssertNotNil(speechRecognizer);
    XCTAssertEqualObjects(language, [speechRecognizer.properties getPropertyById:SPXSpeechServiceConnectionRecognitionLanguage]);
    XCTAssertEqualObjects(testEndpointId, [speechRecognizer.properties getPropertyById:SPXSpeechServiceConnectionEndpointId]);
}

- (void)testCreateAutoDetectSourceLanguageConfig {
    NSArray *languages = @[@"zh-CN", @"de-DE"];
    SPXAutoDetectSourceLanguageConfiguration* autoDetectSourceLanguageConfig = [[SPXAutoDetectSourceLanguageConfiguration alloc]init:languages];
    SPXSpeechRecognizer* speechRecognizer = [[SPXSpeechRecognizer alloc] initWithSpeechConfiguration:self.speechConfig 
                                                                         autoDetectSourceLanguageConfiguration:autoDetectSourceLanguageConfig 
                                                                         audioConfiguration:self.audioConfig];
    XCTAssertNotNil(speechRecognizer);
    XCTAssertEqualObjects(@"zh-CN,de-DE", [speechRecognizer.properties getPropertyById:SPXSpeechServiceConnectionAutoDetectSourceLanguages]);
    XCTAssertEqualObjects(@"", [speechRecognizer.properties getPropertyByName:@"zh-CNSPEECH-ModelId"]);
    XCTAssertEqualObjects(@"", [speechRecognizer.properties getPropertyByName:@"de-DESPEECH-ModelId"]);
 
    NSString *zhLanguage = @"zh-CN"; 
    NSString *deLanguage = @"de-DE";
    NSString *testEndpointId = @"myendpoint";
    SPXSourceLanguageConfiguration* zhSourceLanguageConfig = [[SPXSourceLanguageConfiguration alloc]init:zhLanguage];
    SPXSourceLanguageConfiguration* deSourceLanguageConfig = [[SPXSourceLanguageConfiguration alloc]initWithLanguage:deLanguage endpointId:testEndpointId];
    NSArray *languageConfigs = @[zhSourceLanguageConfig, deSourceLanguageConfig];
    autoDetectSourceLanguageConfig =
        [[SPXAutoDetectSourceLanguageConfiguration alloc]initWithSourceLanguageConfigurations:languageConfigs];
    speechRecognizer = [[SPXSpeechRecognizer alloc] initWithSpeechConfiguration:self.speechConfig 
                                                    autoDetectSourceLanguageConfiguration:autoDetectSourceLanguageConfig 
                                                    audioConfiguration:self.audioConfig];
    XCTAssertNotNil(speechRecognizer);
    XCTAssertEqualObjects(@"zh-CN,de-DE", [speechRecognizer.properties getPropertyById:SPXSpeechServiceConnectionAutoDetectSourceLanguages]);
    XCTAssertEqualObjects(@"", [speechRecognizer.properties getPropertyByName:@"zh-CNSPEECH-ModelId"]);
    XCTAssertEqualObjects(@"myendpoint", [speechRecognizer.properties getPropertyByName:@"de-DESPEECH-ModelId"]);

    autoDetectSourceLanguageConfig = [[SPXAutoDetectSourceLanguageConfiguration alloc]initWithOpenRange];
    SPXSpeechSynthesizer* speechSynthesizer = [[SPXSpeechSynthesizer alloc] initWithSpeechConfiguration:self.speechConfig
                                                                         autoDetectSourceLanguageConfiguration:autoDetectSourceLanguageConfig
                                                                         audioConfiguration:nil];
    XCTAssertNotNil(speechSynthesizer);
    XCTAssertEqualObjects(@"open_range", [speechSynthesizer.properties getPropertyById:SPXSpeechServiceConnectionAutoDetectSourceLanguages]);
}

- (void)testCreateSrcLangConfigDifferentParams {
    NSError * err = nil;
    SPXSourceLanguageConfiguration* sourceLanguageConfig = [[SPXSourceLanguageConfiguration alloc]init:nil error:&err];
    XCTAssertNil(sourceLanguageConfig);
    XCTAssertNotNil(err);
    XCTAssertEqual(err.code, 0x5);

    err = nil;
    sourceLanguageConfig = [[SPXSourceLanguageConfiguration alloc]init:@"" error:&err];
    XCTAssertNil(sourceLanguageConfig);
    XCTAssertNotNil(err);
    XCTAssertEqual(err.code, 0x5);

    err = nil;
    sourceLanguageConfig = [[SPXSourceLanguageConfiguration alloc]init:@"zh-CN" error:&err];
    XCTAssertNotNil(sourceLanguageConfig);
    XCTAssertNil(err);

    err = nil;
    sourceLanguageConfig = [[SPXSourceLanguageConfiguration alloc]initWithLanguage:@"zh-CN" endpointId:nil error:&err];
    XCTAssertNil(sourceLanguageConfig);
    XCTAssertNotNil(err);
    XCTAssertEqual(err.code, 0x5);

    err = nil;
    sourceLanguageConfig = [[SPXSourceLanguageConfiguration alloc]initWithLanguage:@"zh-CN" endpointId:@"" error:&err];
    XCTAssertNil(sourceLanguageConfig);
    XCTAssertNotNil(err);
    XCTAssertEqual(err.code, 0x5);

    err = nil;
    sourceLanguageConfig = [[SPXSourceLanguageConfiguration alloc]initWithLanguage:@"zh-CN" endpointId:@"myEndpointId" error:&err];
    XCTAssertNotNil(sourceLanguageConfig);
    XCTAssertNil(err);
}

- (void)testCreateAutoDetectSrcLangConfigDifferentParams {
    NSError * err = nil;
    SPXAutoDetectSourceLanguageConfiguration* autoDetectSourceLanguageConfig = [[SPXAutoDetectSourceLanguageConfiguration alloc]init:nil error:&err];
    XCTAssertNil(autoDetectSourceLanguageConfig);
    XCTAssertNotNil(err);
    XCTAssertEqual(err.code, 0x5);

    err = nil;
    autoDetectSourceLanguageConfig = [[SPXAutoDetectSourceLanguageConfiguration alloc]init:@[] error:&err];
    XCTAssertNil(autoDetectSourceLanguageConfig);
    XCTAssertNotNil(err);
    XCTAssertEqual(err.code, 0x5);

    err = nil;
    autoDetectSourceLanguageConfig = [[SPXAutoDetectSourceLanguageConfiguration alloc]init:@[@"zh-CN", @""] error:&err];
    XCTAssertNil(autoDetectSourceLanguageConfig);
    XCTAssertNotNil(err);
    XCTAssertEqual(err.code, 0x5);

    err = nil;
    autoDetectSourceLanguageConfig = [[SPXAutoDetectSourceLanguageConfiguration alloc]init:@[@"zh-CN", @"de-DE"] error:&err];
    XCTAssertNotNil(autoDetectSourceLanguageConfig);
    XCTAssertNil(err);

    NSString *zhLanguage = @"zh-CN";
    NSString *deLanguage = @"de-DE";
    NSString *testEndpointId = @"myendpoint";
    SPXSourceLanguageConfiguration* zhSourceLanguageConfig = [[SPXSourceLanguageConfiguration alloc]init:zhLanguage];
    SPXSourceLanguageConfiguration* deSourceLanguageConfig = [[SPXSourceLanguageConfiguration alloc]initWithLanguage:deLanguage endpointId:testEndpointId];

    err = nil;
    autoDetectSourceLanguageConfig = [[SPXAutoDetectSourceLanguageConfiguration alloc]initWithSourceLanguageConfigurations:nil error:&err];
    XCTAssertNil(autoDetectSourceLanguageConfig);
    XCTAssertNotNil(err);
    XCTAssertEqual(err.code, 0x5);

    err = nil;
    autoDetectSourceLanguageConfig = [[SPXAutoDetectSourceLanguageConfiguration alloc]initWithSourceLanguageConfigurations:@[zhSourceLanguageConfig, deSourceLanguageConfig] error:&err];
    XCTAssertNotNil(autoDetectSourceLanguageConfig);
    XCTAssertNil(err);
}
@end
