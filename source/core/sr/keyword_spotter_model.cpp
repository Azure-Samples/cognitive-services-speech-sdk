#include "stdafx.h"
#include "keyword_spotter_model.h"
#include "file_utils.h"
#include "string_utils.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


void CSpxKwsModel::InitFromFile(const wchar_t* fileName)
{
    if (fileName) {
        SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

        SPX_IFTRUE_THROW_HR(fileName == nullptr || fileName[0] == '\0', SPXERR_INVALID_ARG);
        SPX_IFTRUE_THROW_HR(!m_fileName.empty(), SPXERR_ALREADY_INITIALIZED);

        m_fileName = fileName;

        FILE* file = nullptr;
        PAL::fopen_s(&file, PAL::ToString(std::wstring(fileName)).c_str(), "rb");
        if (file != nullptr) fclose(file);

        SPX_IFTRUE_THROW_HR(file == nullptr, SPXERR_FILE_OPEN_FAILED);
    }
}


} } } } // Microsoft::CognitiveServices::Speech::Impl
