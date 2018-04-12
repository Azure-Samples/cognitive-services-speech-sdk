//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_value.h: Public API declarations for Value C++ class
//

#pragma once
#include <functional>
#include <speechapi_cxx_common.h>
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


class BaseValueCollection
{
public:

    BaseValueCollection() { }
    virtual ~BaseValueCollection() { }

    virtual Value operator[](const wchar_t* name) = 0;

    virtual bool ContainsString(const wchar_t* name) = 0;
    virtual void SetString(const wchar_t* name, const wchar_t* value) = 0;
    virtual std::wstring GetString(const wchar_t* name, const wchar_t* defaultValue = L"") = 0;

    virtual bool ContainsNumber(const wchar_t* name) = 0;
    virtual void SetNumber(const wchar_t* name, int32_t value) = 0;
    virtual int32_t GetNumber(const wchar_t* name, int32_t defaultValue = 0) = 0;

    virtual bool ContainsBool(const wchar_t* name) = 0;
    virtual void SetBool(const wchar_t* name, bool value) = 0;
    virtual bool GetBool(const wchar_t* name, bool defaultValue = false) = 0;
};


template<class T>
class ValueCollection : public BaseValueCollection
{
public:

    ValueCollection() { }
    ~ValueCollection() { }

    Value operator[](const wchar_t* name) override { return Value(new T(name)); }

    bool ContainsString(const wchar_t* name) override { return T(name).IsString(); }
    void SetString(const wchar_t* name, const wchar_t* value) override { T(name).SetString(value); }
    std::wstring GetString(const wchar_t* name, const wchar_t* defaultValue = L"") override { return T(name).GetString(defaultValue); }

    bool ContainsNumber(const wchar_t* name) override { return T(name).IsNumber(); }
    void SetNumber(const wchar_t* name, int32_t value) override { T(name).SetNumber(value); }
    int32_t GetNumber(const wchar_t* name, int32_t defaultValue = 0) override { return T(name).GetNumber(defaultValue); }

    bool ContainsBool(const wchar_t* name) override { return T(name).IsBool(); }
    void SetBool(const wchar_t* name, bool value) override { T(name).SetBool(value); }
    bool GetBool(const wchar_t* name, bool defaultValue = false) override { return T(name).GetBool(defaultValue); }

    // TODO: Fix SWIG such that we don't need to expose the default methods below... 
    //       And then... once fixed ... delete the next 3 lines of code:
    //
    ValueCollection(ValueCollection&&) = delete;
    ValueCollection(const ValueCollection&) = delete;
    ValueCollection& operator=(ValueCollection&&) = delete;
    const ValueCollection& operator=(const ValueCollection&) = delete;
};


template<class Handle, class T>
class HandleValueCollection : public BaseValueCollection
{
public:

    HandleValueCollection() : m_handle(SPXHANDLE_INVALID) { }
    HandleValueCollection(Handle handle) : m_handle(handle) { }
    ~HandleValueCollection() { }

    Value operator[](const wchar_t* name) override { return Value(new T(m_handle, name)); }

    bool ContainsString(const wchar_t* name) override { return T(m_handle, name).IsString(); }
    void SetString(const wchar_t* name, const wchar_t* value) override { T(m_handle, name).SetString(value); }
    std::wstring GetString(const wchar_t* name, const wchar_t* defaultValue = L"") override { return T(m_handle, name).GetString(defaultValue); }

    bool ContainsNumber(const wchar_t* name) override { return T(m_handle, name).IsNumber(); }
    void SetNumber(const wchar_t* name, int32_t value) override { T(m_handle, name).SetNumber(value); }
    int32_t GetNumber(const wchar_t* name, int32_t defaultValue = 0) override { return T(m_handle, name).GetNumber(defaultValue); }

    bool ContainsBool(const wchar_t* name) override { return T(m_handle, name).IsBool(); }
    void SetBool(const wchar_t* name, bool value) override { T(m_handle, name).SetBool(value); }
    bool GetBool(const wchar_t* name, bool defaultValue = false) override { return T(m_handle, name).GetBool(defaultValue); }


    // TODO: Fix SWIG such that we don't need to expose the default methods below... 
    //       And then... once fixed ... delete the next 5 lines of code:
    //
    HandleValueCollection(HandleValueCollection&&) { SPX_THROW_HR(SPXERR_NOT_IMPL); }
    HandleValueCollection(const HandleValueCollection&) { SPX_THROW_HR(SPXERR_NOT_IMPL); }
    HandleValueCollection& operator=(HandleValueCollection&&) { SPX_REPORT_ON_FAIL(SPXERR_NOT_IMPL); throw SPXERR_NOT_IMPL; }
    const HandleValueCollection& operator=(const HandleValueCollection&) { SPX_REPORT_ON_FAIL(SPXERR_NOT_IMPL); throw SPXERR_NOT_IMPL; }


protected:

    Handle m_handle;

private:

    // TODO: Fix SWIG such that we don't need to expose the default methods below... 
    //       And then... once fixed ... Uncomment the next 5 lines of code:
    //    
    // HandleValueCollection() = delete;
    // HandleValueCollection(HandleValueCollection&&) = delete;
    // HandleValueCollection(const HandleValueCollection&) = delete;
    // HandleValueCollection& operator=(HandleValueCollection&&) = delete;
    // const HandleValueCollection& operator=(const HandleValueCollection&) = delete;

};


} // CARBON_NAMESPACE_ROOT
