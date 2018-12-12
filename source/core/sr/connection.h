#pragma once

#include <memory>
#include "ispxinterfaces.h"
#include "interface_helpers.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class CSpxConnection :
    public ISpxConnection,
    public ISpxObjectWithSiteInitImpl<ISpxConnectionSite>
{
public:

    CSpxConnection();
    virtual ~CSpxConnection();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxConnection)
    SPX_INTERFACE_MAP_END()

    // --- ISpxObjectWithSiteInit
    void Init() override;
    void Term() override;

    // --- ISpxConnection
    void Open() override;
    std::shared_ptr<ISpxRecognizer> GetRecognizer() override;

private:

    CSpxConnection(const CSpxConnection&) = delete;
    CSpxConnection(const CSpxConnection&&) = delete;

    CSpxConnection& operator=(const CSpxConnection&) = delete;

    std::shared_ptr<ISpxRecognizer> m_recognizer;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl
