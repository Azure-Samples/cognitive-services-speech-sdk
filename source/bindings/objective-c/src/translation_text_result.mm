//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "translation_text_result_private.h"

#import "common_private.h"

@implementation TranslationTextResult
{
    std::shared_ptr<TranslationImpl::TranslationTextResult> resultImpl;
}

- (instancetype)init :(std::shared_ptr<TranslationImpl::TranslationTextResult>)resultHandle
{
    self = [super init :resultHandle];
    resultImpl = resultHandle;

    switch (resultImpl->TranslationStatus)
    {
        case TranslationImpl::TranslationStatusCode::Success:
            _translationStatus = TranslationStatus::TranslationSuccess;
            break;
        case TranslationImpl::TranslationStatusCode::Error:
            _translationStatus = TranslationStatus::TranslationError;
            break;
        default:
            // Todo error handling.
            NSLog(@"Unknown translation status");
            self = nil;
            break;
    }

    _failureReason = [NSString stringWithString:resultImpl->FailureReason];

    auto resultInMap = resultImpl->Translations;
    _translations = [[NSMutableDictionary alloc] initWithCapacity:resultInMap.size()];
    for (auto it : resultInMap)
        [_translations setValue:[NSString stringWithString:it.second] forKey:[NSString stringWithString:it.first]];

    return self;
}

- (instancetype)initWithError: (NSString *)message
{
    self = [super initWithError:message];
    _translationStatus = TranslationStatus::TranslationError;
    _failureReason = message;
    return self;
}

- (void)dealloc
{
    if (resultImpl != nullptr) {
        resultImpl.reset();
    }
}

@end
