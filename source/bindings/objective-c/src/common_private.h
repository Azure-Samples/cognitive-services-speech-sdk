//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import <Foundation/Foundation.h>
#import "nsstring_stdstring.h"

// The C++ headers
#define BINDING_OBJECTIVE_C
#import "speechapi_cxx.h"

namespace SpeechImpl = Microsoft::CognitiveServices::Speech;
namespace TranslationImpl = Microsoft::CognitiveServices::Speech::Translation;
namespace IntentImpl = Microsoft::CognitiveServices::Speech::Intent;
namespace AudioImpl = Microsoft::CognitiveServices::Speech::Audio;

using RecognizerSharedPtr = std::shared_ptr<SpeechImpl::Recognizer>;
using SpeechRecoSharedPtr = std::shared_ptr<SpeechImpl::SpeechRecognizer>;
using SpeechSynthSharedPtr = std::shared_ptr<SpeechImpl::SpeechSynthesizer>;
using TranslationRecoSharedPtr = std::shared_ptr<TranslationImpl::TranslationRecognizer>;
using IntentRecoSharedPtr = std::shared_ptr<IntentImpl::IntentRecognizer>;
