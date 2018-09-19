//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef microphone_test_h
#define microphone_test_h

#import <Foundation/Foundation.h>
#import "speechapi.h"

@interface MicrophoneTest : NSObject

+(void) runAsync;
+(void) runTranslation;
+(void) runContinuous;

@end

#endif /* microphone_test_h */
