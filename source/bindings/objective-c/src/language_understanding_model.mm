//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "language_understanding_model_private.h"
#import "common_private.h"

@implementation LanguageUnderstandingModel
{
    std::shared_ptr<IntentImpl::LanguageUnderstandingModel> modelImpl;
}

+ (LanguageUnderstandingModel *)FromEndpoint: (NSString *)uri
{
    auto modelImpl = IntentImpl::LanguageUnderstandingModel::FromEndpoint([uri string]);
    return [[LanguageUnderstandingModel alloc] init :modelImpl];
}

+ (LanguageUnderstandingModel *)FromAppId: (NSString *)appId
{
    auto modelImpl = IntentImpl::LanguageUnderstandingModel::FromAppId([appId string]);
    return [[LanguageUnderstandingModel alloc] init :modelImpl];
}

+ (LanguageUnderstandingModel *)FromSubscription: (NSString *)key withAppId: (NSString *)appId andRegion: (NSString *)region
{
    auto modelImpl = IntentImpl::LanguageUnderstandingModel::FromSubscription([key string], [appId string], [region string]);
    return [[LanguageUnderstandingModel alloc] init :modelImpl];
}

- (instancetype)init :(std::shared_ptr<IntentImpl::LanguageUnderstandingModel>)modelHandle
{
    self = [super init];
    modelImpl = modelHandle;
    return self;
}

- (std::shared_ptr<IntentImpl::LanguageUnderstandingModel>)getModelHandle
{
    return modelImpl;
}

@end
