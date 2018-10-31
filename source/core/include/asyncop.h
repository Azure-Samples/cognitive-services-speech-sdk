#pragma once
#include <future>
#include <memory>


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


enum AsyncOpState { AOS_Started, AOS_Completed, AOS_Canceled, AOS_Error };

template<class T>
class CSpxAsyncOp
{
public:

    CSpxAsyncOp(CSpxAsyncOp&& other) = default;

    CSpxAsyncOp(std::shared_future<T>& future, AsyncOpState state) :
        Future(future),
        State(state)
    {
    };

    ~CSpxAsyncOp() = default;

    bool WaitFor(uint32_t milliseconds)
    {
        bool completed = false;

        auto status = this->Future.wait_for(std::chrono::milliseconds(milliseconds));
        switch (status)
        {
        case std::future_status::ready:
            completed = true;
            break;

        case std::future_status::timeout:
            completed = false;
            break;

        default:
        case std::future_status::deferred:
            SPX_THROW_HR(SPXERR_NOT_IMPL);
            break;
        }

        return completed;
    }

    std::shared_future<T> Future;
    AsyncOpState State;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl
