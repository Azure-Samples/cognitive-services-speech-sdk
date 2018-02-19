#pragma once
#include "ispxinterfaces.h"


namespace CARBON_IMPL_NAMESPACE() {


class CSpxSessionEventArgs :
    public ISpxSessionEventArgs, 
    public ISpxSessionEventArgsInit
{
public:

    CSpxSessionEventArgs();

    // --- ISpxSessionEventArgs
    
    virtual const std::wstring& GetSessionId() override;

    // --- ISpxSessionEventArgsInit

    virtual void Init(const std::wstring& sessionId) override;


private:

    CSpxSessionEventArgs(const CSpxSessionEventArgs&) = delete;
    CSpxSessionEventArgs(const CSpxSessionEventArgs&&) = delete;

    CSpxSessionEventArgs& operator=(const CSpxSessionEventArgs&) = delete;

    std::wstring m_sessionId;
};


} // CARBON_IMPL_NAMESPACE
