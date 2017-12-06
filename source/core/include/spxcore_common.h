#pragma once

#include <spxerror.h>
#include <spxdebug.h>
#include <speechapi_c_common.h>
#include <memory>


#define CARBON_IMPL_NAMESPACE() CarbonIMPL


namespace CARBON_IMPL_NAMESPACE() {


class ISpxRecognizer;


class CSpxCarbonFactory
{
public:

    static std::shared_ptr<CSpxCarbonFactory> Get()
    {
        return std::make_shared<CSpxCarbonFactory>();
    }

    template <class T, class I, class... Types>
    inline std::shared_ptr<I> make_shared(Types&&... Args)
    {
        std::shared_ptr<T> ptr = std::make_shared<T>(std::forward<Types>(Args)...);
        auto it = std::dynamic_pointer_cast<I>(ptr);
        return it;
    }
};


template <class T, class I, class... Types>
inline std::shared_ptr<I> SpxMakeShared(Types&&... Args)
{
    auto factory = CSpxCarbonFactory::Get();
    return factory->make_shared<T, I>(std::forward<Types>(Args)...);
};


}; // CARBON_IMPL_NAMESPACE()
