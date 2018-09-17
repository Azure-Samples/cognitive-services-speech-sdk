//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef recognizer_property_collection_private_h
#define recognizer_property_collection_private_h

#import "property_collection.h"

@interface RecognizerPropertyCollection : NSObject <PropertyCollection>

-(instancetype)initWithPropertyCollection :(SpeechImpl::PropertyCollection *)propertiesHandle from:(RecognizerSharedPtr)recoHandle;

@end

#endif /* recognizer_property_collection_private_h */
