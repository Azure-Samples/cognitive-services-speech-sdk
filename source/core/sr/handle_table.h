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
        SPX_DBG_TRACE_FUNCTION();
    }

    ~CSpxHandleTable()
    {
        SPX_DBG_TRACE_FUNCTION();
    }

    Handle TrackHandle(std::shared_ptr<T> t)
    {
        Handle handle = SPXHANDLE_INVALID;

        T* ptr = t.get();
        if (ptr != nullptr)
        {
            handle = static_cast<Handle>(ptr);
        }

        m_handles.emplace(handle, t);
        return handle;
    }

    bool IsTracked(Handle handle)
    {
        return m_handles.find(handle) != m_handles.end();
    }

    std::shared_ptr<T> operator[](Handle handle)
    {
        SPX_THROW_HR_IF(SPXERR_INVALID_ARG, m_handles.find(handle) == m_handles.end());
        return m_handles[handle];
    }

    void StopTracking(Handle handle)
    {
        m_handles.erase(handle);
    }

    void Term()
    {
        m_handles.clear();
    }

private:

    std::map<Handle, std::shared_ptr<T>> m_handles;
};

class CSpxHandleTableManager
{
public:

    template<class T, class Handle>
    static CSpxHandleTable<T, Handle>* Get()
    {
        auto name = typeid(T).raw_name();

        if (m_tables->find(name) == m_tables->end())
        {
            auto ht = new CSpxHandleTable<T, Handle>();
            m_tables->emplace(name, ht);

            auto fn = [=]() { ht->Term(); };
            m_termFns->push_back(fn);
        }

        return (CSpxHandleTable<T, Handle>*) (*m_tables)[name];
    }

    static void Term()
    {
        for (auto termFn : *m_termFns.get())
        {
            termFn();
        }
    }


private:

    static std::unique_ptr<std::map<const char*, void*>> m_tables;
    static std::unique_ptr<std::list<std::function<void(void)>>> m_termFns;
};


using CSpxSharedPtrHandleTableManager = CSpxHandleTableManager;


}; // CARBON_IMPL_NAMESPACE()
