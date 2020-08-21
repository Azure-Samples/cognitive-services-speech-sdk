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
namespace TranscriptionImpl = Microsoft::CognitiveServices::Speech::Transcription;
namespace KeywordImpl = Microsoft::CognitiveServices::Speech;

using RecognizerSharedPtr = std::shared_ptr<SpeechImpl::Recognizer>;
using SpeechRecoSharedPtr = std::shared_ptr<SpeechImpl::SpeechRecognizer>;
using SpeechSynthSharedPtr = std::shared_ptr<SpeechImpl::SpeechSynthesizer>;
using TranslationRecoSharedPtr = std::shared_ptr<TranslationImpl::TranslationRecognizer>;
using IntentRecoSharedPtr = std::shared_ptr<IntentImpl::IntentRecognizer>;
using KeywordRecoSharedPtr = std::shared_ptr<KeywordImpl::KeywordRecognizer>;
using UserSharedPtr = std::shared_ptr<TranscriptionImpl::User>;
using ParticipantSharedPtr = std::shared_ptr<TranscriptionImpl::Participant>;
using ConversationSharedPtr = std::shared_ptr<TranscriptionImpl::Conversation>;
using ConversationTranscriberSharedPtr = std::shared_ptr<TranscriptionImpl::ConversationTranscriber>;
using ConversationTranslatorSharedPtr = std::shared_ptr<TranscriptionImpl::ConversationTranslator>;
