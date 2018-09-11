//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef recognition_result_base_h
#define recognition_result_base_h

#import <Foundation/Foundation.h>
#import "recognition_status.h"

@interface RecognitionResultBase : NSObject

@property (readonly) NSString* resultId;

@property (readonly) RecognitionStatus reason;

@property (readonly) NSString* text;

@property (readonly) NSUInteger duration;

@property (readonly) NSUInteger offset;

@property (readonly) NSString* errorDetails;

// Todo: get property bag

@end

#endif /* recognition_result_base_h */
