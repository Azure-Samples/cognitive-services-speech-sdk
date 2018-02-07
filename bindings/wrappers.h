#pragma once

#include <future>
#include <memory>


template<class T>
class FutureWrapper {
public:
    FutureWrapper() = default;
    FutureWrapper(const FutureWrapper&) = default;

    FutureWrapper(std::future<T>&& future)
        : m_future(std::make_shared<std::future<T>>(std::move(future)))
    {}

    T Get() { return m_future->get(); }
private:
    std::shared_ptr<std::future<T>> m_future;
};
