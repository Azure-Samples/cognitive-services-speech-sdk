//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef common_private_h
#define common_private_h

#import <Foundation/Foundation.h>
#import "nsstring_stdstring.h"

// The C++ headers
#define BINDING_OBJECTIVE_C
#import "speechapi_cxx.h"

namespace SpeechImpl = Microsoft::CognitiveServices::Speech;
namespace TranslationImpl = Microsoft::CognitiveServices::Speech::Translation;

using SpeechRecoSharedPtr = std::shared_ptr<SpeechImpl::SpeechRecognizer>;
using TranslationRecoSharedPtr = std::shared_ptr<TranslationImpl::TranslationRecognizer>;

#endif /* common_private_h */
