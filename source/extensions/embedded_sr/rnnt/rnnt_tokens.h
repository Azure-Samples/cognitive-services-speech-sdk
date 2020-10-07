//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// rnnt_tokens.h: Implementation declarations for CRnntTokens C++ class
//

#pragma once

#include <string>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <vector>
#include <cstdint>
#include <cstring>
#include <memory>
#include <locale>

#include "rnnt_client.h"
#include "string_utils.h"
#include "spxdebug.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace RNNT {

class RnntEntry
{
public:

    RnntEntry() : m_length(0) {}

    RnntEntry(size_t length, const uint32_t* tokens, std::vector<float>&& scores) :
        m_length(length),
        m_value(length > 0 ? new uint32_t[length] : nullptr),
        m_scores(scores)
    {
        if (length > 0)
        {
            memcpy(m_value.get(), tokens, length * sizeof(tokens[0]));
        }
    }

    size_t Length() const { return m_length; }
    const uint32_t* Value() const { return m_value.get(); }

    bool operator==(const RnntEntry& other) const
    {
        if (m_length != other.Length())
        {
            return false;
        }
        return memcmp(m_value.get(), other.m_value.get(), m_length) == 0;
    }

    bool operator!=(const RnntEntry& other) const
    {
        return !(*this == other);
    }

private:

    size_t m_length;
    std::unique_ptr<uint32_t[]> m_value;
    std::vector<float> m_scores;
};

using RnntEntryPtr = std::unique_ptr<RnntEntry>;

struct TokenDefs
{
    uint32_t m_blank = (uint32_t)-1;
    uint32_t m_resetWithBlankToken = (uint32_t)-1;
    uint32_t m_replaceWithBlankToken = (uint32_t)-1;
    uint32_t m_noopToken = (uint32_t)-1;
    uint32_t m_nodupToken = (uint32_t)-1;
    uint32_t m_blockToken = (uint32_t)-1;
    uint32_t m_lastToken = (uint32_t)-1;
    uint32_t m_eosToken = (uint32_t)-1;
};

class RnntTokens
{
public:

    RnntTokens(const std::wstring& tokenFile)
    {
        LoadTokens(tokenFile);
    }

    const TokenDefs& GetTokenDefs() { return m_tokenDefs; }

    std::wstring TokenToString(const RnntEntryPtr& entry, bool displayText)
    {
        size_t total = 0;
        const uint32_t* start = entry->Value();
        size_t count = entry->Length();
        std::for_each(start, start + count, [this, &total](const uint32_t& token)
            {
                SPX_THROW_HR_IF(SPXERR_OUT_OF_RANGE, token >= m_tokens.size());
                total += m_tokens[token].Length;
            });

        std::wstring result;
        if (total > 0)
        {
            result.reserve(total);
            if (displayText)
            {
                bool inSegment = false;
                std::for_each(start, start + count, [this, &result, &inSegment](const uint32_t& token)
                    {
                        const TokenEntry& entry = m_tokens[token];
                        if (entry.Length == 0 || entry.LexicalText[0] == L'<')
                        {
                            return;
                        }
                        if (entry.LexicalText[0] == L'_')
                        {
                            if (inSegment)
                            {
                                result += L' ';
                            }
                            inSegment = true;
                            result.append(entry.LexicalText, 1U, std::wstring::npos);
                            return;
                        }
                        result += entry.LexicalText;
                    });
            }
            else
            {
                std::for_each(start, start + count, [this, &result](const uint32_t& token)
                    {

                        result += m_tokens[token].LexicalText;
                    });
            }
        }
        return result;
    }

private:

    struct TokenEntry
    {
        TokenEntry(const std::wstring& s) :
            Length(s.length()),
            LexicalText(s)
        {}

        size_t Length = 0;
        std::wstring LexicalText;
    };

    void LoadTokens(const std::wstring& tokenFile)
    {
        std::wifstream tfs(PAL::ToString(tokenFile));
        SPX_THROW_HR_IF(SPXERR_FILE_OPEN_FAILED, !tfs.is_open());

        tfs.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));
        std::istream_iterator<std::wstring, wchar_t> begin(tfs), end;
        std::for_each(begin, end, [this](const std::wstring& s)
            {
                if (s == L"<BLANK>" || s == L"<blank>")
                {
                    SPX_THROW_HR_IF(SPXERR_UNSUPPORTED_FORMAT, m_tokenDefs.m_blank != (uint32_t)-1);
                    m_tokenDefs.m_blank = (uint32_t)m_tokens.size();
                }

                if (s == L"<eosReset>" ||
                    s == L"<eosResetLast>" ||
                    s == L"<eosResetNodup>" ||
                    s == L"<eosResetNodupLast>")
                {
                    SPX_THROW_HR_IF(SPXERR_UNSUPPORTED_FORMAT, m_tokenDefs.m_resetWithBlankToken != (uint32_t)-1);
                    m_tokenDefs.m_resetWithBlankToken = (uint32_t)m_tokens.size();
                }

                if (s == L"<eosReplace>" ||
                    s == L"<eosReplaceLast>" ||
                    s == L"<eosReplaceNodup>" ||
                    s == L"<eosReplaceNodupLast>")
                {
                    SPX_THROW_HR_IF(SPXERR_UNSUPPORTED_FORMAT, m_tokenDefs.m_replaceWithBlankToken != (uint32_t)-1);
                    m_tokenDefs.m_replaceWithBlankToken = (uint32_t)m_tokens.size();
                }

                if (s == L"<eosNoop>" ||
                    s == L"<eosNoopLast>" ||
                    s == L"<eosNoopNodup>" ||
                    s == L"<eosNoopNodupLast>" ||
                    s == L"<EOS>")
                {
                    SPX_THROW_HR_IF(SPXERR_UNSUPPORTED_FORMAT, m_tokenDefs.m_noopToken != (uint32_t)-1);
                    m_tokenDefs.m_noopToken = (uint32_t)m_tokens.size();
                }

                if (s == L"<eosResetNodup>" ||
                    s == L"<eosResetNodupLast>" ||
                    s == L"<eosReplaceNodup>" ||
                    s == L"<eosReplaceNodupLast>" ||
                    s == L"<eosNoopNodup>" ||
                    s == L"<eosNoopNodupLast>" ||
                    s == L"<eosNodup>" ||
                    s == L"<eosNodupLast>" ||
                    s == L"<EOS>")
                {
                    SPX_THROW_HR_IF(SPXERR_UNSUPPORTED_FORMAT, m_tokenDefs.m_nodupToken != (uint32_t)-1);
                    m_tokenDefs.m_nodupToken = (uint32_t)m_tokens.size();
                }

                if (s == L"<eosBlock>")
                {
                    SPX_THROW_HR_IF(SPXERR_UNSUPPORTED_FORMAT, m_tokenDefs.m_blockToken != (uint32_t)-1);
                    m_tokenDefs.m_blockToken = (uint32_t)m_tokens.size();
                }

                if (s == L"<eosResetLast>" ||
                    s == L"<eosResetNodupLast>" ||
                    s == L"<eosReplaceLast>" ||
                    s == L"<eosReplaceNodupLast>" ||
                    s == L"<eosNoopLast>" ||
                    s == L"<eosNoopNodupLast>" ||
                    s == L"<eosLast>" ||
                    s == L"<eosNodupLast>")
                {
                    SPX_THROW_HR_IF(SPXERR_UNSUPPORTED_FORMAT, m_tokenDefs.m_lastToken != (uint32_t)-1);
                    m_tokenDefs.m_lastToken = (uint32_t)m_tokens.size();
                }

                if (s == L"<EOS>" || s == L"<eos>")
                {
                    SPX_THROW_HR_IF(SPXERR_UNSUPPORTED_FORMAT, m_tokenDefs.m_eosToken != (uint32_t)-1);
                    m_tokenDefs.m_eosToken = (uint32_t)m_tokens.size();
                }

                m_tokens.emplace_back(s);
            });

        SPX_THROW_HR_IF(SPXERR_UNSUPPORTED_FORMAT, m_tokenDefs.m_blank == (uint32_t)-1);
    }

private:

    std::vector<TokenEntry> m_tokens;
    TokenDefs m_tokenDefs;
};

}}}}
