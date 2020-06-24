//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
// speechapi_cxx_utils.h: General utility classes and functions.
//
#pragma once

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Utils {

/// <summary>
/// Base class that disables the copy constructor
/// </summary>
struct NonCopyable
{
    /// <summary>
    /// Default destructor.
    /// </summary>
    NonCopyable() = default;

    /// <summary>
    /// Virtual destructor.
    /// </summary>
    virtual ~NonCopyable() = default;

    /// <summary>
    /// Disable copy constructor.
    /// </summary>
    NonCopyable(const NonCopyable&) = delete;

    /// <summary>
    /// Disable copy assignment operator.
    /// </summary>
    /// <returns>Reference to the object.</returns>
    NonCopyable& operator=(const NonCopyable &) = delete;
};

/// <summary>
/// Base class that disables the move constructor
/// </summary>
struct NonMovable
{
    /// <summary>
    /// Default destructor.
    /// </summary>
    NonMovable() = default;

    /// <summary>
    /// Virtual destructor.
    /// </summary>
    virtual ~NonMovable() = default;

    /// <summary>
    /// Disable move constructor.
    /// </summary>
    NonMovable(NonMovable &&) = delete;

    /// <summary>
    /// Disable move assignment operator.
    /// </summary>
    /// <returns>Reference to the object.</returns>
    NonMovable& operator=(NonMovable &&) = delete;
};

template<typename F, typename... Args>
SPXHANDLE CallFactoryMethodRight(F method, Args&&... args)
{
    SPXHANDLE handle;
    auto hr = method(std::forward<Args>(args)..., &handle);
    SPX_THROW_ON_FAIL(hr);
    return handle;
}

template<typename F, typename... Args>
SPXHANDLE CallFactoryMethodLeft(F method, Args&&... args)
{
    SPXHANDLE handle;
    auto hr = method(&handle, std::forward<Args>(args)...);
    SPX_THROW_ON_FAIL(hr);
    return handle;
}

template<typename T, typename U, typename F>
std::function<void(const EventSignal<const T&>&)> Callback(U* callee, F f)
{
    return [=](const EventSignal<const T&>& evt)
    {
        (callee->*f)(evt);
    };
}

/// <summary>
/// Helper class implementing the scope guard idiom.
/// (The given function will be executed on destruction)
/// </summary>
template<typename F>
class ScopeGuard
{
public:
    ScopeGuard(ScopeGuard&&) = default;
    ScopeGuard(const ScopeGuard&) = delete;

    explicit ScopeGuard(F f): m_fn{ f }
    {}

    ~ScopeGuard()
    {
        m_fn();
    }

private:
    F m_fn;
};

/// <summary>
/// Creates a scope guard with the given function.
/// </summary>
template<typename F>
ScopeGuard<F> MakeScopeGuard(F fn)
{
    return ScopeGuard<F>{ fn };
}

/// <summary>
/// Function that converts a handle to its underlying type.
/// </summary>
/// <typeparam name="Handle">Handle type.</typeparam>
/// <typeparam name="T">Object type.</typeparam>
/// <param name="obj">Object from which to get the handle.</param>
template <typename Handle, typename T>
inline Handle HandleOrInvalid(std::shared_ptr<T> obj)
{
    return obj == nullptr
        ? static_cast<Handle>(SPXHANDLE_INVALID)
        : static_cast<Handle>(*obj.get());
}


template<typename... Ts>
struct TypeList {};

template<typename T, template<typename...> class F, typename L>
struct TypeListIfAny;

template<typename T, template<typename...> class F>
struct TypeListIfAny<T, F, TypeList<>>
{
    static constexpr bool value{ false };
};

template<typename T, template<typename...> class F, typename U, typename... Us>
struct TypeListIfAny<T, F, TypeList<U, Us...>>
{
    static constexpr bool value = F<U, T>::value || Microsoft::CognitiveServices::Speech::Utils::TypeListIfAny<T, F, TypeList<Us...>>::value;
};

} } } }
