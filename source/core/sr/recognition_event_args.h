#pragma once
#include "ispxinterfaces.h"
#include "interface_helpers.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


class CSpxRecognitionEventArgs :
    public ISpxRecognitionEventArgs, 
    public ISpxRecognitionEventArgsInit
{
public:

    CSpxRecognitionEventArgs();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxRecognitionEventArgs)
        SPX_INTERFACE_MAP_ENTRY(ISpxRecognitionEventArgsInit)
    SPX_INTERFACE_MAP_END()

    // --- ISpxRecognitionEventArgs
    virtual const std::wstring& GetSessionId() override;
    virtual std::shared_ptr<ISpxRecognitionResult> GetResult() override;
    virtual const uint64_t& GetOffset() override;

    // --- ISpxRecognitionEventArgsInit
    virtual void Init(const std::wstring& sessionId, std::shared_ptr<ISpxRecognitionResult> result) override;
    virtual void Init(const std::wstring& sessionId, uint64_t offset) override;


private:

    CSpxRecognitionEventArgs(const CSpxRecognitionEventArgs&) = delete;
    CSpxRecognitionEventArgs(const CSpxRecognitionEventArgs&&) = delete;

    CSpxRecognitionEventArgs& operator=(const CSpxRecognitionEventArgs&) = delete;

    std::wstring m_sessionId;
    std::shared_ptr<ISpxRecognitionResult> m_result;
    uint64_t m_offset;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl
