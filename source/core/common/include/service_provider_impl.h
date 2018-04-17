//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// service_provider_impl.h: Implementation declarations for ISpxServiceProviderImpl C++ class
//

#pragma once

#include "spxcore_common.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


class ISpxServiceProviderImpl : public ISpxAddServiceProvider, public ISpxServiceProvider
{
public:

    // --- ISpxServiceProvider
    std::shared_ptr<ISpxInterfaceBase> QueryService(const char* serviceName) override
    {
        return InternalQueryService(serviceName);
    }

    // --- ISpxAddServiceProvider
    void AddService(const char* serviceName, std::shared_ptr<ISpxInterfaceBase> service) override
    {
        return InternalAddService(serviceName, service);
    }


protected:

    template <class I>
    std::shared_ptr<I> InternalQueryService()
    {
        auto service = InternalQueryService(PAL::GetTypeName<I>().c_str());
        return SpxQueryInterface<I>(service);
    }

    std::shared_ptr<ISpxInterfaceBase> InternalQueryService(const char* serviceName)
    {
        SPX_IFTRUE_THROW_HR(serviceName == nullptr, SPXERR_INVALID_ARG);

        auto item = m_services.find(std::string(serviceName));
        if (item != m_services.end())
        {
            return item->second;
        }

        return nullptr;
    }

    template <class T>
    void InternalAddService(std::shared_ptr<T> service)
    {
        SPX_IFTRUE_THROW_HR(service == nullptr, SPXERR_INVALID_ARG);
        InternalAddService(PAL::GetTypeName<T>().c_str(), SpxQueryInterface<ISpxInterfaceBase>(service));
    }

    void InternalAddService(const char* serviceName, std::shared_ptr<ISpxInterfaceBase> service)
    {
        SPX_IFTRUE_THROW_HR(serviceName == nullptr, SPXERR_INVALID_ARG);
        m_services.emplace(serviceName, service);
    }
    

private:

    std::map<std::string, std::shared_ptr<ISpxInterfaceBase>> m_services;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl
