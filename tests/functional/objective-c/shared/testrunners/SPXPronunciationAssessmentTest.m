//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import <XCTest/XCTest.h>
#import <AVFoundation/AVFoundation.h>
#import <MicrosoftCognitiveServicesSpeech/SPXSpeechApi.h>
@interface SPXPronunciationAssessmentTest : XCTestCase {
    // the file/transcription to use
    NSString *targetFileName;
    NSString *targetTranscription;
}

@property (nonatomic, assign) NSString * speechKey;
@property (nonatomic, assign) NSString * serviceRegion;
@property (nonatomic, retain) SPXSpeechConfiguration* speechConfig;
@property (nonatomic, retain) SPXAudioConfiguration* audioConfig;
@end

@implementation SPXPronunciationAssessmentTest

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

- (void)testPronunciationAssessmentConfig {
    SPXPronunciationAssessmentConfiguration* pronConfig =
    [[SPXPronunciationAssessmentConfiguration alloc]init:@"reference"
                                           gradingSystem:SPXPronunciationAssessmentGradingSystem_HundredMark
                                             granularity:SPXPronunciationAssessmentGranularity_Phoneme];
    XCTAssertNotNil(pronConfig);
    XCTAssertTrue([[pronConfig toJson] containsString:@"HundredMark"]);
    XCTAssertTrue([[pronConfig toJson] containsString:@"Phoneme"]);
    XCTAssertTrue([[pronConfig toJson] containsString:@"Comprehensive"]);
    XCTAssertTrue([[pronConfig toJson] containsString:@"reference"]);

    pronConfig =
    [[SPXPronunciationAssessmentConfiguration alloc]init:@"reference"
                                           gradingSystem:SPXPronunciationAssessmentGradingSystem_HundredMark
                                             granularity:SPXPronunciationAssessmentGranularity_Word
                                            enableMiscue:true];
    XCTAssertNotNil(pronConfig);
    pronConfig.referenceText = @"new reference";
    XCTAssertTrue([[pronConfig toJson] containsString:@"HundredMark"]);
    XCTAssertTrue([[pronConfig toJson] containsString:@"Word"]);
    XCTAssertTrue([[pronConfig toJson] containsString:@"new reference"]);

    SPXPronunciationAssessmentConfiguration* pronConfig2 = [[SPXPronunciationAssessmentConfiguration alloc]initWithJson:[pronConfig toJson]];
    XCTAssertNotNil(pronConfig2);
    XCTAssertEqualObjects([pronConfig toJson], [pronConfig2 toJson]);
}

- (void)testPronunciationAssessmentConfigInvalid {
    NSError * err = nil;
    SPXPronunciationAssessmentConfiguration* pronConfig = [[SPXPronunciationAssessmentConfiguration alloc]initWithJson:@"invalid json" error:&err];
    XCTAssertNil(pronConfig);
    XCTAssertNotNil(err);
    XCTAssertEqual(err.code, 0x5);
}

- (void)testPronunciationAssessmentConfigApplyTo {
    SPXSpeechRecognizer* speechRecognizer = [[SPXSpeechRecognizer alloc] initWithSpeechConfiguration:self.speechConfig
                                                                                  audioConfiguration:self.audioConfig];
    XCTAssertNotNil(speechRecognizer);

    SPXPronunciationAssessmentConfiguration* pronConfig = [[SPXPronunciationAssessmentConfiguration alloc]init:@"reference"];
    XCTAssertNotNil(pronConfig);

    [pronConfig applyToRecognizer:speechRecognizer];

    XCTAssertEqualObjects([pronConfig toJson], [speechRecognizer.properties getPropertyById:SPXPronunciationAssessment_Params]);
}

- (void)testPronunciationAssessment {
    SPXSpeechRecognizer* speechRecognizer = [[SPXSpeechRecognizer alloc] initWithSpeechConfiguration:self.speechConfig
                                                                                  audioConfiguration:self.audioConfig];
    XCTAssertNotNil(speechRecognizer);

    SPXPronunciationAssessmentConfiguration* pronConfig = [[SPXPronunciationAssessmentConfiguration alloc]init:@""];
    XCTAssertNotNil(pronConfig);

    [pronConfig applyToRecognizer:speechRecognizer];

    SPXSpeechRecognitionResult* result = [speechRecognizer recognizeOnce];

    NSLog(@"recognition result:%@. Status %ld. offset %llu duration %llu resultid:%@",
          result.text, (long)result.reason, result.offset, result.duration, result.resultId);
    XCTAssertEqualObjects(result.text, targetTranscription);
    XCTAssertEqual(result.reason, SPXResultReason_RecognizedSpeech);
    XCTAssertGreaterThan(result.duration, 0);
    XCTAssertGreaterThan(result.offset, 0);
    XCTAssertGreaterThan([result.resultId length], 0);

    SPXPronunciationAssessmentResult* pronResult = [[SPXPronunciationAssessmentResult alloc] init:result];
    XCTAssertNotNil(pronResult);

    NSLog(@"pronunciation assessment result. accuracy score: %f. pronunciation score: %f. completeness score: %f. fluency score: %f",
          pronResult.accuracyScore, pronResult.pronunciationScore, pronResult.completenessScore, pronResult.fluencyScore);

    XCTAssertGreaterThan(pronResult.accuracyScore, 0);
    XCTAssertGreaterThan(pronResult.pronunciationScore, 0);
    XCTAssertGreaterThan(pronResult.completenessScore, 0);
    XCTAssertGreaterThan(pronResult.fluencyScore, 0);
}

@end
