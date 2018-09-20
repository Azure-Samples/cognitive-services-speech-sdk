//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "SPXPropertyCollection.h"

@interface RecognizerPropertyCollection : NSObject <SPXPropertyCollection>

-(instancetype)initWithPropertyCollection :(SpeechImpl::PropertyCollection *)propertiesHandle from:(RecognizerSharedPtr)recoHandle;

@end