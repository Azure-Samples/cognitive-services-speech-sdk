//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import <Foundation/Foundation.h>
#import "SPXPropertyCollection.h"

@interface SpeechSynthesisResultPropertyCollection : NSObject <SPXPropertyCollection>

-(instancetype)initFrom :(std::shared_ptr<SpeechImpl::SpeechSynthesisResult>)resultHandle;

@end
