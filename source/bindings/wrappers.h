#pragma once

#include <future>
#include <memory>
#include <functional>


template<class T>
class FutureWrapper
{
public:
    FutureWrapper() = default;
    FutureWrapper(const FutureWrapper&) = default;

    FutureWrapper(std::future<T>&& future)
        : m_future(std::make_shared<std::future<T>>(std::move(future)))
    {}

    ~FutureWrapper() = default;

    T Get()
    {
        if (m_future->valid())
        {
            return m_future->get();
        }
        throw std::future_error(std::future_errc::no_state);
    }
private:
    std::shared_ptr<std::future<T>> m_future;
};

template<class T>
class CallbackWrapper
{
public:
#if defined(SWIGJAVA) || defined(SWIGCSHARP)
    virtual void Execute(T eventArgs) const = 0;
#else
    virtual void operator()(T eventArgs) const = 0;
#endif

    virtual ~CallbackWrapper() = default;

    std::function<void(T eventArgs)> GetFunction()
    {
#if defined(SWIGJAVA) || defined(SWIGCSHARP)
        return std::bind(&CallbackWrapper::Execute, this, std::placeholders::_1);
#else
        return std::bind(&CallbackWrapper::operator(), this, std::placeholders::_1);
#endif
    }
};
