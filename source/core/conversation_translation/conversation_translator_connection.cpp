//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "common.h"
#include "conversation_translator_connection.h"
#include "conversation_translator_logging.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {
namespace ConversationTranslation {

    CSpxConversationTranslatorConnection::CSpxConversationTranslatorConnection()
        : m_convTrans(), m_reco()
    {
        SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    }

    CSpxConversationTranslatorConnection::~CSpxConversationTranslatorConnection()
    {
        SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    }

    void CSpxConversationTranslatorConnection::Init(std::weak_ptr<ISpxConversationTranslator> convTrans, std::weak_ptr<ISpxRecognizer> reco)
    {
        m_convTrans = convTrans;
        m_reco = reco;
    }

    void CSpxConversationTranslatorConnection::Open(bool forContinuousRecognition)
    {
        auto convTrans = m_convTrans.lock();
        SPX_IFTRUE_THROW_HR(convTrans == nullptr, SPXERR_RUNTIME_ERROR);

        convTrans->Connect();
        if (forContinuousRecognition)
        {
            convTrans->StartTranscribing();
        }
    }

    void CSpxConversationTranslatorConnection::Close()
    {
        auto convTrans = m_convTrans.lock();
        if (convTrans != nullptr)
        {
            convTrans->Disconnect();
        }
    }

    std::shared_ptr<ISpxRecognizer> CSpxConversationTranslatorConnection::GetRecognizer()
    {
        auto shared = m_reco.lock();
        if (shared == nullptr)
        {
            CT_LOG_WARNING("Connection: Recognizer instance is no longer valid");
        }

        return shared;
    }

    std::shared_ptr<ISpxConversationTranslator> CSpxConversationTranslatorConnection::GetConversationTranslator()
    {
        auto shared = m_convTrans.lock();
        if (shared == nullptr)
        {
            CT_LOG_WARNING("Connection: Conversation translator instance is no longer valid");
        }

        return shared;
    }

    void CSpxConversationTranslatorConnection::SetParameter(std::string && path, std::string && name, std::string && value)
    {
        auto reco = m_reco.lock();
        SPX_IFTRUE_THROW_HR(reco == nullptr, SPXERR_INVALID_RECOGNIZER);

        auto messageParamFromUser = reco->QueryInterface<ISpxMessageParamFromUser>();
        SPX_IFTRUE_THROW_HR(messageParamFromUser == nullptr, SPXERR_INVALID_RECOGNIZER);

        messageParamFromUser->SetParameter(std::move(path), std::move(name), std::move(value));
    }

    void CSpxConversationTranslatorConnection::SendNetworkMessage(std::string && path, std::string && payload)
    {
        auto reco = m_reco.lock();
        SPX_IFTRUE_THROW_HR(reco == nullptr, SPXERR_INVALID_RECOGNIZER);

        auto messageParamFromUser = reco->QueryInterface<ISpxMessageParamFromUser>();
        SPX_IFTRUE_THROW_HR(messageParamFromUser == nullptr, SPXERR_INVALID_RECOGNIZER);

        messageParamFromUser->SendNetworkMessage(std::move(path), std::move(payload));
    }

    void CSpxConversationTranslatorConnection::SendNetworkMessage(std::string && path, std::vector<uint8_t>&& payload)
    {
        auto reco = m_reco.lock();
        SPX_IFTRUE_THROW_HR(reco == nullptr, SPXERR_INVALID_RECOGNIZER);

        auto messageParamFromUser = reco->QueryInterface<ISpxMessageParamFromUser>();
        SPX_IFTRUE_THROW_HR(messageParamFromUser == nullptr, SPXERR_INVALID_RECOGNIZER);

        messageParamFromUser->SendNetworkMessage(std::move(path), std::move(payload));
    }

}}}}} // Microsoft::CognitiveServices::Speech::Impl::Conversation
