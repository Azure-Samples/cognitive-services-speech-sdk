//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once

#include <create_object_helpers.h>
#include <event.h>
#include <property_id_2_name_map.h>
#include "conversation_connection.h"
#include "conversation_manager.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {
namespace ConversationTranslation {

    namespace ConversationConstants
    {
        constexpr auto Host = "dev.microsofttranslator.com";
        constexpr auto RestPath = "/capito/room";
        constexpr auto WebSocketPath = "/capito/translate";
        constexpr auto ApiVersion = "2.0";
        constexpr auto ClientTraceIdHeader = "X-ClientTraceId";
        constexpr auto RequestIdHeader = "X-RequestId";
        constexpr auto ClientAppIdHeader = "X-ClientAppId";
        constexpr auto ClientAppId = "F8E43FCD-AB42-4215-9D16-8C5FA34E780D";
        constexpr uint32_t MaxWaitTimeMs = 500;
    }

    namespace ConversationKeys
    {
        // NOTE: If you change this value, please also update the value in source\core\sr\usp_reco_engine_adapter.cpp
        // in the SetUspAuthentication() method.
        constexpr auto Conversation_Token = "ConversationTranslator_Token";

        constexpr auto Conversation_Management_Endpoint = "ConversationTranslator_RestEndpoint";
        constexpr auto Conversation_Endpoint = "ConversationTranslator_Endpoint";
        constexpr auto Conversation_Nickname = "ConversationTranslator_Name";
        constexpr auto Conversation_Region = "ConversationTranslator_Region";
        constexpr auto ConversationServiceConnection_Key = "ConversationTranslator_SubscriptionKey";
        constexpr auto Conversation_ClientId = "ConversationTranslator_ClientId";
        constexpr auto Conversation_RoomPin = "ConversationTranslator_RoomPin";
        constexpr auto Conversation_MultiChannelAudio = "ConversationTranslator_MultiChannelAudio";
    }

    namespace ConversationQueryParameters
    {
        constexpr auto ApiVersion = "api-version";
        constexpr auto AuthTokenHeader = "X-CapitoToken";
        constexpr auto AuthToken = "token";
    }

    class ISpxConversationInternals : public ISpxInterfaceBaseFor<ISpxConversationInternals>
    {
    public:
        Impl::event<> ConversationDeleted;

        virtual bool IsConnected() const = 0;
        virtual bool CanRejoin() const = 0;
        virtual std::shared_ptr<ConversationArgs> GetConversationArgs() const = 0;
        virtual std::shared_ptr<ConversationManager> GetConversationManager() const = 0;
        virtual std::shared_ptr<ConversationConnection> GetConversationConnection() const = 0;
    };

    class ThreadingHelpers : public ISpxObjectWithSiteInitImpl<ISpxRecognizerSite>
    {
    public:
        ThreadingHelpers(ISpxThreadService::Affinity affinity) :
            m_threadService(),
            m_affinity(affinity),
            m_keepSessionAlive()
        {
        }

        ~ThreadingHelpers()
        {
            SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
            SpxTermAndClear(m_keepSessionAlive);
            m_threadService = nullptr;
        }

        virtual void Init()
        {
            SPX_DBG_TRACE_FUNCTION();

            std::shared_ptr<ISpxRecognizerSite> site = GetSite();
            SPX_IFTRUE_THROW_HR(site == nullptr, SPXERR_UNINITIALIZED);
            m_keepSessionAlive = site;

            // get the base site if available
            auto baseSite = SpxQueryInterface<ISpxGenericSite>(site);
            SPX_IFTRUE_THROW_HR(baseSite == nullptr, SPXERR_UNEXPECTED_CONVERSATION_TRANSLATOR_SITE_FAILURE);

            m_threadService = SpxCreateObjectWithSite<ISpxThreadService>("CSpxThreadService", baseSite);
            SPX_IFTRUE_THROW_HR(m_threadService == nullptr, SPXERR_UNEXPECTED_CONVERSATION_SITE_FAILURE);
        }

    protected:
        inline CSpxAsyncOp<void> RunAsynchronously(std::function<void()> func, std::promise<bool>&& executed = std::promise<bool>())
        {
            if (m_threadService == nullptr)
            {
                throw ExceptionWithCallStack(SPXERR_UNINITIALIZED);
            }

            auto keepalive = ISpxInterfaceBase::shared_from_this();
            std::packaged_task<void()> task([keepalive, func]()
            {
                return func();
            });
            auto future = task.get_future().share();
            m_threadService->ExecuteAsync(move(task), m_affinity, std::move(executed));
            return CSpxAsyncOp<void>(future, AsyncOpState::AOS_Started);
        }

        template<typename TRet>
        inline TRet RunSynchronously(std::function<TRet()> func)
        {
            if (m_threadService == nullptr)
            {
                throw ExceptionWithCallStack(SPXERR_UNINITIALIZED);
            }

            TRet ret;
            std::packaged_task<void()> task([func, &ret]() { ret = func(); });
            m_threadService->ExecuteSync(move(task), m_affinity);
            return ret;
        }

        template<typename TRet>
        inline TRet RunSynchronously(std::function<TRet()> func) const
        {
            if (m_threadService == nullptr)
            {
                throw ExceptionWithCallStack(SPXERR_UNINITIALIZED);
            }

            TRet ret;
            std::packaged_task<void()> task([func, &ret]() { ret = func(); });
            m_threadService->ExecuteSync(move(task), m_affinity);
            return ret;
        }

        inline void RunSynchronously(std::function<void()> func)
        {
            if (m_threadService == nullptr)
            {
                throw ExceptionWithCallStack(SPXERR_UNINITIALIZED);
            }

            std::packaged_task<void()> task(func);
            m_threadService->ExecuteSync(move(task), m_affinity);
        }

        inline void RunSynchronously(std::function<void()> func) const
        {
            if (m_threadService == nullptr)
            {
                throw ExceptionWithCallStack(SPXERR_UNINITIALIZED);
            }

            std::packaged_task<void()> task(func);
            m_threadService->ExecuteSync(move(task), m_affinity);
        }

    protected:
        std::shared_ptr<ISpxThreadService> m_threadService;

    private:
        const ISpxThreadService::Affinity m_affinity;
        std::shared_ptr<ISpxRecognizerSite> m_keepSessionAlive;
    };

    class NamedPropertiesHelper
    {
    public:
        static inline const string GetString(shared_ptr<ISpxNamedProperties> properties, PropertyId id, const char * defaultValue = nullptr)
        {
            string value = __GetString(properties, id);
            return value == UNSET_DEFAULT_VALUE
                ? string(defaultValue == nullptr ? "" : defaultValue)
                : value;
        }

        static inline const string GetString(shared_ptr<ISpxNamedProperties> properties, initializer_list<std::string> ids, const char * defaultValue = nullptr)
        {
            for (const std::string& id : ids)
            {
                string value = properties->GetStringValue(id.c_str(), UNSET_DEFAULT_VALUE);
                if (value != UNSET_DEFAULT_VALUE)
                {
                    return value;
                }
            }

            return defaultValue == nullptr ? "" : defaultValue;
        }

        static inline int32_t GetInt(shared_ptr<ISpxNamedProperties> properties, PropertyId id, int32_t defaultValue = -1)
        {
            string value = __GetString(properties, id);
            if (value == UNSET_DEFAULT_VALUE || value.empty())
            {
                return defaultValue;
            }

            return static_cast<int32_t>(std::stol(value));
        }

        static inline bool GetBool(shared_ptr<ISpxNamedProperties> properties, PropertyId id, int32_t defaultValue = -1)
        {
            string value = __GetString(properties, id);
            if (value == UNSET_DEFAULT_VALUE || value.empty())
            {
                return defaultValue;
            }

            return PAL::ToBool(value);
        }

        static inline bool GetBool(shared_ptr<ISpxNamedProperties> properties, const char * propertyName, bool defaultValue = false)
        {
            string value = properties->GetStringValue(propertyName, UNSET_DEFAULT_VALUE);
            if (value == UNSET_DEFAULT_VALUE || value.empty())
            {
                return defaultValue;
            }

            return PAL::ToBool(value);
        }

        static inline void SetString(shared_ptr<ISpxNamedProperties> properties, PropertyId id, const std::string& value)
        {
            SetString(properties, id, value.c_str());
        }

        static inline void SetString(shared_ptr<ISpxNamedProperties> properties, PropertyId id, const char * value)
        {
            const char * propertyName = GetPropertyName(id);
            if (propertyName == nullptr)
            {
                SPX_THROW_HR(SPXERR_RUNTIME_ERROR);
            }

            properties->SetStringValue(propertyName, value);
        }

    private:
        static inline const string __GetString(shared_ptr<ISpxNamedProperties> properties, PropertyId id)
        {
            const char * propertyName = GetPropertyName(id);
            if (propertyName == nullptr)
            {
                SPX_THROW_HR(SPXERR_RUNTIME_ERROR);
            }

            return properties->GetStringValue(propertyName, UNSET_DEFAULT_VALUE);
        }

    private:
        static constexpr auto UNSET_DEFAULT_VALUE = "!!<<NOT_SET>>!!";
    };

}}}}} // Microsoft::CognitiveServices::Speech::Impl::Conversation
