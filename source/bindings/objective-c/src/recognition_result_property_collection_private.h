//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef recognition_result_property_collection_h
#define recognition_result_property_collection_h

#import <Foundation/Foundation.h>
#import "property_collection.h"

@interface RecognitionResultPropertyCollection : NSObject <PropertyCollection>

-(instancetype)initFrom :(std::shared_ptr<SpeechImpl::RecognitionResult>)resultHandle;

@end

#endif /* recognition_result_property_collection_h */
