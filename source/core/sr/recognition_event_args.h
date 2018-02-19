#pragma once
#include "ispxinterfaces.h"


namespace CARBON_IMPL_NAMESPACE() {


class CSpxRecognitionEventArgs :
    public ISpxRecognitionEventArgs, 
    public ISpxRecognitionEventArgsInit
{
public:

    CSpxRecognitionEventArgs();

    // --- ISpxRecognitionEventArgs
    
    virtual const std::wstring& GetSessionId() override;
    virtual std::shared_ptr<ISpxRecognitionResult> GetResult() override;

    // --- ISpxRecognitionEventArgsInit

    virtual void Init(const std::wstring& sessionId, std::shared_ptr<ISpxRecognitionResult> result) override;


private:
    
    CSpxRecognitionEventArgs(const CSpxRecognitionEventArgs&) = delete;
    CSpxRecognitionEventArgs(const CSpxRecognitionEventArgs&&) = delete;

    CSpxRecognitionEventArgs& operator=(const CSpxRecognitionEventArgs&) = delete;

    std::wstring m_sessionId;
    std::shared_ptr<ISpxRecognitionResult> m_result;
};


} // CARBON_IMPL_NAMESPACE
