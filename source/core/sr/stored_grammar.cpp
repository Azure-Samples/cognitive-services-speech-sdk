#include "stdafx.h"
#include "stored_grammar.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

void CSpxStoredGrammar::InitStoredGrammar(const wchar_t* id)
{
    SPX_IFTRUE_THROW_HR(!m_id.empty(), SPXERR_ALREADY_INITIALIZED);
    m_id = id;
}

std::wstring CSpxStoredGrammar::GetStorageId() const
{
    return m_id;
}

}}}} // Microsoft::CognitiveServices::Speech::Impl
