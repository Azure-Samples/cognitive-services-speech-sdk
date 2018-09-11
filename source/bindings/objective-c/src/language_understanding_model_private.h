//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef language_understanding_model_private_h
#define language_understanding_model_private_h

#import "language_understanding_model.h"
#import "common_private.h"

@interface LanguageUnderstandingModel (Private)

- (std::shared_ptr<IntentImpl::LanguageUnderstandingModel>)getModelHandle;

@end

#endif /* language_understanding_model_private_h */
