//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef common_private_h
#define common_private_h

#import <Foundation/Foundation.h>
#import "NSString_STL.h"

// The C++ headers
#import "speechapi_cxx.h"

namespace SpeechImpl = Microsoft::CognitiveServices::Speech;

using SpeechRecoSharedPtr = std::shared_ptr<SpeechImpl::SpeechRecognizer>;

#endif /* common_private_h */
