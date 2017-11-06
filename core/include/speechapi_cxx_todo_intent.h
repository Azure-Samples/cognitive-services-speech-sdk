#pragma once

#include <speechapi_cxx_recognizer.h>

namespace CARBON_NAMESPACE_ROOT::Recognition::Intent { // ---------------

class IntentRecognizer : Recognizer 
{
public:

    IntentRecognizer() : Recognizer(m_parameters) { throw nullptr; };
    virtual ~IntentRecognizer() = default;

    bool IsEnabled() override { throw nullptr; }
    void Enable() override { throw nullptr; }
    void Disable() override { throw nullptr; }

private:

    RecognizerParameters m_parameters;
};

}
