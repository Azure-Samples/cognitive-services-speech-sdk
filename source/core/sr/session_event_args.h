#pragma once
#include "ispxinterfaces.h"


namespace CARBON_IMPL_NAMESPACE() {


class CSpxSessionEventArgs : public ISpxSessionEventArgs
{
public:

    CSpxSessionEventArgs(const std::wstring& sessionId);

    // --- ISpxSessionEventArgs
    
    virtual const std::wstring& GetSessionId() override;


private:

    CSpxSessionEventArgs(const CSpxSessionEventArgs&) = delete;
    CSpxSessionEventArgs(const CSpxSessionEventArgs&&) = delete;

    CSpxSessionEventArgs& operator=(const CSpxSessionEventArgs&) = delete;

    const std::wstring m_sessionId;
};


}; // CARBON_IMPL_NAMESPACE()
