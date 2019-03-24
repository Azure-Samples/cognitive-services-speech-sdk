#include "stdafx.h"
#include "synthesis_event_args.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


CSpxSynthesisEventArgs::CSpxSynthesisEventArgs()
{
}

std::shared_ptr<ISpxSynthesisResult> CSpxSynthesisEventArgs::GetResult()
{
    SPX_IFTRUE_THROW_HR(m_result == nullptr, SPXERR_UNINITIALIZED);
    return m_result;
}

void CSpxSynthesisEventArgs::Init(std::shared_ptr<ISpxSynthesisResult> result)
{
    SPX_IFTRUE_THROW_HR(m_result != nullptr, SPXERR_ALREADY_INITIALIZED);
    m_result = result;
}


} } } } // Microsoft::CognitiveServices::Speech::Impl
