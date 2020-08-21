//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "keyword_recognition_model_private.h"
#import "speechapi_private.h"
#import "common_private.h"

@implementation SPXKeywordRecognitionModel
{
    std::shared_ptr<KeywordImpl::KeywordRecognitionModel> keywordRecognitionModelImpl;
}

- (instancetype)initFromFile:(NSString *)fileName
{
    auto modelImpl = KeywordImpl::KeywordRecognitionModel::FromFile([fileName toSpxString]);
    if (modelImpl == nullptr) {
        return nil;
    }
    return [self initWithImpl:modelImpl];
}

- (instancetype)initFromFile:(NSString *)fileName error:(NSError * _Nullable * _Nullable)outError
{
    try {
        self = [self initFromFile:fileName];
        return self;
    }
    catch (NSException *exception) {
        *outError = [Util getErrorFromException:exception];
    }
    return nil;
}

- (instancetype)initWithImpl:(std::shared_ptr<KeywordImpl::KeywordRecognitionModel>)modelImpl
{
    self = [super init];
    if (!self || modelImpl == nullptr) {
        return nil;
    }
    keywordRecognitionModelImpl = modelImpl;
    return self;
}

- (std::shared_ptr<KeywordImpl::KeywordRecognitionModel>)getModelHandle
{
    return keywordRecognitionModelImpl;
}

@end
