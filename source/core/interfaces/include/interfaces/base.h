//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
#pragma once

#include <typeinfo>
#define SpxTypeName(x)  typeid(x).name()

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


class ISpxInterfaceBase : public std::enable_shared_from_this<ISpxInterfaceBase>
{
public:

    virtual ~ISpxInterfaceBase() = default;

    template <class I>
    std::shared_ptr<I> QueryInterface()
    {
        return QueryInterfaceInternal<I>();
    }

protected:

    template <class I>
    std::shared_ptr<I> QueryInterfaceInternal()
    {
        // try to query for the interface via our virtual method...
        auto ptr = QueryInterface(SpxTypeName(I));
        if (ptr != nullptr)
        {
            auto interfacePtr = reinterpret_cast<I*>(ptr);
            return interfacePtr->shared_from_this();
        }

        // if that fails, let the caller know
        return nullptr;
    }

    virtual void* QueryInterface(const char* /*interfaceName*/) { return nullptr; }

    typedef std::enable_shared_from_this<ISpxInterfaceBase> base_type;

    std::shared_ptr<ISpxInterfaceBase> shared_from_this()
    {
        return base_type::shared_from_this();
    }

    std::shared_ptr<const ISpxInterfaceBase> shared_from_this() const
    {
        return base_type::shared_from_this();
    }
};

template<typename T>
struct ISpxInterfaceBaseFor : virtual public ISpxInterfaceBase
{
public:
    virtual ~ISpxInterfaceBaseFor() = default;

    std::shared_ptr<T> shared_from_this()
    {
        std::shared_ptr<T> result(base_type::shared_from_this(), static_cast<T*>(this));
        return result;
    }

    std::shared_ptr<const T> shared_from_this() const
    {
        std::shared_ptr<const T> result(base_type::shared_from_this(), static_cast<const T*>(this));
        return result;
    }

private:
    typedef ISpxInterfaceBase base_type;

    ISpxInterfaceBaseFor&& operator =(const ISpxInterfaceBaseFor&&) = delete;
};

} } } }