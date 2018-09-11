//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef translation_text_event_args_h
#define translation_text_event_args_h

#import <Foundation/Foundation.h>
#import "translation_text_result.h"

@interface TranslationTextEventArgs : NSObject

@property (readonly) NSString* sessionId;

@property (readonly) TranslationTextResult* result;

@end

#endif /* translation_text_event_args_h */
