//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "SPXPropertyCollection.h"

@interface PropertyCollection : NSObject <SPXPropertyCollection>

-(instancetype)initWithPropertyCollection :(SpeechImpl::PropertyCollection *)propertiesHandle;

@end

@interface RecognizerPropertyCollection : PropertyCollection

-(instancetype)initWithPropertyCollection :(SpeechImpl::PropertyCollection *)propertiesHandle from:(RecognizerSharedPtr)recoHandle;

@end

@interface SpeechSynthesizerPropertyCollection : PropertyCollection

-(instancetype)initWithPropertyCollection :(SpeechImpl::PropertyCollection *)propertiesHandle from:(SpeechSynthSharedPtr)speechSynthHandle;

@end