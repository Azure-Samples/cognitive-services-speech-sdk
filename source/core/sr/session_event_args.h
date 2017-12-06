#pragma once
#include "ispxinterfaces.h"


namespace CARBON_IMPL_NAMESPACE() {


class CSpxSessionEventArgs : public ISpxSessionEventArgs
{
public:

    CSpxSessionEventArgs(const std::wstring& sessionId, std::shared_ptr<ISpxRecognitionResult> result);

    // --- ISpxSessionEventArgs
    
    virtual std::wstring GetSessionId() override;


private:

    CSpxSessionEventArgs(const CSpxSessionEventArgs&) = delete;
    CSpxSessionEventArgs(const CSpxSessionEventArgs&&) = delete;

    CSpxSessionEventArgs& operator=(const CSpxSessionEventArgs&) = delete;

    std::wstring m_sessionId;
};


}; // CARBON_IMPL_NAMESPACE()
