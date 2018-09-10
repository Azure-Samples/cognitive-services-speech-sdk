//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef speechfactory_h
#define speechfactory_h

#import <Foundation/Foundation.h>
#import "speech_recognizer.h"
#import "translation_recognizer.h"

typedef NS_ENUM(NSInteger, OutputFormat)
{
    OutputFormatSimple = 0,
    OutoutFormatDetailed = 1
};

@interface SpeechFactory : NSObject

- (SpeechRecognizer*)createSpeechRecognizer;
// - (SpeechRecognizer*) createSpeechREcognizerUsingDefaultMicrophoneWithLanguage: (NSString *)language;
// - (SpeechRecognizer*) createSpeechRecognizerUsingDefaultMicrophoneWithLanguage: (NSString *)language AndOutputFormat: (OutputFormat)outputFormat;

- (SpeechRecognizer*)createSpeechRecognizerWithFileInput:(NSString *)path;
// - (SpeechRecognizer*) createSpeechRecognizerWithFileInput: (NSString *)path AndLanguage: (NSString *)language;
// - (SpeechRecognizer*) createSpeechRecognizerWithFileInput: (NSString *)path AndLanguage: (NSString *)language AndOutputFormat: (OutputFormat)outputFormat;

//- (SpeechRecognizer*) createSpeechRecognizerWithStream: (AudioInputStream *)inputStream;
//- (SpeechRecognizer*) createSpeechRecognizerWithStream: (AudioInputStream *)inputStream AndLanguage: (NSString *)language;
//- (SpeechRecognizer*) createSpeechRecognizerWithStream: (AudioInputStream *)inputStream AndLanguage: (NSString *)language AndOutputFormat: (OutputFormat)outputFormat;

- (TranslationRecognizer*)createTranslationRecognizerFromLanguage:(NSString *)from ToLanguages:(NSArray *)to;
- (TranslationRecognizer*)createTranslationRecognizerFromLanguage:(NSString *)from ToLanguages:(NSArray *)to WithVoiceOutput:(NSString *)voice;
- (TranslationRecognizer*)createTranslationRecognizerWithFileInput:(NSString *)path FromLanguage:(NSString *)from ToLanguages:(NSArray *)to;
- (TranslationRecognizer*)createTranslationRecognizerWithFileInput:(NSString *)path FromLanguage:(NSString *)from ToLanguages:(NSArray *)to WithVoiceOutput:(NSString *)voice;


+ (SpeechFactory*)fromSubscription:(NSString *)subscription AndRegion:(NSString *)region;
+ (SpeechFactory*)fromAuthorizationToken:(NSString *)authToken AndRegion:(NSString *)region;
+ (SpeechFactory*)fromEndpoint:(NSString *)endpointUrl AndSubscription:(NSString *)subscriptionKey;

@end


#endif /* speechfactory_h */
