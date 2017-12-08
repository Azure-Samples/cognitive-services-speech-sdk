#pragma once
#include "ispxinterfaces.h"


namespace CARBON_IMPL_NAMESPACE() {


class CSpxRecognitionEventArgs : public ISpxRecognitionEventArgs
{
public:

    CSpxRecognitionEventArgs(const std::wstring& sessionId, std::shared_ptr<ISpxRecognitionResult> result);

    // --- ISpxRecognitionEventArgs
    
    virtual const std::wstring& GetSessionId() override;
    virtual std::shared_ptr<ISpxRecognitionResult> GetResult() override;


private:
    
    CSpxRecognitionEventArgs(const CSpxRecognitionEventArgs&) = delete;
    CSpxRecognitionEventArgs(const CSpxRecognitionEventArgs&&) = delete;

    CSpxRecognitionEventArgs& operator=(const CSpxRecognitionEventArgs&) = delete;

    const std::wstring m_sessionId;
    std::shared_ptr<ISpxRecognitionResult> m_result;
};


}; // CARBON_IMPL_NAMESPACE()
