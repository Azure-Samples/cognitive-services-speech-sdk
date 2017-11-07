#pragma once
#include <exception>


namespace CARBON_NAMESPACE_ROOT { // ----------------------------------------


class NotYetImplementedException : public std::exception
{
public:

    NotYetImplementedException() : std::exception("CARBON: not yet implemented"){};
};


};

