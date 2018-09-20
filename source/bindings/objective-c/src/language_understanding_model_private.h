//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "SPXLanguageUnderstandingModel.h"
#import "common_private.h"

@interface SPXLanguageUnderstandingModel (Private)

- (std::shared_ptr<IntentImpl::LanguageUnderstandingModel>)getModelHandle;

@end