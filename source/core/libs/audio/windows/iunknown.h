#pragma once
#include <windows.h>

template<typename T>
class IUnknownBase : public T
{
public:
    IUnknownBase()
        : m_refCount(0)
    {
    }

    virtual ~IUnknownBase() = default;

    STDMETHODIMP_(HRESULT) QueryInterface(const IID &riid, void** ppv)
    {
        HRESULT hr = E_NOINTERFACE;
        *ppv = NULL;

        if (riid == __uuidof(T))
        {
            AddRef();
            *ppv = static_cast<T*>(this);
            hr = S_OK;
        }

        return hr;
    }

    STDMETHODIMP_(ULONG) AddRef()
    {
        return InterlockedIncrement(&m_refCount);
    }

    STDMETHODIMP_(ULONG) Release()
    {
        LONG ref = InterlockedDecrement(&m_refCount);

        if (ref == 0)
        {
            delete this;
        }

        return ref;
    }

private:
    volatile ULONG m_refCount;
};


template<typename T, typename S>
class ComPtrList
{
public:
    ComPtrList()
        : p(NULL)
        , _size(0)
    {
    }

    ~ComPtrList()
    {
        release();
    }

    T***operator&()
    {
        release();
        return &p;
    }

    S*size_ptr()
    {
        return &_size;
    }

    T* operator[](int index)
    {
        return p[index];
    }

    S size() { return _size; }
private:
    void release()
    {
        if (NULL != p)
        {
            for (S i = 0; i < _size; i++)
            {
                p[i]->Release();
            }

            CoTaskMemFree(p);
            p = NULL;
        }
    }
    T** p;
    S   _size;
};