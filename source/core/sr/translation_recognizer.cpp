#include "stdafx.h"
#include "translation_recognizer.h"
#include "service_helpers.h"
#include "site_helpers.h"


namespace CARBON_IMPL_NAMESPACE() {


CSpxTranslationRecognizer::CSpxTranslationRecognizer()
{
    SPX_DBG_TRACE_FUNCTION();
}

CSpxTranslationRecognizer::~CSpxTranslationRecognizer()
{
    SPX_DBG_TRACE_FUNCTION();
}

void CSpxTranslationRecognizer::Init()
{
    CSpxRecognizer::Init();
}

} // CARBON_IMPL_NAMESPACE
