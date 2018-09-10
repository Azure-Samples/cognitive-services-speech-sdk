//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef translation_recognizer_h
#define translation_recognizer_h

#import <Foundation/Foundation.h>
#import "recognizer.h"
#import "translation_text_result.h"
#import "translation_text_result_event_args.h"
#import "translation_synthesis_result.h"
#import "translation_synthesis_result_event_args.h"

@interface TranslationRecognizer : Recognizer

typedef void (^TranslationTextResultEventHandlerBlock)(TranslationRecognizer *, TranslationTextResultEventArgs *);
typedef void (^TranslationSynthesisResultEventHandlerBlock)(TranslationRecognizer *, TranslationSynthesisResultEventArgs *);

- (void)close;

- (TranslationTextResult *)translate;
- (void)translateAsync:(void (^)(TranslationTextResult *))resultReceivedBlock;

// Todo: add protocol interface for registering callback.

// Todo: add block interface
- (void)startContinuousTranslation;
- (void)stopContinuousTranslation;

- (void)addFinalResultEventListener:(TranslationTextResultEventHandlerBlock)eventHandler;
- (void)addIntermediateResultEventListener:(TranslationTextResultEventHandlerBlock)eventHandler;
- (void)addSynthesisResultEventListener:(TranslationSynthesisResultEventHandlerBlock)eventHandler;

@end

#endif /* translation_recognizer_h */
