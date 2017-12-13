#pragma once
#include <functional>
#include <list>
#include <map>
#include <memory>


namespace CARBON_IMPL_NAMESPACE() {


template <class T, class Handle>
class CSpxHandleTable
{
public:

    CSpxHandleTable()
    {
    }

    ~CSpxHandleTable()
    {
    }

    Handle TrackHandle(std::shared_ptr<T> t)
    {
        // SPX_DBG_TRACE_VERBOSE("%s 0x%8x", __FUNCTION__, t.get());
        Handle handle = SPXHANDLE_INVALID;

        T* ptr = t.get();
        if (ptr != nullptr)
        {
            handle = reinterpret_cast<Handle>(ptr);
            m_handleMap.emplace(handle, t);
            m_ptrMap.emplace(ptr, handle);
        }

        return handle;
    }

    bool IsTracked(Handle handle)
    {
        return m_handleMap.find(handle) != m_handleMap.end();
    }

    bool IsTracked(T* ptr)
    {
        return m_ptrMap.find(ptr) != m_ptrMap.end();
    }

    std::shared_ptr<T> operator[](Handle handle)
    {
        auto item = m_handleMap.find(handle);
        SPX_THROW_HR_IF(SPXERR_INVALID_ARG, item == m_handleMap.end());
        return item->second;
    }

    Handle operator[](T* ptr)
    {
        auto item = m_ptrMap.find(ptr);
        SPX_IFTRUE_THROW_HR(item == m_ptrMap.end(), SPXERR_INVALID_ARG);
        return item->second;
    }

    void StopTracking(Handle handle)
    {
        if (IsTracked(handle))
        {
            T* ptr = m_handleMap[handle].get();
            m_handleMap.erase(handle);
            m_ptrMap.erase(ptr);
        }
    }

    void StopTracking(T* ptr)
    {
        if (IsTracked(ptr))
        {
            Handle handle = m_ptrMap[ptr];
            m_handleMap.erase(handle);
            m_ptrMap.erase(ptr);
        }
    }
    void Term()
    {
        m_handleMap.clear();
        m_ptrMap.clear();
    }

private:

    std::map<Handle, std::shared_ptr<T>> m_handleMap;
    std::map<T*, Handle> m_ptrMap;
};

class CSpxSharedPtrHandleTableManager
{
public:

    template<class T, class Handle>
    static CSpxHandleTable<T, Handle>* Get()
    {
        auto name = typeid(T).raw_name();

        if (s_tables->find(name) == s_tables->end())
        {
            auto ht = new CSpxHandleTable<T, Handle>();
            s_tables->emplace(name, ht);

            auto fn = [=]() { ht->Term(); };
            s_termFns->push_back(fn);
        }

        return (CSpxHandleTable<T, Handle>*) (*s_tables)[name];
    }

    static void Term()
    {
        for (auto termFn : *s_termFns.get())
        {
            termFn();
        }

        s_tables->clear();
        s_termFns->clear();
    }


private:

    static std::unique_ptr<std::map<const char*, void*>> s_tables;
    static std::unique_ptr<std::list<std::function<void(void)>>> s_termFns;
};


}; // CARBON_IMPL_NAMESPACE()
