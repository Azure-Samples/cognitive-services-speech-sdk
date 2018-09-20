//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "SPXTranslationRecognizer.h"
#import "translation_synthesis_result_event_args_private.h"
#import "common_private.h"

@interface SPXTranslationRecognizer (Private)

- (instancetype)initWithImpl:(TranslationRecoSharedPtr)recoHandle;

- (void)onRecognizedEvent:(SPXTranslationRecognitionEventArgs *)eventArgs;
- (void)onRecognizingEvent:(SPXTranslationRecognitionEventArgs *)eventArgs;
- (void)onSynthesizingEvent:(SPXTranslationSynthesisEventArgs *)eventArgs;
- (void)onCanceledEvent:(SPXTranslationRecognitionCanceledEventArgs *)eventArgs;

@end
