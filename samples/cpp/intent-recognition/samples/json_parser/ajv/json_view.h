//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//
#ifndef __AJV_JSON_VIEW_H
#define __AJV_JSON_VIEW_H

#include <stdlib.h>
#include <vector>

#include <ajv/common.h>
#include <ajv/json_string.h>

namespace ajv {

    class JsonView : protected JsonString::Helpers
    {
    public:

        JsonView();
        ~JsonView();

        JsonView(const JsonView&);
        JsonView& operator=(const JsonView&);

        JsonView(JsonView&&);
        JsonView& operator=(JsonView&&);

        int Parse(const char* json, size_t jsize);

        int Count(int item) const;
        int Find(int item, int index, const char* find, const char** name, size_t* nsize) const;
        int Next(int item) const;
        
        int GetKind(int item) const;
        int GetKind(int item, const char** json, size_t* jsize, const char** value, size_t* vsize, bool* boolean, int* integer, double* number) const;
        
        bool GetJson(int item, const char** json, size_t* jsize) const;
        bool GetString(int item, const char** value, size_t* vsize) const;
        bool GetBool(int item, bool* boolean) const;

        bool GetInt64(int item, int64_t* value) const;
        bool GetUint64(int item, uint64_t* value) const;
        bool GetDouble(int item, double* value) const;

        int Name(int item) const;
        int Parent(int item) const;

    protected:

        static constexpr const char* m_unspecifiedValue = "?";
        static constexpr size_t m_cchUnspecifiedValue = 1;

        int InitItemUnspecified();

        static const char* SkipWhiteSpace(const char* psz, const char* zend);
        static const char* SkipCharsInRange(const char* psz, const char* zend, char ch1, char ch2);
        const char* ParseElement(const char* psz, const char* zend);
        const char* ParseValue(const char* psz, const char* zend);
        const char* ParseArray(const char* psz, const char* zend);
        const char* ParseObject(const char* psz, const char* zend);
        const char* ParseMember(const char* psz, const char* zend);
        const char* ParseString(const char* psz, const char* zend);
        const char* ParseBoolean(const char* psz, const char* zend);
        const char* ParseNumber(const char* psz, const char* zend);
        const char* ParseNull(const char* psz, const char* zend);
        const char* ParseError(const char* psz, const char* zend);

        struct _item {
            const char* start;
            const char* end;
            union {
                int first;
                int firstInArray;
                int firstInObject;
                int namesValue;
            };
            union {
                int next;
                int nextInArray;
                int nextInObject;
            };
        };

        int PeekItem() { return m_itemCount; }
        int ReserveItem();
        int InitItem(const char* psz);

        const char* EndItem(int item, const char* psz);
        static const char* CopyItem(const _item& item, char* dest, size_t cch);

        static constexpr size_t m_itemCountResize{ 256 };
        std::vector<_item> m_items{ m_itemCountResize };
        int m_itemCount{ 0 };

        static constexpr int m_itemError{ -1 };
        static constexpr int m_itemEnd{ 0 };

        static constexpr int m_maxOpenItems{ 1024 };
        int m_openItems{ 0 };
    };

    AJV_FN_NO_INLINE JsonView::JsonView()
    {
        EndItem(InitItem(nullptr), nullptr);
    }

    AJV_FN_NO_INLINE JsonView::~JsonView()
    {
    }

    AJV_FN_NO_INLINE JsonView::JsonView(const JsonView& other) :
        m_items(other.m_items),
        m_itemCount(other.m_itemCount)
    {
    }
    
    AJV_FN_NO_INLINE JsonView& JsonView::operator=(const JsonView& other)
    {
        m_items = other.m_items;
        m_itemCount = other.m_itemCount;
        return *this;
    }

    AJV_FN_NO_INLINE JsonView::JsonView(JsonView&& other) :
        m_items(std::move(other.m_items)),
        m_itemCount(other.m_itemCount)
    {
        other.m_itemCount = 0;
    }

    AJV_FN_NO_INLINE JsonView& JsonView::operator=(JsonView&& other)
    {
        m_items = std::move(other.m_items);
        m_itemCount = other.m_itemCount;
        other.m_itemCount = 0;
        return *this;
    }

    AJV_FN_NO_INLINE_(int) JsonView::Parse(const char* json, size_t jsize)
    {
        if (IsNullOrWhiteSpace(json, jsize)) return 0;

        auto item = PeekItem();
        auto zend = json + jsize;
        auto psz = ParseElement(json, zend);

        return psz == zend ? item : -1;
    }

    AJV_FN_NO_INLINE_(int) JsonView::Count(int item) const
    {
        if (item <= 0 || item >= m_itemCount) return 0;
        if (m_items[item].start == nullptr) return 0;

        auto parent = item;
        auto kind = m_items[parent].start[0];

        if (kind == '[' || kind == '{')
        {
            int count = 0;
            auto ivalue = m_items[parent].firstInArray;
            while (ivalue > 0)
            {
                ivalue = m_items[ivalue].nextInArray;
                count++;
            }
            return count;
        }

        return 0;
    }

    AJV_FN_NO_INLINE_(int) JsonView::Find(int item, int index, const char* find, const char** name, size_t* nsize) const
    {
        if (item <= 0 || item >= m_itemCount) return -1;
        if (m_items[item].start == nullptr) return -1;

        auto parent = item;
        auto kind = m_items[parent].start[0];

        if (kind == '[' && find == nullptr)
        {
            auto ivalue = m_items[parent].firstInArray;
            while (index > 0 && ivalue > 0)
            {
                ivalue = m_items[ivalue].nextInArray;
                index--;
            }
            return ivalue > 0 ? ivalue : 0;
        }
        else if (kind == '{')
        {
            auto iname = m_items[parent].firstInObject;
            if (find != nullptr)
            {
                auto len = strlen(find);
                while (iname > 0)
                {
                    auto check = m_items[iname].start + 1;
                    auto match = strncmp(check, find, len) == 0 && IsStartString(check[len]);
                    if (match) break;

                    iname = m_items[iname].nextInObject;
                }
            }
            else
            {
                while (iname > 0 && index > 0)
                {
                    iname = m_items[iname].nextInObject;
                    index--;
                }
            }

            if (iname > 0)
            {
                if (name != nullptr) *name = m_items[iname].start + 1;
                if (nsize != nullptr) *nsize = m_items[iname].end - m_items[iname].start - 1;

                auto ivalue = m_items[iname].namesValue;
                return ivalue > 0 ? ivalue : 0;
            }

            return 0;
        }

        return -1;
    }

    AJV_FN_NO_INLINE_(int) JsonView::Next(int item) const
    {
        if (item < 0 || item >= m_itemCount) return -1;
        if (item == 0) return 0;
        return m_items[item].next;
    }

    AJV_FN_NO_INLINE_(int) JsonView::GetKind(int item) const
    {
        if (item < 0 || item >= m_itemCount) return -1;
        if (item == 0) return 0;

        auto& get = m_items[item];
        if (get.start == nullptr) return -1;

        auto kind = get.start[0];
        if (IsStartBoolean(kind)) return 'b';
        if (IsStartNumber(kind)) return '1';

        return kind;
    }

    AJV_FN_NO_INLINE_(int) JsonView::GetKind(int item, const char** json, size_t* jsize, const char** value, size_t* vsize, bool* boolean, int* integer, double* number) const
    {
        if (json != nullptr) *json = nullptr;
        if (jsize != nullptr) *jsize = 0;
        if (value != nullptr) *value = nullptr;
        if (vsize != nullptr) *vsize = 0;
        if (boolean != nullptr) *boolean = false;
        if (integer != nullptr) *integer = 0;
        if (number != nullptr) *number = 0;

        if (item < 0 || item >= m_itemCount) return -1;
        if (item == 0) return 0;

        auto get = m_items[item];
        if (get.start == nullptr) return -1;

        if (json != nullptr) *json = get.start;
        if (jsize != nullptr) *jsize = get.end - get.start + 1;

        auto kind = get.start[0];
        if (IsStartString(kind))
        {
            if (value != nullptr) *value = get.start + 1;
            if (vsize != nullptr) *vsize = get.end - get.start - 1;
        }
        else if (IsStartBoolean(kind))
        {
            kind = 'b';
            if (boolean != nullptr)
            {
                *boolean = get.start[0] == 't';
            }
        }
        else if (IsStartNumber(kind))
        {
            kind = '1';
            if (integer != nullptr)
            {
                char buffer[50];
                CopyItem(get, buffer, sizeof(buffer));
                *integer = std::atoi(buffer);
            }
            if (number != nullptr)
            {
                char buffer[50];
                CopyItem(get, buffer, sizeof(buffer));
                *number = std::atof(buffer);
            }
        }

        return kind;
    }

    AJV_FN_NO_INLINE_(bool) JsonView::GetJson(int item, const char** json, size_t* jsize) const
    {
        if (json != nullptr) *json = nullptr;
        if (jsize != nullptr) *jsize = 0;
        if (item <= 0 || item >= m_itemCount) return false;

        auto& get = m_items[item];
        if (get.start != nullptr && get.start[0] == '?') return false;

        if (json != nullptr) *json = get.start;
        if (jsize != nullptr) *jsize = get.end - get.start + 1;

        return true;
    }

    AJV_FN_NO_INLINE_(bool) JsonView::GetString(int item, const char** value, size_t* vsize) const
    {
        if (value != nullptr) *value = nullptr;
        if (vsize != nullptr) *vsize = 0;
        if (item <= 0 || item >= m_itemCount) return false;

        auto& get = m_items[item];

        auto kind = get.start[0];
        if (IsStartString(kind))
        {
            if (value != nullptr) *value = get.start + 1;
            if (vsize != nullptr) *vsize = get.end - get.start - 1;
            return true;
        }

        return false;
    }

    AJV_FN_NO_INLINE_(bool) JsonView::GetBool(int item, bool* value) const
    {
        if (value != nullptr) *value = false;
        if (item <= 0 || item >= m_itemCount) return false;

        auto& get = m_items[item];
        auto kind = get.start[0];

        if (IsStartBoolean(kind))
        {
            if (value != nullptr) *value = get.start[0] == 't';
            return true;
        }

        return false;
    }

    AJV_FN_NO_INLINE_(bool) JsonView::GetInt64(int item, int64_t* value) const
    {
        if (value != nullptr) *value = 0;
        if (item <= 0 || item >= m_itemCount) return false;

        auto& get = m_items[item];
        auto kind = get.start[0];

        if (IsStartNumber(kind))
        {
            if (value != nullptr)
            {
                char buffer[50]; 
                CopyItem(get, buffer, sizeof(buffer));

                char *bufferEnd;
                *value = std::strtoll(buffer, &bufferEnd, 10);

                auto useAtof = *bufferEnd == '.' || *bufferEnd == 'e' || *bufferEnd == 'E';
                if (useAtof) *value = (int64_t)std::atof(buffer);
            }

            return true;
        }

        return false;
    }

    AJV_FN_NO_INLINE_(bool) JsonView::GetUint64(int item, uint64_t* value) const
    {
        if (value != nullptr) *value = 0;
        if (item <= 0 || item >= m_itemCount) return false;

        auto& get = m_items[item];
        auto kind = get.start[0];

        if (IsStartNumber(kind))
        {
            if (value != nullptr)
            {
                char buffer[50];
                CopyItem(get, buffer, sizeof(buffer));

                char* bufferEnd;
                *value = std::strtoull(buffer, &bufferEnd, 10);

                auto useAtof = buffer[0] == '-' || *bufferEnd == '.' || *bufferEnd == 'e' || *bufferEnd == 'E';
                if (useAtof)
                {
                    auto num = std::atof(buffer);
                    *value = num < 0 ? 0 : (uint64_t)num;
                }
            }

            return true;
        }

        return false;
    }

    AJV_FN_NO_INLINE_(bool) JsonView::GetDouble(int item, double* number) const
    {
        if (number != nullptr) *number = 0;
        if (item <= 0 || item >= m_itemCount) return false;

        auto& get = m_items[item];
        auto kind = get.start[0];

        if (IsStartNumber(kind))
        {
            if (number != nullptr)
            {
                char buffer[50];
                CopyItem(get, buffer, sizeof(buffer));
                *number = std::atof(buffer);
            }
            return true;
        }

        return false;
    }

    AJV_FN_NO_INLINE_(int) JsonView::Name(int item) const
    {
        if (item < 0) return -1;
        if (item == 0 || item >= m_itemCount) return 0;

        int stage = 0;
        int ivalue = item;
        int iname = -1;
        int inameCheckFirst = -1;
        int icheck;
        for (icheck = ivalue - 1; icheck > m_itemEnd; icheck--)
        {
            auto& check = m_items[icheck];

            // stage 0 tries to find the value's name
            if (stage == 0 && check.namesValue == ivalue && check.start != nullptr && IsStartString(*check.start))
            {
                // we found the name that points to the value, and it's a string, move to the next stage
                stage = 1;
                iname = icheck;
                inameCheckFirst = iname;
                continue;
            }
            
            // stage 1 tries to ensure that the the name (or it's prevous sibling) is in an object
            if (stage == 1 && check.firstInObject == inameCheckFirst && check.start != nullptr && IsStartObject(*check.start))
            {
                // we found the object, whose first name is the iname (or an earlier sibling)
                break;
            }
            else if (stage == 1 && check.nextInObject == inameCheckFirst)
            {
                inameCheckFirst = icheck;
            }
        }
        return icheck > 0 ? iname : 0;
    }

    AJV_FN_NO_INLINE_(int) JsonView::Parent(int item) const
    {
        if (item < 0) return -1;
        if (item == 0 || item >= m_itemCount) return 0;

        int stage = 0;
        int ineed = item;
        int icheck;
        for (icheck = ineed - 1; icheck > m_itemEnd; icheck--)
        {
            auto& check = m_items[icheck];

            // in stage 0, if we find string who's namesValue == ineed, item is member value
            if (stage == 0 && check.namesValue == ineed && check.start != nullptr && IsStartString(*check.start))
            {
                // for member values, update what we're looking for to be == the name item
                stage = 1;
                ineed = icheck;
                continue;
            }

           // in stage 0 or 1, if we find item who's first == ineed, we found the parent
            if (check.first == ineed && check.start != nullptr && (IsStartObject(*check.start) || IsStartArray(*check.start)))
            {
                break;
            }
 
            // in stage 0 or 1, if we find item who's next == ineed, we found an earlier sibling
            if (check.next == ineed)
            {
                // regardless of what it is, update the item we need (looking for container where first == item, or item's oldest sibling)
                ineed = icheck;
                continue;
            }
        }
        return icheck > 0 ? icheck : 0;
    }

    AJV_FN_NO_INLINE_(int) JsonView::InitItemUnspecified()
    {
        auto item = InitItem(m_unspecifiedValue);
        EndItem(item, m_unspecifiedValue + m_cchUnspecifiedValue - 1);
        return item;
    }

    AJV_FN_NO_INLINE_(const char*) JsonView::SkipWhiteSpace(const char* psz, const char* zend)
    {
        while (psz < zend && IsWhiteSpace(*psz))
        {
            psz++;
        }
        return psz;
    }

    AJV_FN_NO_INLINE_(const char*) JsonView::SkipCharsInRange(const char* psz, const char* zend, char ch1, char ch2)
    {
        while (psz < zend && *psz >= ch1 && *psz <= ch2)
        {
            psz++;
        }
        return psz;
    }

    AJV_FN_NO_INLINE_(const char*) JsonView::ParseElement(const char* psz, const char* zend)
    {
        psz = SkipWhiteSpace(psz, zend);
        psz = ParseValue(psz, zend);
        psz = SkipWhiteSpace(psz, zend);
        return psz;
    }

    AJV_FN_NO_INLINE_(const char*) JsonView::ParseValue(const char* psz, const char* zend)
    {
        if (psz < zend)
        {
            auto ch = *psz;
            if (IsStartArray(ch)) return ParseArray(psz, zend);
            if (IsStartObject(ch)) return ParseObject(psz, zend);
            if (IsStartString(ch)) return ParseString(psz, zend);
            if (IsStartBoolean(ch)) return ParseBoolean(psz, zend);
            if (IsStartNumber(ch)) return ParseNumber(psz, zend);
            if (IsStartNull(ch)) return ParseNull(psz, zend);
        }

        return ParseError(psz, zend);
    }

    AJV_FN_NO_INLINE_(const char*) JsonView::ParseArray(const char* psz, const char* zend)
    {
        auto parent = InitItem(psz++);
        if (parent <= m_itemEnd) return ParseError(psz, zend);

        psz = SkipWhiteSpace(psz, zend);
        if (psz < zend && !IsEndArray(*psz))
        {
            auto first = PeekItem();
            psz = ParseElement(psz, zend);
            if (psz > zend) return psz;

            m_items[parent].firstInArray = first;

            auto previous = first;
            while (psz < zend && IsContinueArray(*psz))
            {
                auto item = PeekItem();
                psz = ParseElement(psz + 1, zend);
                if (psz > zend) return psz;

                m_items[previous].nextInArray = item;
                previous = item;
            }
        }

        return psz < zend && IsEndArray(*psz)
            ? EndItem(parent, psz)
            : ParseError(psz, zend);
    }

    AJV_FN_NO_INLINE_(const char*) JsonView::ParseObject(const char* psz, const char* zend)
    {
        auto parent = InitItem(psz++);
        if (parent <= m_itemEnd) return ParseError(psz, zend);

        psz = SkipWhiteSpace(psz, zend);
        if (psz < zend && !IsEndObject(*psz))
        {
            auto first = PeekItem();
            psz = ParseMember(psz, zend);
            if (psz > zend) return psz;

            m_items[parent].firstInObject = first;

            auto previous = first;
            while (psz < zend && IsContinueObject(*psz))
            {
                psz = SkipWhiteSpace(psz + 1, zend);
                if (psz < zend)
                {
                    auto item = PeekItem();
                    psz = ParseMember(psz, zend);
                    if (psz > zend) return psz;

                    m_items[previous].nextInObject = item;
                    m_items[previous + 1].nextInObject = item + 1;
                    previous = item;
                }
            }
        }

        return psz < zend && IsEndObject(*psz)
            ? EndItem(parent, psz)
            : ParseError(psz, zend);
    }

    AJV_FN_NO_INLINE_(const char*) JsonView::ParseMember(const char* psz, const char* zend)
    {
        if (IsStartString(*psz))
        {
            auto name = PeekItem();
            psz = ParseString(psz, zend);
            if (psz > zend) return psz;

            psz = SkipWhiteSpace(psz, zend);
            if (psz < zend && *psz == ':')
            {
                auto value = PeekItem();
                psz = ParseElement(psz + 1, zend);
                if (psz > zend) return psz;

                m_items[name].namesValue = value;

                return psz;
            }
        }

        return ParseError(psz, zend);
    }

    AJV_FN_NO_INLINE_(const char*) JsonView::ParseString(const char* psz, const char* zend)
    {
        auto item = InitItem(psz++);
        if (item <= m_itemEnd) return ParseError(psz, zend);

        while (psz < zend && *psz != '\"')
        {
            auto ch = *psz;
            if (ch == '\\')
            {
                if (psz + 5 < zend && psz[1] == 'u' && IsHex4Digits(psz + 2))
                {
                    psz += 6;
                    continue;
                }
                else if (psz + 1 < zend && IsCharEscape2Char2(psz[1]))
                {
                    psz += 2;
                    continue;
                }
                break;
            }
            else if (IsCharEscape2Required(ch) || Utf8::IsInvalid(ch))
            {
                return ParseError(psz, zend);
            }

            auto cb = Utf8::Is1Start(ch) ? 1 : Utf8::Is2Start(ch) ? 2 : Utf8::Is3Start(ch) ? 3 : Utf8::Is4Start(ch) ? 4 : 0;
            if (cb == 0) return ParseError(psz, zend);
            psz += cb;
        }

        return psz < zend && *psz == '\"'
            ? EndItem(item, psz)
            : ParseError(psz, zend);
    }

    AJV_FN_NO_INLINE_(const char*) JsonView::ParseBoolean(const char* psz, const char* zend)
    {
        auto item = InitItem(psz);

        auto isTrue = psz + 3 < zend && psz[0] == 't' && psz[1] == 'r' && psz[2] == 'u' && psz[3] == 'e';
        if (isTrue) return EndItem(item, psz + 3);

        auto isFalse = psz + 4 < zend && psz[0] == 'f' && psz[1] == 'a' && psz[2] == 'l' && psz[3] == 's' && psz[4] == 'e';
        if (isFalse) return EndItem(item, psz + 4);

        return ParseError(psz, zend);
    }

    AJV_FN_NO_INLINE_(const char*) JsonView::ParseNumber(const char* psz, const char* zend)
    {
        auto item = InitItem(psz);
        if (item <= m_itemEnd) return ParseError(psz, zend);

        if (*psz == '-') psz++;

        if (psz >= zend || !IsDigit(*psz)) return ParseError(psz, zend);
        psz = *psz == '0' ? psz + 1 : SkipCharsInRange(psz + 1, zend, '0', '9');

        if (psz < zend && *psz == '.')
        {
            psz++;
            if (psz >= zend || !IsDigit(*psz)) return ParseError(psz, zend);
            psz = SkipCharsInRange(psz + 1, zend, '0', '9');
        }

        if (psz < zend && (*psz == 'e' || *psz == 'E'))
        {
            psz++;
            if (*psz == '-' || *psz == '+') psz++;
            if (psz >= zend || !IsDigit(*psz)) return ParseError(psz, zend);
            psz = SkipCharsInRange(psz + 1, zend, '0', '9');
        }

        return EndItem(item, psz - 1);
    }

    AJV_FN_NO_INLINE_(const char*) JsonView::ParseNull(const char* psz, const char* zend)
    {
        auto item = InitItem(psz);

        auto isNull = psz + 3 < zend && psz[0] == 'n' && psz[1] == 'u' && psz[2] == 'l' && psz[3] == 'l';
        if (isNull) return EndItem(item, psz + 3);

        return ParseError(psz, zend);
    }

    AJV_FN_NO_INLINE_(const char*) JsonView::ParseError(const char* /* psz */, const char* zend)
    {
        return zend + 1;
    }

    AJV_FN_NO_INLINE_(int) JsonView::ReserveItem()
    {
         auto i = m_itemCount++;
        if (m_items.size() < (size_t)m_itemCount)
        {
            m_items.resize(m_items.size() + m_itemCountResize);
        }
        return i;
    }

    AJV_FN_NO_INLINE_(int) JsonView::InitItem(const char* psz)
    {
        if (m_openItems >= m_maxOpenItems)
        {
            return -1;
        }

        auto i = ReserveItem();
        auto& item = m_items[i];

        m_openItems++;
        item.start = psz;
        item.end = psz;
        item.first = 0;
        item.next = 0;

        return i;
    }

    AJV_FN_NO_INLINE_(const char*) JsonView::EndItem(int item, const char* psz)
    {
        m_openItems--;
        m_items[item].end = psz;
        return ++psz;
    }

    AJV_FN_NO_INLINE_(const char*) JsonView::CopyItem(const _item& item, char* dest, size_t cch)
    {
        auto needed = (size_t)(item.end - item.start + 1);
        if (cch < needed) return nullptr;

        auto rv = dest;
        for (auto ptr = item.start; cch > 1 && ptr <= item.end; dest++, ptr++, cch--) *dest = *ptr;
        *dest = '\0'; // cch >= 1 (see above)

        return rv;
    }
}

#endif // __AJV_JSON_VIEW_H
