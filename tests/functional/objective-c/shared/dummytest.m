//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "dummytest.h"

@implementation DummyTest
    
+ (void)runTest:(NSString *)speechKey withRegion:(NSString *)region;
{
    // Test AudioStreamFormat
    SPXAudioStreamFormat *streamFormat __attribute__((unused)) = [[SPXAudioStreamFormat alloc] init];
    streamFormat = [[SPXAudioStreamFormat alloc] initUsingPCMWithSampleRate:16000 bitsPerSample:16 channels:1];
    
    // Test audio stream.
    SPXPushAudioInputStream *pushStream __attribute__((unused)) = [[SPXPushAudioInputStream alloc] init];
    pushStream = [[SPXPushAudioInputStream alloc] initWithAudioFormat:streamFormat];
    NSData *streamData = [[NSData alloc] init];
    [pushStream write:streamData];
    [pushStream close];
    
    SPXPullAudioInputStream *pullStream __attribute__((unused)) = [[SPXPullAudioInputStream alloc] initWithReadHandler:^ (NSMutableData *data, NSUInteger size) { return (NSInteger)0; } closeHandler: ^(void) {return;}];
    pullStream = [[SPXPullAudioInputStream alloc] initWithAudioFormat:streamFormat readHandler:^ (NSMutableData *data, NSUInteger size) { return (NSInteger)0; } closeHandler: ^(void) {return;}];
    
    // Test audio configuration
    SPXAudioConfiguration *audioConfig __attribute__((unused))  = [[SPXAudioConfiguration alloc] init];
    audioConfig = [[SPXAudioConfiguration alloc] initWithWavFileInput:@"testaudio.wav"];
    audioConfig =[[SPXAudioConfiguration alloc] initWithStreamInput:pullStream];
    audioConfig =[[SPXAudioConfiguration alloc] initWithStreamInput:pushStream];
    audioConfig =[[SPXAudioConfiguration alloc] initWithMicrophone:nil];
    audioConfig =[[SPXAudioConfiguration alloc] initWithMicrophone:@"mymicrophone"]; // this would break when starting recognition.
    
    // Test luis model
    SPXLanguageUnderstandingModel *luisModel __attribute__((unused));
    //luisModel = [[SPXLanguageUnderstandingModel alloc] initWithEndpoint:@"https://www.luis.ai"];
    luisModel = [[SPXLanguageUnderstandingModel alloc] initWithAppId:@"LuisAppId"];
    luisModel = [[SPXLanguageUnderstandingModel alloc] initWithSubscription:@"werwe" withAppId:@"LuisAppId" andRegion:@"someregion"];
    
    // Test Speech Configuration
    SPXSpeechConfiguration *testSpeechConfig __attribute__((unused)) = [[SPXSpeechConfiguration alloc] initWithAuthorizationToken:@"SDFSDF"  region:@"someregion"];
    testSpeechConfig = [[SPXSpeechConfiguration alloc] initWithEndpoint:@"https://someregion.api.com" subscription:@"dummy"];
    NSString *value __attribute__((unused)) = [testSpeechConfig getPropertyById:SPXSpeechServiceConnectionKey];
    [testSpeechConfig setPropertyTo:@"Dummy2" byId:SPXSpeechServiceConnectionKey];
    value = testSpeechConfig.speechRecognitionLanguage;
    testSpeechConfig.speechRecognitionLanguage = value;
    value = testSpeechConfig.endpointId;
    testSpeechConfig.endpointId = value;
    value = testSpeechConfig.authorizationToken;
    testSpeechConfig.authorizationToken = value;
    value = testSpeechConfig.subscriptionKey;
    value = testSpeechConfig.region;
    
    // test speech recognizer
    SPXSpeechRecognizer *testSpeechRecognizer = [[SPXSpeechRecognizer alloc] init:testSpeechConfig];
    value = testSpeechRecognizer.endpointId;
    value = testSpeechRecognizer.authorizationToken;
    
    // Test speech translation configuration
    SPXSpeechTranslationConfiguration *testTranslationConfig __attribute__((unused)) = [[SPXSpeechTranslationConfiguration alloc] initWithAuthorizationToken:@"SDFSDF"  region:@"someregion"];
    testTranslationConfig = [[SPXSpeechTranslationConfiguration alloc] initWithEndpoint:@"https://someregion.api.com" subscription:@"dummy"];
    value = [testTranslationConfig getPropertyById:SPXSpeechServiceConnectionKey];
    [testTranslationConfig setPropertyTo:@"dummy" byId:SPXSpeechServiceConnectionKey];

    [testTranslationConfig setSpeechRecognitionLanguage:@"en-us"];
    value = testTranslationConfig.speechRecognitionLanguage;
    testTranslationConfig.speechRecognitionLanguage = value;

    [testTranslationConfig addTargetLanguage:@"de-DE"];
    NSArray *target __attribute__((unused)) = [testTranslationConfig targetLanguages];
    NSString *voiceName __attribute__((unused)) = [testTranslationConfig voiceName];
    
    // test translation recognizer
    SPXTranslationRecognizer *testTranslationRecognizer = [[SPXTranslationRecognizer alloc] init:testTranslationConfig];
    value = testTranslationRecognizer.authorizationToken;
}
    
@end
    
