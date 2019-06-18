//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "language_understanding_model_private.h"
#import "common_private.h"

@implementation SPXLanguageUnderstandingModel
{
    std::shared_ptr<IntentImpl::LanguageUnderstandingModel> languageUnderstandingModelImpl;
}

- (instancetype)initWithEndpoint:(NSString *)uri
{
    auto modelImpl = IntentImpl::LanguageUnderstandingModel::FromEndpoint([uri toSpxString]);
    if (modelImpl == nullptr) {
        return nil;
    }
    return [self initWithImpl:modelImpl];
}

- (instancetype)initWithAppId:(NSString *)appId
{
    auto modelImpl = IntentImpl::LanguageUnderstandingModel::FromAppId([appId toSpxString]);
    if (modelImpl == nullptr) {
        return nil;
    }
    return [self initWithImpl:modelImpl];
}


- (instancetype)initWithSubscription:(NSString *)key withAppId:(NSString *)appId andRegion:(NSString *)region
{
    auto modelImpl = IntentImpl::LanguageUnderstandingModel::FromSubscription([key toSpxString], [appId toSpxString], [region toSpxString]);
    if (modelImpl == nullptr) {
        return nil;
    }
    return [self initWithImpl:modelImpl];
}

- (instancetype)initWithImpl:(std::shared_ptr<IntentImpl::LanguageUnderstandingModel>)modelImpl
{
    self = [super init];
    if (!self || modelImpl == nullptr) {
        return nil;
    }
    languageUnderstandingModelImpl = modelImpl;
    return self;
}

- (std::shared_ptr<IntentImpl::LanguageUnderstandingModel>)getModelHandle
{
    return languageUnderstandingModelImpl;
}

@end
