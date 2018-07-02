//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// com_ini_and_uninit.h: implement a class for initialization and un-initialization of COM.
//

#pragma once

#include "stdafx.h"
#include <windows.h>
#include <mmsystem.h>

/*
 The worker thread in COmInitANdUnInit class worker is being started in microphonePump ctor, sitting idle there. The microphonePump dtor signals the thread to exit. The thread has a local variable that does COM init in its ctor and COM un-init in its dctor.
All COM calls are within microphonePump class. The worker thread lives longer than all the COM calls in our code.

More details, see https://blogs.msdn.microsoft.com/oldnewthing/20130419-00/?p=4613
*/
namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class ComInitAndUnInit
{
public:
    ComInitAndUnInit();
    virtual ~ComInitAndUnInit();

private:
    void worker_thread_proc();

private:
    struct ComInitAndDestroy
    {

        ComInitAndDestroy()
        {
            if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED)))
            {
                SPX_THROW_ON_FAIL(SPXERR_MIC_NOT_AVAILABLE);
            }
        }

        ~ComInitAndDestroy()
        {
            CoUninitialize();
        }
    };

private:
    std::thread    m_workerThread;
    std::condition_variable m_cvCom;
    bool           m_comIsDone = false;
    bool           m_comStarted = false;
    std::mutex     m_comMutex;
};

}}}}