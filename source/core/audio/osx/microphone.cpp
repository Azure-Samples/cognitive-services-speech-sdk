#include "stdafx.h"

#include "microphone.h"
#include "ispxinterfaces.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

using namespace std;

shared_ptr<ISpxAudioPump> Microphone::Create() 
{
    SPX_THROW_HR(SPXERR_NOT_IMPL);
    return nullptr;
}

} } } } // Microsoft::CognitiveServices::Speech::Impl
