#pragma once
#include <string>
#include <speechapi_cxx_common.h>
#include <speechapi_cxx_todo_recognition.h>

namespace CARBON_NAMESPACE_ROOT :: Recognition {

class RecognitionResult
{
public:

    virtual ~RecognitionResult() = 0;

    const std::wstring& ResultId;
    const RecognitionReason Reason;
    
    const std::wstring& Text;

    const PayloadItems& Payload;
   
    
protected:

    RecognitionResult(const std::wstring& resultId,const RecognitionReason reason,const std::wstring& text,const PayloadItems& payload) :
        ResultId(resultId), Reason(reason), Text(text), Payload(payload)
    {
    };
    

private:

    RecognitionResult(const RecognitionResult&) = delete;
    RecognitionResult(const RecognitionResult&&) = delete;

    RecognitionResult& operator=(const RecognitionResult&) = delete;
};

}; // CARBON_NAMESPACE_ROOT :: Recognition
