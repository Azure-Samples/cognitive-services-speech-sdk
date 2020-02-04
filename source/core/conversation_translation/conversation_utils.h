//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once

#include <create_object_helpers.h>
#include <event.h>
#include <thread_service.h>
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
        virtual bool IsConnected() const = 0;
        virtual std::shared_ptr<ConversationArgs> GetConversationArgs() const = 0;
        virtual std::shared_ptr<CSpxConversationManager> GetConversationManager() const = 0;
        virtual std::shared_ptr<ConversationConnection> GetConversationConnection() const = 0;
    };

    struct FactoryUtils
    {
        /// <summary>
        /// Helper method to create new instances of ISpx interfaces. This bypasses the site
        /// factory methods and is intended for internal use only
        /// </summary>
        /// <typeparam name="C">The type that implements the interface I</typeparam>
        /// <typeparam name="Args...">The arguments to pass to the constructor for C</typeparam>
        template<typename C, typename ...Args>
        static shared_ptr<ISpxInterfaceBase> CreateBaseInstance(Args... args)
        {
            // WARNING: Because of the way the ISpx interface hierarchy is set up, directly creating a new
            //          instance of the concrete type won't properly set up the internals of enable_shared_from_this.
            //          This means that calling shared_from_this() will throw an exception. The reason is
            //          that the C++ memory code won't properly detect an unambiguous and accessible
            //          inheritance of shared_from_this. Instead, we instantiate in a roundabout way and
            //          create the shared_ptr with an instance of an interface pointer
            unique_ptr<C> rawPtr = make_unique<C>(args...);
            shared_ptr<ISpxInterfaceBase> ptr(dynamic_cast<ISpxInterfaceBase*>(rawPtr.get()));
            if (ptr != nullptr)
            {
                rawPtr.release();
            }

            auto init = dynamic_pointer_cast<ISpxObjectInit>(ptr);
            if (init != nullptr)
            {
                init->Init();
            }

            return ptr;
        }

        /// <summary>
        /// Helper method to create new instances of ISpx interfaces. This bypasses the site
        /// factory methods and is intended for internal use only
        /// </summary>
        /// <typeparam name="C">The type that implements the interface I</typeparam>
        /// <typeparam name="Args...">The arguments to pass to the constructor for C</typeparam>
        template<typename C, typename ...Args>
        static shared_ptr<C> CreateInstance(Args... args)
        {
            auto base = CreateBaseInstance<C, Args...>(args...);
            return dynamic_pointer_cast<C>(base);
;       }

        /// <summary>
        /// Helper method to create new instances of ISpx interfaces. This bypasses the site
        /// factory methods and is intended for internal use only
        /// </summary>
        /// <typeparam name="I">The ISpx interface type</typeparam>
        /// <typeparam name="C">The type that implements the interface I</typeparam>
        /// <typeparam name="Args...">The arguments to pass to the constructor for C</typeparam>
        template<typename I, typename C, typename ...Args>
        static shared_ptr<I> CreateInstance(Args... args)
        {
            auto base = CreateBaseInstance<C, Args...>(args...);
            return dynamic_pointer_cast<I>(base);
        }
    };

    class ThreadingHelpers : public ISpxObjectWithSiteInitImpl<ISpxRecognizerSite>
    {
    public:
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

            m_threadService = FactoryUtils::CreateInstance<ISpxThreadService, CSpxThreadService>();
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
            m_threadService->ExecuteAsync(move(task), ISpxThreadService::Affinity::Background, std::move(executed));
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
            m_threadService->ExecuteSync(move(task), ISpxThreadService::Affinity::Background);
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
            m_threadService->ExecuteSync(move(task), ISpxThreadService::Affinity::Background);
            return ret;
        }

        inline void RunSynchronously(std::function<void()> func)
        {
            if (m_threadService == nullptr)
            {
                throw ExceptionWithCallStack(SPXERR_UNINITIALIZED);
            }

            std::packaged_task<void()> task(func);
            m_threadService->ExecuteSync(move(task), ISpxThreadService::Affinity::Background);
        }

        inline void RunSynchronously(std::function<void()> func) const
        {
            if (m_threadService == nullptr)
            {
                throw ExceptionWithCallStack(SPXERR_UNINITIALIZED);
            }

            std::packaged_task<void()> task(func);
            m_threadService->ExecuteSync(move(task), ISpxThreadService::Affinity::Background);
        }

    protected:
        std::shared_ptr<ISpxThreadService> m_threadService;

    private:
        std::shared_ptr<ISpxRecognizerSite> m_keepSessionAlive;
    };

}}}}} // Microsoft::CognitiveServices::Speech::Impl::Conversation
