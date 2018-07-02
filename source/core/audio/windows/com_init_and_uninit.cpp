//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// com_ini_and_uninit.cpp: implement a class for initialization and un-initialization of COM.
//

#include "stdafx.h"
#include "com_init_and_uninit.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

ComInitAndUnInit::ComInitAndUnInit()
{
    SPX_DBG_TRACE_SCOPE("ComInitAndUnInit ctor enter", "ComInitAndUnInit ctor done");
    if (!m_workerThread.joinable())
    {
        m_workerThread = std::thread([this]()
        {
            worker_thread_proc();
        });
    }
    {
        std::unique_lock<std::mutex> lock(m_comMutex);
        m_cvCom.wait(lock, [&started = m_comStarted] { return started; });
    }
}

ComInitAndUnInit::~ComInitAndUnInit()
{
    {
        std::unique_lock<std::mutex> lock(m_comMutex);
        m_comIsDone = true;
    }
    m_cvCom.notify_all();
    m_workerThread.join();
}

void ComInitAndUnInit::worker_thread_proc()
{
    SPX_DBG_TRACE_SCOPE("com worker_thread_proc() enter", "com worker_thread_proc done");

    // the ctor of this object initializes COM and its dtor destroys COM
    ComInitAndDestroy initAndDestroyCom;

    {
        std::unique_lock<std::mutex> lock(m_comMutex);
        m_comStarted = true;
    }
    SPX_DBG_TRACE_VERBOSE("com successfully Initialized");
    m_cvCom.notify_all();

    while (!m_comIsDone)
    {
        std::unique_lock<std::mutex> lock(m_comMutex);
        m_cvCom.wait(lock, [&done = m_comIsDone] { return done; });
    }
}

}}}}