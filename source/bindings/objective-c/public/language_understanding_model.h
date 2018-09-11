//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef language_understanding_model_h
#define language_understanding_model_h

#import <Foundation/Foundation.h>

@interface LanguageUnderstandingModel : NSObject

+ (LanguageUnderstandingModel *)FromEndpoint: (NSString *)uri;

+ (LanguageUnderstandingModel *)FromAppId: (NSString *)appId;

+ (LanguageUnderstandingModel *)FromSubscription: (NSString *)key withAppId: (NSString *)appId andRegion: (NSString *)region;

@end

#endif /* language_understanding_model_h */
