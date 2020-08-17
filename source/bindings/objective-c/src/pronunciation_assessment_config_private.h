//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "SPXPronunciationAssessmentConfiguration.h"
#import "common_private.h"

@interface SPXPronunciationAssessmentConfiguration (Private)

- (instancetype)initWithImpl:(std::shared_ptr<SpeechImpl::PronunciationAssessmentConfig>) configImpl;
- (std::shared_ptr<SpeechImpl::PronunciationAssessmentConfig>)getHandle;

@end
