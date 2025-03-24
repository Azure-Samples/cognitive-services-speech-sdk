//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//
#pragma once

#include "traits.h"

namespace Microsoft {
namespace SpeechSDK {
namespace Standalone {
namespace Intent {
namespace Impl {

template<typename T>
class Maybe
{
public:
    Maybe(): m_placeholder{ nullptr }, m_hasValue{ false }
    {}

    Maybe(std::nullptr_t): m_placeholder{ nullptr }, m_hasValue{ false }
    {}

    Maybe(T value): m_value{ std::move(value) }, m_hasValue{ true }
    {}

    Maybe(const Maybe<T>& other): m_hasValue{ other.m_hasValue }
    {
        if (other)
        {
            new(&m_value) T(other.m_value);
        }
        else
        {
            m_placeholder = nullptr;
        }
    }

    Maybe(Maybe<T>&& other) noexcept : m_hasValue{ std::move(other.m_hasValue) }
    {
        if (m_hasValue)
        {
            new(&m_value) T(std::move(other.m_value));
        }
        else
        {
            m_placeholder = nullptr;
        }
    }

    Maybe& operator=(const Maybe& rhs)
    {
        if (this != &rhs)
        {
            auto hadValue = m_hasValue;
            m_hasValue = rhs.m_hasValue;
            if (rhs)
            {
                if (hadValue)
                {
                    m_value = rhs.m_value;
                }
                else
                {
                    new(&m_value) T(rhs.m_value);
                }
            }
            else
            {
                if (hadValue)
                {
                    m_value.~T();
                }
                m_placeholder = nullptr;
            }
        }
        return *this;
    }

    Maybe& operator=(Maybe&& rhs)
    {
        if (this != &rhs)
        {
            auto hadValue = m_hasValue;
            m_hasValue = rhs.m_hasValue;
            if (rhs)
            {
                if (hadValue)
                {
                    m_value = std::move(rhs.m_value);
                }
                else
                {
                    new(&m_value) T(std::move(rhs.m_value));
                }
            }
            else
            {
                if (hadValue)
                {
                    m_value.~T();
                }
                m_placeholder = nullptr;
            }
        }
        return *this;
    }

    ~Maybe()
    {
        if (m_hasValue)
        {
            m_value.~T();
        }
    }

    operator bool() const&
    {
        return m_hasValue;
    }

    const T& Get() const &
    {
        if (!m_hasValue)
        {
            std::abort();
        }
        return m_value;
    }

    T& Get() &
    {
        if (!m_hasValue)
        {
            std::abort();
        }
        return m_value;
    }

    const T&& Get() const &&
    {
        if (!m_hasValue)
        {
            std::abort();
        }
        return std::move(m_value);
    }

    T&& Get() &&
    {
        if (!m_hasValue)
        {
            std::abort();
        }
        return std::move(m_value);
    }

    const T * operator->() const
    {
        if (!m_hasValue)
        {
            std::abort();
        }
        return &m_value;
    }

    T * operator->()
    {
        if (!m_hasValue)
        {
            std::abort();
        }
        return &m_value;
    }

    template<typename U, std::enable_if_t<std::is_convertible<U, T>::value, int> = 0>
    T GetOr(U&& defaultValue) const &
    {
        if (!m_hasValue)
        {
            return static_cast<T>(std::forward<U>(defaultValue));
        }
        return m_value;
    }

    template<typename U, std::enable_if_t<std::is_convertible<U, T>::value, int> = 0>
    T GetOr(U&& defaultValue) &&
    {
        if (!m_hasValue)
        {
            return static_cast<T>(std::forward<U>(defaultValue));
        }
        return std::move(m_value);
    }

    template<typename U, typename F>
    Maybe<U> Map(F mapFn)
    {
        if (!m_hasValue)
        {
            return Maybe<U>{};
        }
        return mapFn(m_value);
    }

    template<typename Exception, typename F, std::enable_if_t<MatchesSignature<F, Exception>::value, int> = 0>
    T Unwrap(F onEmpty)
    {
        if (m_hasValue)
        {
            return std::move(m_value);
        }
        throw onEmpty();
    }

private:
    union
    {
        T m_value;
        void* m_placeholder;
    };
    bool m_hasValue;
};


}}}}}
