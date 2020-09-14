//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once
#include "spxcore_common.h"
#include "interface_helpers.h"
#include "service_helpers.h"
#include "object_with_site_init_impl.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class CSpxSession2 :
    public ISpxObjectWithSiteInitImpl<ISpxGenericSite>,
    public ISpxGenericSite,
    public ISpxServiceProvider,
    public ISpxSession2
{
public:

    CSpxSession2();
    virtual ~CSpxSession2();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxGenericSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxServiceProvider)
        SPX_INTERFACE_MAP_ENTRY(ISpxSession2)
    SPX_INTERFACE_MAP_END()

    SPX_SERVICE_MAP_BEGIN()
        SPX_SERVICE_MAP_ENTRY_SITE(GetSite())
    SPX_SERVICE_MAP_END()

    // --- ISpxObjectInit (overrides)
    void Init() final;
    void Term() final;

    // --- ISpxSession2 (overrides)
    const char* GetSessionId() const override;

    // void AddRecognizer(std::shared_ptr<ISpxRecognizer2> recognizer);
    // void RemoveRecognizer(ISpxRecognizer2* recognizer);

    // CSpxAsyncOp<std::shared_ptr<ISpxRecognitionResult2>> RecognizeOnceAsync(const char* kind = nullptr, const std::shared_ptr<ISpxNamedProperties>& properties = nullptr);
    // CSpxAsyncOp<void> StartContinuousAsync(const char* kind = nullptr, const std::shared_ptr<ISpxNamedProperties>& properties = nullptr);
    // CSpxAsyncOp<void> StopAsync(const char* stopModeOrKind = nullptr); // "once", "continuous", "{kind}", nullptr == "all"

    // CSpxAsyncOp<void> SendTechMessage(const char* tech, const char* message, const char* payload, const std::shared_ptr<ISpxNamedProperties>& properties);

private:

    const std::string m_sessionId;
};

} } } } // Microsoft::CognitiveServices::Speech::Impl
