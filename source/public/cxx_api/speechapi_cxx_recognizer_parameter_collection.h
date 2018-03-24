//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_recognizer_parameter_collection.h: Public API declarations for RecognizerParameterCollection C++ class
//

#pragma once
#include <speechapi_cxx_value.h>


namespace CARBON_NAMESPACE_ROOT {
namespace Recognition {


class RecognizerParameter : public Value
{
public:

    RecognizerParameter(SPXRECOHANDLE hreco, const wchar_t* name) :
        m_hreco(hreco),
        m_name(name)
    {
    }

    // --- Value virtual overrides ---

    bool IsString() override { return HasString(m_hreco, m_name.c_str()); }
    std::wstring GetString(const wchar_t* defaultValue) override { return GetString(m_hreco, m_name.c_str(), defaultValue); }
    void SetString(const wchar_t* value) override { return SetString(m_hreco, m_name.c_str(), value); }

    bool IsNumber() override { return HasNumber(m_hreco, m_name.c_str()); }
    int32_t GetNumber(int32_t defaultValue) override { return GetNumber(m_hreco, m_name.c_str(), defaultValue); }
    void SetNumber(int32_t value) override { SetNumber(m_hreco, m_name.c_str(), value); }

    bool IsBool() override { return HasBool(m_hreco, m_name.c_str()); }
    bool GetBool(bool defaultValue) override { return GetBool(m_hreco, m_name.c_str(), defaultValue); }
    void SetBool(bool value) override { SetBool(m_hreco, m_name.c_str(), value); }

    // --- Static helpers ---
    
    static std::wstring GetString(SPXRECOHANDLE hreco, const wchar_t* name, const wchar_t* defaultValue)
    {
        const size_t cch = 1024;
        wchar_t sz[cch+1];
        SPX_THROW_ON_FAIL(Recognizer_GetParameter_String(hreco, name, sz, cch, defaultValue));
        return sz;
    }

    static int32_t GetNumber(SPXRECOHANDLE hreco, const wchar_t* name, int32_t defaultValue)
    {
        int32_t value;
        SPX_THROW_ON_FAIL(Recognizer_GetParameter_Int32(hreco, name, &value, defaultValue));
        return value;
    }

    static bool GetBool(SPXRECOHANDLE hreco, const wchar_t* name, bool defaultValue)
    {
        bool value;
        SPX_THROW_ON_FAIL(Recognizer_GetParameter_Bool(hreco, name, &value, defaultValue));
        return !!value;
    }

    static void SetString(SPXRECOHANDLE hreco, const wchar_t* name, const wchar_t* value)
    {
        SPX_THROW_ON_FAIL(Recognizer_SetParameter_String(hreco, name, value));
    }

    static void SetNumber(SPXRECOHANDLE hreco, const wchar_t* name, int32_t value)
    {
        SPX_THROW_ON_FAIL(Recognizer_SetParameter_Int32(hreco, name, value));
    }

    static void SetBool(SPXRECOHANDLE hreco, const wchar_t* name, bool value)
    {
        SPX_THROW_ON_FAIL(Recognizer_SetParameter_Bool(hreco, name, value));
    }

    static bool HasString(SPXRECOHANDLE hreco, const wchar_t* name)
    {
        return Recognizer_HasParameter_String(hreco, name);
    }

    static bool HasNumber(SPXRECOHANDLE hreco, const wchar_t* name)
    {
        return Recognizer_HasParameter_Int32(hreco, name);
    }

    static bool HasBool(SPXRECOHANDLE hreco, const wchar_t* name)
    {
        return Recognizer_HasParameter_Bool(hreco, name);
    }

private:

    SPXRECOHANDLE m_hreco;
    std::wstring m_name;
};


class RecognizerParameterCollection : public ValueCollection<SPXRECOHANDLE, RecognizerParameter>
{
public:

    RecognizerParameterCollection(SPXRECOHANDLE hreco) :
        ValueCollection(hreco)
    {
    }


    // TODO: Fix SWIG such that we don't need to expose the default methods below... 
    //       And then... once fixed ... delete the next 5 lines of code:
    //
    RecognizerParameterCollection() { SPX_REPORT_ON_FAIL(SPXERR_NOT_IMPL); throw SPXERR_NOT_IMPL; }
    RecognizerParameterCollection(RecognizerParameterCollection&&) { SPX_REPORT_ON_FAIL(SPXERR_NOT_IMPL); throw SPXERR_NOT_IMPL; }
    RecognizerParameterCollection(const RecognizerParameterCollection&) : ValueCollection(*this) { SPX_REPORT_ON_FAIL(SPXERR_NOT_IMPL); throw SPXERR_NOT_IMPL; }
    RecognizerParameterCollection& operator=(RecognizerParameterCollection&&) { SPX_REPORT_ON_FAIL(SPXERR_NOT_IMPL); throw SPXERR_NOT_IMPL; }
    RecognizerParameterCollection& operator=(const RecognizerParameterCollection&) { SPX_REPORT_ON_FAIL(SPXERR_NOT_IMPL); throw SPXERR_NOT_IMPL; }


private:

    // TODO: Fix SWIG such that we don't need to expose the default methods below... 
    //       And then... once fixed ... Uncomment the next 5 lines of code:
    //    
    // RecognizerParameterCollection() = delete;
    // RecognizerParameterCollection(RecognizerParameterCollection&&) = delete;
    // RecognizerParameterCollection(const RecognizerParameterCollection&) = delete;
    // RecognizerParameterCollection& operator=(RecognizerParameterCollection&&) = delete;
    // RecognizerParameterCollection& operator=(const RecognizerParameterCollection&) = delete;
};

} } // CARBON_NAMESPACE_ROOT :: Recognition
