//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef SPXSpeechRecognitionEndtoEndTest_h
#define SPXSpeechRecognitionEndtoEndTest_h

#import <XCTest/XCTest.h>

@interface SPXSpeechRecognitionEndtoEndTest : XCTestCase {
    // test data
    NSString *weatherFileName;
    NSString *weatherTextEnglish;
    NSString *margaritaFileName;
    NSString *margaritaTextEnglish;
    
    // the file/transcription to use, set to weather as the default.
    // Overwrite in test methods as needed
    NSString *targetFileName;
    NSString *targetTranscription;
    
    NSMutableDictionary *result;
    
    NSPredicate *sessionStoppedCountPred;
    
    double timeoutInSeconds;
}

- (void) speechInit;
- (void) setAudioSource;

- (void) _testRecognizeAsyncWithPreConnection;
- (void) _testContinuousRecognition;
- (void) _testRecognizeAsync;
- (void) _testRecognizeOnce;
- (void) _testContinuousRecognitionWithPreConnection;
- (void) _testRecognizerWithMultipleHandlers;

@property (nonatomic, assign) NSString * speechKey;
@property (nonatomic, assign) NSString * serviceRegion;
@property (nonatomic, retain) SPXSpeechRecognizer* speechRecognizer;
@property (nonatomic, retain) SPXAudioConfiguration* audioConfig;
@property (nonatomic, retain) SPXConnection* connection;
@end

#endif /* SPXSpeechRecognitionEndtoEndTest_h */
