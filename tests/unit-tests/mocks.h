//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
#pragma once

#include <algorithm>
#include <memory>
#include <functional>
#include <stdexcept>
#include <type_traits>

#include <ispxinterfaces.h>
#include <service_provider_impl.h>
#include <property_bag_impl.h>
#include <site_helpers.h>
#include <interface_helpers.h>

#include "unit_test_utils.h"

class MockSite:
    public Carbon::ISpxGenericSite,
    public Carbon::ISpxServiceProviderImpl
{
public:
    MockSite() = default;

    template<typename I, typename T, std::enable_if_t<std::is_base_of<Carbon::ISpxInterfaceBase, T>::value, int> = 0>
    void AddService(std::shared_ptr<T> ptr)
    {
        Carbon::ISpxServiceProviderImpl::AddService(typeid(I).name(), ptr);
    }

    void AddService(const char* serviceName, std::shared_ptr<ISpxInterfaceBase> service) final
    {
        Carbon::ISpxServiceProviderImpl::AddService(serviceName, service);
    }

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(Carbon::ISpxGenericSite)
        SPX_INTERFACE_MAP_ENTRY(Carbon::ISpxServiceProvider)
    SPX_INTERFACE_MAP_END()
};

class MockAudioProcessorNotifyMe : public Carbon::ISpxAudioProcessorNotifyMe
{
public:
    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(Carbon::ISpxAudioProcessorNotifyMe)
    SPX_INTERFACE_MAP_END()

    MockAudioProcessorNotifyMe() = default;

    inline void NotifyMe(const std::shared_ptr<Carbon::ISpxAudioProcessor>& ptr) final
    {
        CallIfNotEmpty(NotifyMeHandler, ptr);
    }

    std::function<void(const std::shared_ptr<Carbon::ISpxAudioProcessor>&)> NotifyMeHandler;

};

class MockBufferProperties :
    public Carbon::ISpxBufferProperties
{
public:
    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(Carbon::ISpxBufferProperties)
    SPX_INTERFACE_MAP_END()

    MockBufferProperties() = default;

    inline void SetBufferProperty(const char* name, const char* value) final
    {
        CallIfNotEmpty(SetBufferPropertyHandler, name, value);
    }

    inline PropertyValue_Type GetBufferProperty(const char*, const char*) final
    {
        throw std::runtime_error{ "Not implemented" };
    }

    inline PropertyValue_Type GetBufferProperty(const char*, Carbon::OffsetType, int, Carbon::OffsetType*) final
    {
        throw std::runtime_error{ "Not implemented" };
    }

    inline std::list<FoundPropertyData_Type> GetBufferProperties(const char*, Carbon::OffsetType, Carbon::OffsetType) final
    {
        throw std::runtime_error{ "Not implemented" };
    }

    inline std::list<FoundPropertyData_Type> GetBufferProperties(Carbon::OffsetType, Carbon::OffsetType) final
    {
        throw std::runtime_error{ "Not implemented" };
    }

    std::function<void(const char*, const char*)> SetBufferPropertyHandler;
};

class MockBufferDataWriter:
    public Carbon::ISpxBufferDataWriter
{
public:
    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(Carbon::ISpxBufferDataWriter)
    SPX_INTERFACE_MAP_END()

    MockBufferDataWriter() = default;

    inline void Write(uint8_t* buffer, uint32_t size) final
    {
        CallIfNotEmpty(WriteHandler, buffer, size);
    }

    std::function<void(uint8_t*, uint32_t)> WriteHandler;
};

class MockBufferData :
    public Carbon::ISpxBufferData
{
public:
    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(Carbon::ISpxBufferData)
    SPX_INTERFACE_MAP_END()

    MockBufferData() = default;

    inline Carbon::OffsetType GetOffset() final
    {
        return CallIfNotEmptyR(GetOffsetHandler, 0);
    }

    inline Carbon::OffsetType GetNewMultiReaderOffset() final
    {
        return CallIfNotEmptyR(GetNewMultiReaderOffsetHandler, 0);
    }

    inline uint32_t Read(uint8_t* buffer, uint32_t size) final
    {
        return CallIfNotEmptyR(ReadHandler, 0, buffer, size);
    }

    inline uint32_t ReadAt(uint64_t offset, uint8_t* buffer, uint32_t size) final
    {
        return CallIfNotEmptyR(ReadAtHandler, 0, offset, buffer, size);
    }

    inline uint64_t GetBytesDead() final
    {
        return CallIfNotEmptyR(GetBytesDeadHandler, 0);
    }

    inline uint64_t GetBytesRead() final
    {
        return CallIfNotEmptyR(GetBytesReadHandler, 0);
    }

    inline uint64_t GetBytesReady() final
    {
        return CallIfNotEmptyR(GetBytesReadyHandler, 0);
    }

    inline uint64_t GetBytesReadyMax() final
    {
        return CallIfNotEmptyR(GetBytesReadyMaxHandler, 0);
    }

    std::function<uint64_t()> GetOffsetHandler;
    std::function<uint64_t()> GetNewMultiReaderOffsetHandler;
    std::function<uint32_t(uint8_t*, uint32_t)> ReadHandler;
    std::function<uint32_t(uint64_t, uint8_t*, uint32_t)> ReadAtHandler;
    std::function<uint64_t()> GetBytesDeadHandler;
    std::function<uint64_t()> GetBytesReadHandler;
    std::function<uint64_t()> GetBytesReadyHandler;
    std::function<uint64_t()> GetBytesReadyMaxHandler;
};

class MockAudioSourceNotifyMe :
    public Carbon::ISpxAudioSourceNotifyMe
{
public:
    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(Carbon::ISpxAudioSourceNotifyMe)
    SPX_INTERFACE_MAP_END()

    MockAudioSourceNotifyMe() = default;

    inline void NotifyMe(const std::shared_ptr<Carbon::ISpxAudioSource>& source, const std::shared_ptr<Carbon::ISpxBufferData>& buffer) final
    {
        CallIfNotEmpty(NotifyMeHandler, source, buffer);
    }

    std::function<void(const std::shared_ptr<Carbon::ISpxAudioSource>&, const std::shared_ptr<Carbon::ISpxBufferData>&)> NotifyMeHandler;
};

class MockNamedProperties :
    public Carbon::ISpxPropertyBagImpl
{
public:
    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(Carbon::ISpxInterfaceBase)
        SPX_INTERFACE_MAP_ENTRY(Carbon::ISpxNamedProperties)
    SPX_INTERFACE_MAP_END()
};

class MockInteractiveMicrophone :
    public Carbon::ISpxAudioPump,
    public Carbon::ISpxObjectWithSiteInitImpl<Carbon::ISpxGenericSite>
{
public:
    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(Carbon::ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(Carbon::ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(Carbon::ISpxAudioPump)
    SPX_INTERFACE_MAP_END()

    MockInteractiveMicrophone() = default;

    inline uint16_t DefaultGetFormat(Carbon::SPXWAVEFORMATEX* format, uint16_t size) const
    {
        if (format == nullptr)
        {
            return static_cast<uint16_t>(m_format.size());
        }
        std::memcpy(format, m_format.get(), std::min(static_cast<size_t>(size), m_format.size()));
        return static_cast<uint16_t>(m_format.size());
    }

    inline uint16_t GetFormat(Carbon::SPXWAVEFORMATEX* format, uint16_t size) const final
    {
        if (GetFormatHandler)
        {
            return GetFormatHandler(format, size);
        }
        return DefaultGetFormat(format, size);
    }

    inline void SetFormat(const Carbon::SPXWAVEFORMATEX* format, uint16_t size) final
    {
        m_format = Carbon::SpxWaveFormatEx{ size };
        std::memcpy(m_format.get(), format, size);
    }

    inline void StartPump(std::shared_ptr<Carbon::ISpxAudioProcessor> processor) final
    {
        CallIfNotEmpty(StartPumpHandler, std::move(processor));
    }

    inline void PausePump() final
    {
        CallIfNotEmpty(PausePumpHandler);
    }

    inline void StopPump() final
    {
        CallIfNotEmpty(StopPumpHandler);
    }

    inline Carbon::ISpxAudioPump::State GetState() const final
    {
        return m_state;
    }

    inline void SetState(Carbon::ISpxAudioPump::State state)
    {
        m_state = state;
    }

    inline std::string GetPropertyValue(const std::string& key) const final
    {
        return CallIfNotEmptyR(GetPropertyValueHandler, "", key);
    }

    std::function<uint16_t(Carbon::SPXWAVEFORMATEX *, uint16_t size)> GetFormatHandler;
    std::function<void(std::shared_ptr<Carbon::ISpxAudioProcessor>)> StartPumpHandler;
    std::function<void()> PausePumpHandler;
    std::function<void()> StopPumpHandler;
    std::function<std::string(const std::string&)> GetPropertyValueHandler;
private:
    Carbon::SpxWaveFormatEx m_format{ ConstructDefaultFormat() };
    Carbon::ISpxAudioPump::State m_state{ Carbon::ISpxAudioPump::State::Idle };
};

class MockAudioProcessor :
    public Carbon::ISpxAudioProcessor
{
public:
    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(Carbon::ISpxAudioProcessor)
    SPX_INTERFACE_MAP_END()

    void SetFormat(const Carbon::SPXWAVEFORMATEX* format) final
    {
        CallIfNotEmpty(SetFormatHandler, format);
    }

    void ProcessAudio(const Carbon::DataChunkPtr& audioChunk) final
    {
        CallIfNotEmpty(ProcessAudioHandler, audioChunk);
    }

    std::function<void(const Carbon::SPXWAVEFORMATEX*)> SetFormatHandler;
    std::function<void(const Carbon::DataChunkPtr&)> ProcessAudioHandler;
};

class ObjectFactoryProxy :
    public Carbon::ISpxObjectFactory
{
public:
    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(Carbon::ISpxInterfaceBase)
        SPX_INTERFACE_MAP_ENTRY(Carbon::ISpxObjectFactory)
    SPX_INTERFACE_MAP_END()

    void RegisterOverride(std::string className, std::function<void*()> fn)
    {
        m_overrides.emplace(std::move(className), fn);
    }

    void* CreateObject(const char* className, const char* interfaceName) final
    {
        auto found = m_overrides.find(className);
        if (found != m_overrides.end())
        {
            auto fn = found->second;
            return fn();
        }
        auto factory = Carbon::SpxQueryInterface<Carbon::ISpxObjectFactory>(Carbon::SpxGetNonCoreRootSite());
        return factory->CreateObject(className, interfaceName);
    }
private:
    std::map<std::string, std::function<void* ()>, std::less<>> m_overrides;
};

