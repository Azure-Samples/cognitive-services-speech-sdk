//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//
#ifndef __AJV_JSON_STRING_H
#define __AJV_JSON_STRING_H

#pragma once

#include <memory>
#include <string>
#include <ajv/common.h>

namespace ajv {

    class JsonString
    {
    public:

        class Helpers
        {
        protected:

            static bool IsWhiteSpace(char ch) { return ch == ' ' || ch == '\r' || ch == '\n' || ch == '\t'; }

            static bool IsNullOrWhiteSpace(const char* ptr, size_t size)
            {
                auto check = ptr != nullptr;
                while (check && size > 0)
                {
                    check = IsWhiteSpace(*ptr);
                    size--;
                    ptr++;
                }
                return check || ptr == nullptr;
            }

            static bool IsNullOrWhiteSpace(const char* psz)
            {
                auto check = psz != nullptr;
                while (check && *psz != '\0')
                {
                    check = IsWhiteSpace(*psz);
                    psz++;
                }
                return check || psz == nullptr;
            }

            static bool IsDigit(char ch) { return ch >= '0' && ch <= '9'; }
            static bool IsHexDigit(char ch) { return IsDigit(ch) || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F'); }
            static bool IsHex4Digits(const char* psz) { return IsHexDigit(psz[0]) && IsHexDigit(psz[1]) && IsHexDigit(psz[2]) && IsHexDigit(psz[3]); }
            static char HexDigit(uint8_t num) { return num <= 9 ? '0' + num : 'a' + num - 10; }
            static uint8_t HexValue(char ch) { return ch >= '0' && ch <= '9' ? ch - '0' : ch >= 'a' && ch <= 'f' ? ch - 'a' + 10 : ch - 'A' + 10; }

            class Utf8
            {
            public: 

                static AJV_FN_NO_INLINE_(size_t) EncodedSize(const char* ptr, size_t cch)
                {
                    size_t size = 0;

                    auto end = ptr + cch;
                    while (ptr < end)
                    {
                        char ch = *ptr;
                        if (IsCharEscape2Required(ch))
                        {
                            size += 2;
                            ptr += 1;
                        }
                        else if (IsCharEscape6Required(ch))
                        {
                            auto cb = Utf8::Is1Start(ch) ? 1 : Utf8::Is2Start(ch) ? 2 : Utf8::Is3Start(ch) ? 3 : 4;
                            size += cb < 4 ? 6 : 12;
                            ptr += cb;
                        }
                        else
                        {
                            size += 1;
                            ptr += 1;
                        }
                    }

                    return size;
                }

                static AJV_FN_NO_INLINE_(size_t) Encode(const char* src, size_t srcSize, char* dest, size_t destSize)
                {
                    auto srcEnd = src + srcSize;
                    auto destEnd = dest + destSize;

                    size_t cch = 0;
                    while (src < srcEnd && dest < destEnd)
                    {
                        auto ch = *src;
                        if (IsCharEscape2Required(ch))
                        {
                            Escape2(src, dest, destEnd, cch);
                        }
                        else if (IsCharEscape6Required(ch))
                        {
                            Escape6(src, srcEnd, dest, destEnd, cch);
                        }
                        else
                        {
                            *dest++ = *src++;
                            cch++;
                        }
                    }

                    return cch;
                }

                static AJV_FN_NO_INLINE_(size_t) Decode(const char* src, size_t srcSize, char* dest, size_t destSize)
                {
                    auto srcEnd = src + srcSize;
                    auto destEnd = dest + destSize;
                    auto destStart = dest;
                    while (src < srcEnd && dest < destEnd)
                    {
                        auto ch = *src;
                        if (ch == '\\') // deal with escapes
                        {
                            if (src + 5 < srcEnd && src[1] == 'u' && IsHex4Digits(src + 2))
                            {
                                Escape6Decode(src, srcEnd, dest, destEnd);
                                continue;
                            }
                            else if (src + 1 < srcEnd && IsCharEscape2Char2(src[1]))
                            {
                                *dest++ = Escape2Char(src[1]);
                                src += 2;
                            }
                            continue;
                        }

                        // check to see if it's valid UTF (1-4 bytes), or not (invalid)
                        auto cb = Utf8::Is1Start(ch) ? 1 : Utf8::Is2Start(ch) ? 2 : Utf8::Is3Start(ch) ? 3 : Utf8::Is4Start(ch) ? 4 : 0;
                        if (cb == 0) break; // stop copying here

                        *dest++ = *src++; // optimized for UTF 1 byte (aka ASCII) (0..0x7f)
                        while (--cb > 0 && src < srcEnd && dest < destEnd)
                        {
                            *dest++ = *src++;
                        }
                    }
                    return dest - destStart;
                }

                static AJV_FN_NO_INLINE_(std::string) Decode(const char* ptr, size_t size)
                {
                    // if decoding isn't needed, just return the string
                    auto decode = JsonString::Encoder::Encoded(ptr, size);
                    if (!decode) return std::string(ptr, size);

                    // when decoding smaller strings, use on stack buffer
                    char stackBuffer[1024];
                    auto allocBiggerBuffer = size >= sizeof(stackBuffer);

                    // safely new/delete larger buffer, or use the on stack buffer
                    auto deleter = [&](char* p) { if (p != stackBuffer) delete[] p; };
                    std::unique_ptr<char[], decltype(deleter)> buffer(allocBiggerBuffer ? new char[size] : stackBuffer, deleter);

                    // finally ... decode the string
                    return std::string(buffer.get(), Decode(ptr, size, buffer.get(), size));
                }

                static bool IsEscaped(const char* ptr, size_t cch)
                {
                    auto end = ptr + cch;
                    while (ptr < end && *ptr != '\\') ptr++;
                    return ptr < end;
                }

                static bool IsInvalid(unsigned char ch) { return ch == 0 || (ch >= 0x80 && ch <= 0xbf) || ch >= 0xf5; }

                static bool IsSurrogateHigh(int point) { return point >= 0xd800 && point <= 0xdbff; }
                static uint32_t SurrogateHigh(int point) { return 0xd800 + ((point - 0x10000) >> 10); } // (10 bit)
                static uint32_t SurrogateLow(int point) { return 0xdc00 + ((point - 0x10000) &0x3ff); } // (10 bit)

                static int CodePoint(uint32_t highSurrogate, uint32_t lowSurrogate) {
                    return ((highSurrogate - 0xd800) << 10) + (lowSurrogate - 0xdc00) + 0x10000;
                }

                static int CodePoint(const char*& ptr, const char* end) { 
                    return ptr + 0 < end && Utf8::Is1Start(*ptr) ? Utf8::CodePoint1(ptr)
                         : ptr + 1 < end && Utf8::Is2Start(*ptr) ? Utf8::CodePoint2(ptr)
                         : ptr + 2 < end && Utf8::Is3Start(*ptr) ? Utf8::CodePoint3(ptr)
                         : ptr + 3 < end && Utf8::Is4Start(*ptr) ? Utf8::CodePoint4(ptr)
                         : '?';
                }

                static char* CodePointDecode(int point, char*& dest, char* destEnd) {
                    return Utf8::IsCodePoint1(point) ? Utf8::CodePoint1Decode(point, dest)
                         : Utf8::IsCodePoint2(point) && dest + 2 < destEnd ? Utf8::CodePoint2Decode(point, dest)
                         : Utf8::IsCodePoint3(point) && dest + 3 < destEnd ? Utf8::CodePoint3Decode(point, dest)
                         : Utf8::IsCodePoint4(point) && dest + 4 < destEnd ? Utf8::CodePoint4Decode(point, dest)
                         : Utf8::CodePoint1Decode('?', dest);
                }

                // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
                //  U+0000 ..  U+007f       (7 bit)     (1 byte)                            //                (7 bits)
                static bool Is1Start(unsigned char ch) { return ch <= 0x7f; }               //  0b0___(0..7)____(0..f)
                static uint8_t Mask1Start(unsigned char ch) { return ch & 0x7f; }           //  0b0xxx(0..7)xxxx(0..f)
                static char Mask1StartChar(int s) { return Mask1Start((char)s); }

                static bool IsCodePoint1(int point) { return point <= 0x7f; }
                static int CodePoint1(const char*& ptr) {
                    return *ptr++;
                }
                static char* CodePoint1Decode(int point, char*& dest) {
                    *dest++ = Mask1StartChar(point & 0xff);
                    return dest;
                }

                // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
                //  U+0080 ..  U+07ff       (11 bit)    (2 bytes)                           //                (5 bits)                (6 bits)
                static bool Is2Start(unsigned char ch) { return ch >= 0xc0 && ch <= 0xdf; } //  0b110_(c..d)____(0..f)  0b10__(0..3)____(0..f)
                static uint8_t Mask2Start(unsigned char ch) { return ch & 0x1f; }           //  0b000x(0..1)xxxx(0..f)
                static char Mask2StartChar(int s) { return 0xc0 + Mask2Start((char)s); }

                static bool IsCodePoint2(int point) { return point >= 0x0080 && point <= 0x07ff; }
                static int CodePoint2(const char*& ptr) { int point;
                    point =  Mask2Start(*ptr++) << 6;                                       //  0b000x(0..1)xxxx(0..f)
                    point += Next(*ptr++);                                                  //                          0b00xx(0..3)xxxx(0..f)
                    return point;
                }
                static char* CodePoint2Decode(int point, char*& dest) {
                    *dest++ = Mask2StartChar(point >> 6);
                    *dest++ = NextChar(point);
                    return dest;
                }

                // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
                //  U+0800 ..  U+ffff       (16 bit)    (3 bytes)                           //                (4 bits)                (6 bits)                (6 bits)
                static bool Is3Start(unsigned char ch) { return ch >= 0xe0 && ch <= 0xef; } //  0b1110(e..e)____(0..f)  0b10__(0..3)____(0..f)  0b10__(0..3)____(0..f)
                static uint8_t Mask3Start(unsigned char ch) { return ch & 0x0f; }           //  0b0000(0..0)xxxx(0..f)
                static char Mask3StartChar(int s) { return 0xe0 + Mask3Start((char)s); }

                static bool IsCodePoint3(int point) { return point >= 0x0800 && point <= 0xffff; }
                static int CodePoint3(const char*& ptr) { int point;
                    point =  Mask3Start(*ptr++) << 12;                                      //  0b0000(0..0)xxxx(0..f)
                    point += Next(*ptr++) << 6;                                             //                          0b00xx(0..3)xxxx(0..f)
                    point += Next(*ptr++);                                                  //                                                  0b00xx(0..3)xxxx(0..f)
                    return point;
                }
                static char* CodePoint3Decode(int point, char*& dest) {
                    *dest++ = Mask3StartChar(point >> 12);
                    *dest++ = NextChar(point >> 6);
                    *dest++ = NextChar(point);
                    return dest;
                }

                // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
                //  U+10000 ..  U+10ffff    (21 bit)    (4 bytes)                           //                (3 bits)                (6 bits)                (6 bits)                (6 bits)
                static bool Is4Start(unsigned char ch) { return ch >= 0xf0; }               //  0b1111(f..f)0___(0..7)  0b10__(0..3)____(0..f)  0b10__(0..3)____(0..f)  0b10__(0..3)____(0..f)
                static uint8_t Mask4Start(unsigned char ch) { return ch & 0x07; }           //  0b0000(0..0)0xxx(0..7)
                static char Mask4StartChar(int s) { return 0xf0 + Mask4Start((char)s); }

                static bool IsCodePoint4(int point) { return point >= 0x10000 && point <= 0x10ffff; }
                static int CodePoint4(const char*& ptr) { int point;
                    point =  Mask4Start(*ptr++) << 18;                                      //  0b0000(0..0)0xxx(0..7)
                    point += Next(*ptr++) << 12;                                            //                          0b00xx(0..3)xxxx(0..f)
                    point += Next(*ptr++) << 6;                                             //                                                  0b00xx(0..3)xxxx(0..f)
                    point += Next(*ptr++);                                                  //                                                                          0b00xx(0..3)xxxx(0..f)
                    return point;
                }
                static char* CodePoint4Decode(int point, char*& dest) {
                    *dest++ = Mask4StartChar(point >> 18);
                    *dest++ = NextChar(point >> 12);
                    *dest++ = NextChar(point >> 6);
                    *dest++ = NextChar(point);
                    return dest;
                }

                // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
                // Continuation byte        (6 bit)     (1 byte)                            //  0b10__(8..b)____(0..f)
                static uint8_t Next(unsigned char ch) { return ch & 0x3f; }                 //  0b00xx(0..3)xxxx(0..f)
                static char NextChar(int ch) { return 0x80 + Next((char)ch); }
            };

            static bool IsStartObject(char ch) { return ch == '{'; }
            static bool IsEndObject(char ch) { return ch == '}'; }
            static bool IsContinueObject(char ch) { return ch == ','; }

            static bool IsStartArray(char ch) { return ch == '['; }
            static bool IsEndArray(char ch) { return ch == ']'; }
            static bool IsContinueArray(char ch) { return ch == ','; }

            static bool IsStartString(char ch) { return ch == '\"'; }
            static bool IsEndString(char ch) { return ch == '\"'; }

            static bool IsStartBoolean(char ch) { return ch == 't' || ch == 'f'; }
            static bool IsStartNumber(char ch) { return ch == '-' || ch == '+' || IsDigit(ch); }
            static bool IsStartNull(char ch) { return ch == 'n'; }

            static char Escape2Char(unsigned char ch2) { return ch2 == 'b' ? '\b' : ch2 == 'f' ? '\f' : ch2 == 'n' ? '\n' : ch2 == 'r' ? '\r' : ch2 == 't' ? '\t' : ch2; }
            static char Escape2Char2(unsigned char ch) { return ch == '\b' ? 'b' : ch == '\f' ? 'f' : ch == '\n' ? 'n' : ch == '\r' ? 'r' : ch == '\t' ? 't' : ch; }
            static bool IsCharEscape2Char2(unsigned char ch)  { return ch ==  'b' || ch ==  'f' || ch ==  'n' || ch ==  'r' || ch ==  't' || ch == '\"' || ch == '\\' || ch == '/'; }
            static bool IsCharEscape2Required(unsigned char ch) { return ch == '\b' || ch == '\f' || ch == '\n' || ch == '\r' || ch == '\t' || ch == '\"' || ch == '\\'; }
            static bool IsCharEscape6Required(unsigned char ch) { return ch < ' ' || !Utf8::Is1Start(ch); }
            static bool IsCharControlCode(unsigned char ch) { return ch <= 0x1f; }

            static void Escape2(const char*& ptr1, char*& ptr2, char*& end2, size_t& cch)
            {
                if (ptr2 + 2 <= end2)
                {
                    *ptr2++ = '\\';
                    *ptr2++ = Escape2Char2(*ptr1);
                }
                ptr1 += 1;
                cch += 2;
            }

            static void Escape6(const char*& ptr1, const char* end1, char*& ptr2, char*& end2, size_t& cch)
            {
                auto point = Utf8::CodePoint(ptr1, end1);
                if (point < 0x10000 && ptr2 + 6 <= end2)
                {
                    Escape6(point, ptr2, cch);
                }
                else if (point >= 0x10000 && ptr2 + 12 <= end2)
                {
                    point -= 0x10000;
                    Escape6(0xd800 + (point / 0x400), ptr2, cch);
                    Escape6(0xdc00 + (point % 0x400), ptr2, cch);
                }
            }

            static void Escape6(size_t num, char*& ptr, size_t& cch) {
                ptr[0] = '\\';
                ptr[1] = 'u';
                ptr[2] = HexDigit((num >> 12) & 0x0f);
                ptr[3] = HexDigit((num >> 8) & 0x0f);
                ptr[4] = HexDigit((num >> 4) & 0x0f);
                ptr[5] = HexDigit(num & 0x0f);
                ptr += 6;
                cch += 6;
            }

            static const char* Escape6Decode(const char*& src, const char* srcEnd, char*& dest, char* destEnd)
            {
                src += 6;
                auto decoded1 = Escape6DecodeHex4(src - 4); // convert the last 4 hex characters
                int point = decoded1;

                // if it's a "high surrogate", there will be another \uxxxx (the "low surrogate")
                if (Utf8::IsSurrogateHigh(decoded1) && src + 5 < srcEnd && src[1] == 'u' && IsHex4Digits(src + 2))
                {
                    src += 6; // advance past and decode the low, and convert surrogates to point
                    auto decoded2 = Escape6DecodeHex4(src - 4);
                    point = Utf8::CodePoint(decoded1, decoded2);
                }

                return Utf8::CodePointDecode(point, dest, destEnd);
            }

            static uint32_t Escape6DecodeHex4(const char* ptr)
            {
                uint32_t decoded = HexValue(ptr[0]) << 12;
                decoded += HexValue(ptr[1]) << 8;
                decoded += HexValue(ptr[2]) << 4;
                decoded += HexValue(ptr[3]);
                return decoded;
            }

        };

        class Encoder : protected JsonString::Helpers
        {
        public:

            static bool Encoded(const char* ptr, size_t cch) { return Utf8::IsEscaped(ptr, cch); }
            static size_t Encode(const char* src, size_t srcSize, char* dest, size_t destSize) { return Utf8::Encode(src, srcSize, dest, destSize); }
            static size_t EncodedSize(const char* ptr, size_t cch) { return Utf8::EncodedSize(ptr, cch); }

            static std::string Decode(const char* src, size_t srcSize) { return Utf8::Decode(src, srcSize); }
            static size_t Decode(const char* src, size_t srcSize, char* dest, size_t destSize) { return Utf8::Decode(src, srcSize, dest, destSize); }
        };
    };
}

#endif // __AJV_JSON_STRING_H
