//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef intent_recognition_result_h
#define intent_recognition_result_h

#import <Foundation/Foundation.h>
#import "recognition_status.h"
#import "recognition_result_base.h"

@interface IntentRecognitionResult : RecognitionResultBase

@property (readonly) NSString* intentId;

@end

#endif /* intent_recognition_result_h */
