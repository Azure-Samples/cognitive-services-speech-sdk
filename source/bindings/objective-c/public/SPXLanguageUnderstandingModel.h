//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#import <Foundation/Foundation.h>

/**
  * Represents language understanding model used for intent recognition.
  */
@interface SPXLanguageUnderstandingModel : NSObject

/**
  * Initializes a language understanding model using the specified endpoint.
  * @param uri the endpoint of the language understanding model.
  * @return an instance of language understanding model.
  */
- (nullable instancetype)initWithEndpoint:(nonnull NSString *)uri;

/**
  * Initializes a language understanding model using the application id of Language Understanding service.
  * @param appId the application id of Language Understanding service.
  * @return an instance of language understanding model.
  */
- (nullable instancetype)initWithAppId:(nonnull NSString *)appId;

/**
  * Initializes a language understanding model using subscription key, application id and region of Language Understanding service.
  * @param key the subscription key of Language Understanding service.
  * @param appId the application id of Language Understanding service.
  * @param region the region of the Language Understanding service (see the <a href="https://aka.ms/csspeech/region">region page</a>).
  * @return an instance of language understanding model.
  */
- (nullable instancetype)initWithSubscription:(nonnull NSString *)key withAppId:(nonnull NSString *)appId andRegion:(nonnull NSString *)region;

@end