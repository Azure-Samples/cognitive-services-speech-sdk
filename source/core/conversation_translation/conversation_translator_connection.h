//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once

#include <ispxinterfaces.h>
#include <interface_helpers.h>
#include "ISpxConversationInterfaces.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {
namespace ConversationTranslation {

    class CSpxConversationTranslatorConnection :
        public ISpxConversationTranslatorConnection,
        public ISpxConnection,
        public ISpxMessageParamFromUser
    {
    public:
        CSpxConversationTranslatorConnection();
        virtual ~CSpxConversationTranslatorConnection();

        SPX_INTERFACE_MAP_BEGIN()
            SPX_INTERFACE_MAP_ENTRY(ISpxConversationTranslatorConnection)
            SPX_INTERFACE_MAP_ENTRY(ISpxConnection)
            SPX_INTERFACE_MAP_ENTRY(ISpxMessageParamFromUser)
            SPX_INTERFACE_MAP_END()

        // --- ISpxConversationTranslatorConnectionInit
        virtual void Init(std::weak_ptr<ISpxConversationTranslator> convTrans, std::weak_ptr<ISpxRecognizer> reco) override;

        // --- ISpxConnection
        virtual void Open(bool forContinuousRecognition) override;
        virtual void Close() override;
        virtual std::shared_ptr<ISpxRecognizer> GetRecognizer() override;
        virtual std::shared_ptr<ISpxConversationTranslator> GetConversationTranslator() override;

        // --- ISpxUspMessageParamFromUser
        virtual void SetParameter(std::string&& path, std::string&& name, std::string&& value) override;
        virtual void SendNetworkMessage(std::string&& path, std::string&& payload) override;
        virtual void SendNetworkMessage(std::string&& path, std::vector<uint8_t>&& payload) override;

    private:
        DISABLE_COPY_AND_MOVE(CSpxConversationTranslatorConnection);

    private:
        std::weak_ptr<ISpxConversationTranslator> m_convTrans;
        std::weak_ptr<ISpxRecognizer> m_reco;
    };

}}}}} // Microsoft::CognitiveServices::Speech::Impl::Conversation
