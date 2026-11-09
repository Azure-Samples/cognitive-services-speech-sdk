// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

#import <Foundation/Foundation.h>
#import <MicrosoftCognitiveServicesSpeech/SPXSpeechApi.h>
#import <dispatch/dispatch.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    @autoreleasepool {
        NSDictionary<NSString *, NSString *> *environment = NSProcessInfo.processInfo.environment;
        NSString *speechKey = environment[@"SPEECH_KEY"];
        NSString *endpoint = environment[@"SPEECH_ENDPOINT"];
        NSString *audioFile = environment[@"LIVE_INTERPRETER_AUDIO_FILE"];
        if (!speechKey.length || !endpoint.length || !audioFile.length) {
            NSLog(@"Set SPEECH_KEY, SPEECH_ENDPOINT, and LIVE_INTERPRETER_AUDIO_FILE.");
            return EXIT_FAILURE;
        }

        NSError *error = nil;
        SPXSpeechTranslationConfiguration *translationConfig =
            [[SPXSpeechTranslationConfiguration alloc]
                initWithEndpoint:endpoint subscription:speechKey error:&error];
        if (!translationConfig) {
            NSLog(@"Configuration failed: %@", error);
            return EXIT_FAILURE;
        }
        [translationConfig addTargetLanguage:@"fr"];
        translationConfig.voiceName = @"personal-voice";
        [translationConfig setSpeechSynthesisOutputFormat:SPXSpeechSynthesisOutputFormat_Riff16Khz16BitMonoPcm];

        SPXAutoDetectSourceLanguageConfiguration *autoDetectConfig =
            [[SPXAutoDetectSourceLanguageConfiguration alloc] initWithOpenRange:&error];
        if (!autoDetectConfig) {
            NSLog(@"Language detection configuration failed: %@", error);
            return EXIT_FAILURE;
        }
        SPXAudioConfiguration *audioConfig =
            [[SPXAudioConfiguration alloc] initWithWavFileInput:audioFile];
        if (!audioConfig) {
            NSLog(@"Couldn't open WAV input: %@", audioFile);
            return EXIT_FAILURE;
        }
        SPXTranslationRecognizer *recognizer =
            [[SPXTranslationRecognizer alloc]
                initWithSpeechTranslationConfiguration:translationConfig
                autoDetectSourceLanguageConfiguration:autoDetectConfig
                audioConfiguration:audioConfig error:&error];
        if (!recognizer) {
            NSLog(@"Recognizer creation failed: %@", error);
            return EXIT_FAILURE;
        }

        dispatch_semaphore_t stopped = dispatch_semaphore_create(0);
        dispatch_queue_t stateQueue = dispatch_queue_create("live-interpreter", DISPATCH_QUEUE_SERIAL);
        __block NSUInteger audioIndex = 0;
        __block NSUInteger translationCount = 0;
        __block BOOL failed = NO;
        __block BOOL stopping = NO;
        __block BOOL closed = NO;

        [recognizer addRecognizingEventHandler:
            ^(SPXTranslationRecognizer *sender, SPXTranslationRecognitionEventArgs *event) {
            dispatch_sync(stateQueue, ^{
                if (closed || failed) {
                    return;
                }
                NSLog(@"RECOGNIZING: %@", event.result.text);
                NSLog(@"TRANSLATING: %@", event.result.translations);
            });
        }];
        [recognizer addRecognizedEventHandler:
            ^(SPXTranslationRecognizer *sender, SPXTranslationRecognitionEventArgs *event) {
            dispatch_sync(stateQueue, ^{
                if (closed || failed) {
                    return;
                }
                if (event.result.reason == SPXResultReason_NoMatch) {
                    NSLog(@"NO MATCH: no speech recognized in this segment.");
                    return;
                }
                if (event.result.reason != SPXResultReason_TranslatedSpeech) {
                    NSLog(@"Recognition failed: result reason=%lu", (unsigned long)event.result.reason);
                    failed = YES;
                    dispatch_semaphore_signal(stopped);
                    return;
                }
                NSString *language = [event.result.properties
                    getPropertyById:SPXSpeechServiceConnectionAutoDetectSourceLanguageResult];
                NSString *translation = event.result.translations[@"fr"];
                if (!event.result.text.length || !language.length || !translation.length) {
                    NSLog(@"Recognition failed: missing recognized text, detected language, or French translation.");
                    failed = YES;
                    dispatch_semaphore_signal(stopped);
                    return;
                }
                ++translationCount;
                NSLog(@"RECOGNIZED in '%@': %@", language, event.result.text);
                NSLog(@"TRANSLATED (fr): %@", translation);
            });
        }];
        [recognizer addSynthesizingEventHandler:
            ^(SPXTranslationRecognizer *sender, SPXTranslationSynthesisEventArgs *event) {
            dispatch_sync(stateQueue, ^{
                if (closed || failed) {
                    return;
                }
                if (event.result.reason == SPXResultReason_Canceled) {
                    NSLog(@"Translation synthesis failed: result reason=Canceled.");
                    failed = YES;
                    dispatch_semaphore_signal(stopped);
                    return;
                }
                NSData *audio = event.result.audio;
                // Empty audio ends an utterance, not the continuous recognition session.
                if (!audio.length) {
                    return;
                }
                NSString *outputFile = [NSString stringWithFormat:@"translation-%lu.wav",
                    (unsigned long)(audioIndex + 1)];
                NSError *writeError = nil;
                if (![audio writeToFile:outputFile options:NSDataWritingWithoutOverwriting error:&writeError]) {
                    NSLog(@"Couldn't create %@: %@", outputFile, writeError);
                    failed = YES;
                    dispatch_semaphore_signal(stopped);
                    return;
                }
                ++audioIndex;
                NSLog(@"Wrote %@ (%lu bytes)", outputFile, (unsigned long)audio.length);
            });
        }];
        [recognizer addCanceledEventHandler:
            ^(SPXTranslationRecognizer *sender, SPXTranslationRecognitionCanceledEventArgs *event) {
            dispatch_sync(stateQueue, ^{
                if (closed) {
                    return;
                }
                NSLog(@"CANCELED: Reason=%lu", (unsigned long)event.reason);
                if (event.reason == SPXCancellationReason_Error ||
                    (event.reason != SPXCancellationReason_EndOfStream && !stopping)) {
                    NSLog(@"Translation canceled: ErrorCode=%lu: %@",
                        (unsigned long)event.errorCode, event.errorDetails);
                    failed = YES;
                }
                dispatch_semaphore_signal(stopped);
            });
        }];
        [recognizer addSessionStoppedEventHandler:
            ^(SPXRecognizer *sender, SPXSessionEventArgs *event) {
            dispatch_sync(stateQueue, ^{
                if (!closed) {
                    NSLog(@"SESSION STOPPED");
                    dispatch_semaphore_signal(stopped);
                }
            });
        }];

        dispatch_time_t deadline = dispatch_time(DISPATCH_TIME_NOW, 300 * NSEC_PER_SEC);
        // Bound synchronous SDK start/stop calls as well as the completion wait.
        dispatch_after(deadline, dispatch_get_global_queue(QOS_CLASS_USER_INITIATED, 0), ^{
            fprintf(stderr, "Live Interpreter timed out after five minutes.\n");
            _Exit(EXIT_FAILURE);
        });
        if (![recognizer startContinuousRecognition:&error]) {
            NSLog(@"Couldn't start translation: %@", error);
            return EXIT_FAILURE;
        }
        long waitResult = dispatch_semaphore_wait(stopped, deadline);
        dispatch_sync(stateQueue, ^{
            stopping = YES;
            if (waitResult != 0) {
                NSLog(@"Translation timed out after five minutes.");
                failed = YES;
            }
        });
        // Never stop the recognizer from an SDK callback or while holding stateQueue.
        if (![recognizer stopContinuousRecognition:&error]) {
            NSLog(@"Couldn't stop translation: %@", error);
            return EXIT_FAILURE;
        }
        __block int exitCode;
        dispatch_sync(stateQueue, ^{
            closed = YES;
            if (!failed && (translationCount == 0 || audioIndex == 0)) {
                NSLog(@"Translation failed: no translated speech or synthesized audio was produced.");
                failed = YES;
            }
            exitCode = failed ? EXIT_FAILURE : EXIT_SUCCESS;
        });
        return exitCode;
    }
}
