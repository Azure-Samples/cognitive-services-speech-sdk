//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_value.h: Public API declarations for Value C++ class
//

#pragma once
#include <functional>
#include <speechapi_c_common.h>


namespace CARBON_NAMESPACE_ROOT {


class Value
{
public:

    Value(Value *ptr = nullptr) : m_delegatePtr(ptr) { }
    Value(Value&& other) : m_delegatePtr(std::move(other.m_delegatePtr)) { }

    virtual ~Value()
    {
        if (m_delegatePtr != nullptr)
        {
            delete m_delegatePtr;
        }
    }

    virtual bool IsString() { return m_delegatePtr->IsString(); }
    virtual void SetString(const wchar_t* value) { m_delegatePtr->SetString(value); }
    virtual std::wstring GetString(const wchar_t* defaultValue = L"") { return m_delegatePtr->GetString(defaultValue); }

    virtual bool IsNumber() { return m_delegatePtr->IsNumber(); }
    virtual void SetNumber(int32_t value) { m_delegatePtr->SetNumber(value); }
    virtual int32_t GetNumber(int32_t defaultValue = 0) { return m_delegatePtr->GetNumber(defaultValue); }

    virtual bool IsBool() { return m_delegatePtr->IsBool(); }
    virtual void SetBool(bool value) { m_delegatePtr->SetBool(value); }
    virtual bool GetBool(bool defaultValue = false) { return m_delegatePtr->GetBool(defaultValue); }

    const wchar_t* operator=(const wchar_t* value) { SetString(value); return value; }
    int32_t operator=(int32_t value) { SetNumber(value); return value; }
    bool operator=(bool value) { SetBool(value); return value; }

    operator const std::wstring() { return GetString(); }
    operator int32_t() { return GetNumber(); }
    operator bool() { return GetBool(); }


    // TODO: Fix SWIG such that we don't need to expose the default methods below... 
    //       And then... once fixed ... delete the next 3 lines of code:
    //
    Value(const Value&) { SPX_REPORT_ON_FAIL(SPXERR_NOT_IMPL); throw SPXERR_NOT_IMPL; }
    Value& operator=(Value&&) { SPX_REPORT_ON_FAIL(SPXERR_NOT_IMPL); throw SPXERR_NOT_IMPL; }
    const Value& operator=(const Value&) { SPX_REPORT_ON_FAIL(SPXERR_NOT_IMPL); throw SPXERR_NOT_IMPL; }


private:

    // TODO: Fix SWIG such that we don't need to expose the default methods below... 
    //       And then... once fixed ... Uncomment the next 3 lines of code:
    //
    // Value(const Value&) = delete;
    // Value& operator=(Value&&) = delete;
    // const Value& operator=(const Value&) = delete;

    Value* m_delegatePtr;
};


template<class Handle, class T>
class ValueCollection
{
public:

    ValueCollection() : m_handle(SPXHANDLE_INVALID) { }
    ValueCollection(Handle handle) : m_handle(handle) { }

    Value operator[](const wchar_t* name) { return Value(new T(m_handle, name)); }

    bool IsString(const wchar_t* name) { return T(m_handle, name).IsString(); }
    void SetString(const wchar_t* name, const wchar_t* value) { T(m_handle, name).SetString(value); }
    std::wstring GetString(const wchar_t* name, const wchar_t* defaultValue = L"") { return T(m_handle, name).GetString(defaultValue); }

    bool IsNumber(const wchar_t* name) { return T(m_handle, name).IsNumber(); }
    void SetNumber(const wchar_t* name, int32_t value) { T(m_handle, name).SetNumber(value); }
    int32_t GetNumber(const wchar_t* name, int32_t defaultValue = 0) { return T(m_handle, name).GetNumber(defaultValue); }

    bool IsBool(const wchar_t* name) { return T(m_handle, name).IsBool(); }
    void SetBool(const wchar_t* name, bool value) { T(m_handle, name).SetBool(value); }
    bool GetBool(const wchar_t* name, bool defaultValue = false) { return T(m_handle, name).GetBool(defaultValue); }


    // TODO: Fix SWIG such that we don't need to expose the default methods below... 
    //       And then... once fixed ... delete the next 5 lines of code:
    //
    /*ValueCollection(ValueCollection&&) { SPX_THROW_HR(SPXERR_NOT_IMPL); }
    ValueCollection(const ValueCollection&) { SPX_THROW_HR(SPXERR_NOT_IMPL); }
    ValueCollection& operator=(ValueCollection&&) { SPX_REPORT_ON_FAIL(SPXERR_NOT_IMPL); throw SPXERR_NOT_IMPL; }
    const ValueCollection& operator=(const ValueCollection&) { SPX_REPORT_ON_FAIL(SPXERR_NOT_IMPL); throw SPXERR_NOT_IMPL; }*/


private:

    // TODO: Fix SWIG such that we don't need to expose the default methods below... 
    //       And then... once fixed ... Uncomment the next 5 lines of code:
    //    
    // ValueCollection() = delete;
    // ValueCollection(ValueCollection&&) = delete;
    // ValueCollection(const ValueCollection&) = delete;
    // ValueCollection& operator=(ValueCollection&&) = delete;
    // const ValueCollection& operator=(const ValueCollection&) = delete;

    Handle m_handle;
};


} // CARBON_NAMESPACE_ROOT
