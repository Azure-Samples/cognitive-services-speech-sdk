//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef language_understanding_model_h
#define language_understanding_model_h

#import <Foundation/Foundation.h>

/**
  * Represents language understanding model used for intent recognition.
  */
@interface LanguageUnderstandingModel : NSObject

/**
  * Creates a language understanding model using the specified endpoint.
  * @param uri the endpoint of the language understanding model.
  * @return language understanding model being created.
  */
+ (LanguageUnderstandingModel *)FromEndpoint:(NSString *)uri;

/**
  * Creates a language understanding model using the application id of Language Understanding service.
  * @param appId the application id of Language Understanding service.
  * @return language understanding model being created.
  */
+ (LanguageUnderstandingModel *)FromAppId:(NSString *)appId;

/**
  * Creates a language understanding model using subscription key, application id and region of Language Understanding service.
  * @param key the subscription key of Language Understanding service.
  * @param appId the application id of Language Understanding service.
  * @param region the region of the Language Understanding service (see the <a href="https://aka.ms/csspeech/region">region page</a>).
  * @return The language understanding model being created.
  */
+ (LanguageUnderstandingModel *)FromSubscription:(NSString *)key withAppId:(NSString *)appId andRegion:(NSString *)region;

@end

#endif /* language_understanding_model_h */
