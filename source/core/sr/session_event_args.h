#pragma once
#include "ispxinterfaces.h"
#include "interface_helpers.h"


namespace CARBON_IMPL_NAMESPACE() {


class CSpxSessionEventArgs :
    public ISpxSessionEventArgs, 
    public ISpxSessionEventArgsInit
{
public:

    CSpxSessionEventArgs();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxSessionEventArgs)
        SPX_INTERFACE_MAP_ENTRY(ISpxSessionEventArgsInit)
    SPX_INTERFACE_MAP_END()


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
