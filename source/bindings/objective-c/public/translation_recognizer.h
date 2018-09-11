//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef translation_recognizer_h
#define translation_recognizer_h

#import <Foundation/Foundation.h>
#import "recognizer.h"
#import "translation_text_result.h"
#import "translation_text_event_args.h"
#import "translation_synthesis_result.h"
#import "translation_synthesis_event_args.h"

@interface TranslationRecognizer : Recognizer

typedef void (^TranslationTextEventHandlerBlock)(TranslationRecognizer *, TranslationTextEventArgs *);
typedef void (^TranslationSynthesisEventHandlerBlock)(TranslationRecognizer *, TranslationSynthesisEventArgs *);

- (void)close;

- (TranslationTextResult *)translate;
- (void)translateAsync:(void (^)(TranslationTextResult *))resultReceivedBlock;

// Todo: add protocol interface for registering callback.

// Todo: add block interface
- (void)startContinuousTranslation;
- (void)stopContinuousTranslation;

- (void)addFinalResultEventListener:(TranslationTextEventHandlerBlock)eventHandler;
- (void)addIntermediateResultEventListener:(TranslationTextEventHandlerBlock)eventHandler;
- (void)addSynthesisResultEventListener:(TranslationSynthesisEventHandlerBlock)eventHandler;

@end

#endif /* translation_recognizer_h */
