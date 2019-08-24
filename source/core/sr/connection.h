#pragma once

#include <memory>
#include "ispxinterfaces.h"
#include "interface_helpers.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class CSpxConnection :
    public ISpxConnection,
    public ISpxConnectionInit,
    public ISpxMessageParamFromUser
{
public:

    CSpxConnection();
    virtual ~CSpxConnection();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxConnection)
        SPX_INTERFACE_MAP_ENTRY(ISpxConnectionInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxMessageParamFromUser)
    SPX_INTERFACE_MAP_END()

    // --- ISpxConnection
    void Open(bool forContinuousRecognition) override;
    void Close() override;
    std::shared_ptr<ISpxRecognizer> GetRecognizer() override;

    // -- ISpxConnectionInit
    void Init(std::weak_ptr<ISpxRecognizer> recognizer, std::weak_ptr<ISpxMessageParamFromUser> setter) override;

    // --- ISpxUspMessageParamFromUser
    void SetParameter(std::string&& path, std::string&& name, std::string&& value) override;
    void SendNetworkMessage(std::string&& path, std::string&& payload) override;

private:

    CSpxConnection(const CSpxConnection&) = delete;
    CSpxConnection(const CSpxConnection&&) = delete;

    CSpxConnection& operator=(const CSpxConnection&) = delete;

    std::weak_ptr<ISpxRecognizer> m_recognizer;
    std::weak_ptr<ISpxMessageParamFromUser> m_setMessageParamFromUser;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl
