#include "stdafx.h"
#include "phrase.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

void CSpxPhrase::InitPhrase(const wchar_t* phrase)
{
    SPX_IFTRUE_THROW_HR(!m_phrase.empty(), SPXERR_ALREADY_INITIALIZED);
    m_phrase = phrase;
}

std::wstring CSpxPhrase::GetPhrase() const
{
    return m_phrase;
}

} } } } // Microsoft::CognitiveServices::Speech::Impl
