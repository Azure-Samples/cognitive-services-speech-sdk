#pragma once
#include <string>
#include <speechapi_cxx_common.h>


namespace CARBON_NAMESPACE_ROOT :: Recognition {
    

class RecognitionEventArgs : public SessionEventArgs
{
public:

    virtual ~RecognitionEventArgs() = 0;


protected:

    RecognitionEventArgs(const std::wstring& sessionId) :
        SessionEventArgs(sessionId)
    {
    };


private:

    RecognitionEventArgs(const RecognitionEventArgs&) = delete;
    RecognitionEventArgs(const RecognitionEventArgs&&) = delete;

    RecognitionEventArgs& operator=(const RecognitionEventArgs&) = delete;
};


}; // CARBON_NAMESPACE_ROOT :: Recognition
