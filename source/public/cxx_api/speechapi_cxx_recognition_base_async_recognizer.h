#pragma once
#include <future>
#include <memory>
#include <speechapi_cxx_common.h>
#include <speechapi_cxx_eventsignal.h>
#include <speechapi_cxx_session_eventargs.h>
#include <speechapi_cxx_recognition_base_async_recognizer.h>


namespace CARBON_NAMESPACE_ROOT :: Recognition {


class BaseAsyncRecognizer : public AsyncRecognizer<RecognitionResult, RecognitionEventArgs>
{
public:

    template <class T>
    static std::shared_ptr<BaseAsyncRecognizer> From(const std::shared_ptr<T> &recognizer) 
    {
        static_assert(std::is_base_of<Recognizer, T>::value);
        return nullptr;
    }

};


}; // CARBON_NAMESPACE_ROOT :: Recognition
