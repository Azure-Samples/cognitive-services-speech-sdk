#include "stdafx.h"

#include "microphone.h"
#include "ispxinterfaces.h"


namespace CARBON_IMPL_NAMESPACE() {

using namespace std;

shared_ptr<ISpxAudioPump> Microphone::Create() 
{
    SPX_THROW_HR(SPXERR_NOT_IMPL);
    return nullptr;
}

} // CARBON_IMPL_NAMESPACE()