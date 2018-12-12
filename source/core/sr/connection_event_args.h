#pragma once
#include "ispxinterfaces.h"
#include "interface_helpers.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


class CSpxConnectionEventArgs :
    public ISpxConnectionEventArgs,
    public ISpxConnectionEventArgsInit
{
public:

    CSpxConnectionEventArgs();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxConnectionEventArgs)
        SPX_INTERFACE_MAP_ENTRY(ISpxConnectionEventArgsInit)
    SPX_INTERFACE_MAP_END()


    // --- ISpxConnectionEventArgs

    virtual const std::wstring& GetSessionId() override;

    // --- ISpxConnectionEventArgsInit

    virtual void Init(const std::wstring& sessionId) override;


private:

    CSpxConnectionEventArgs(const CSpxConnectionEventArgs&) = delete;
    CSpxConnectionEventArgs(const CSpxConnectionEventArgs&&) = delete;

    CSpxConnectionEventArgs& operator=(const CSpxConnectionEventArgs&) = delete;

    std::wstring m_sessionId;
};

} } } } // Microsoft::CognitiveServices::Speech::Impl

