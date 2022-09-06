//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

// <cpp-enumerate-audio-device>
#include <cstdio>
#include <mmdeviceapi.h>
#include <Functiondiscoverykeys_devpkey.h>

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);

#define EXIT_ON_ERROR(hr)  \
              if (FAILED(hr)) { goto Exit; }

#define SAFE_RELEASE(p)  \
              if ((p) != NULL)  \
                { (p)->Release(); (p) = NULL; }

void ListEndpoints(EDataFlow dataFlow);

int main()
{
    (void)CoInitializeEx(NULL, COINIT_MULTITHREADED);
    ListEndpoints(eCapture);
    ListEndpoints(eRender);
}

//-----------------------------------------------------------
// This function enumerates all active (plugged in) audio
// rendering or capture endpoint devices. It prints the friendly
// name and endpoint ID string of each endpoint device.
//-----------------------------------------------------------
void ListEndpoints(EDataFlow dataFlow)
{
    HRESULT hr = S_OK;
    IMMDeviceEnumerator* pEnumerator = NULL;
    IMMDeviceCollection* pCollection = NULL;
    IMMDevice* pEndpoint = NULL;
    IPropertyStore* pProps = NULL;
    LPWSTR pwszID = NULL;
    PROPVARIANT varName;

    PropVariantInit(&varName);

    hr = CoCreateInstance(CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, IID_IMMDeviceEnumerator, (void**)&pEnumerator);
    EXIT_ON_ERROR(hr);

    hr = pEnumerator->EnumAudioEndpoints(dataFlow, DEVICE_STATE_ACTIVE, &pCollection);
    EXIT_ON_ERROR(hr);

    UINT  count;
    hr = pCollection->GetCount(&count);
    EXIT_ON_ERROR(hr);

    if (count == 0)
    {
        printf("No endpoints found.\n");
    }

    // Each iteration prints the name of an endpoint device.
    for (ULONG i = 0; i < count; i++)
    {
        // Get pointer to endpoint number i.
        hr = pCollection->Item(i, &pEndpoint);
        EXIT_ON_ERROR(hr);

        // Get the endpoint ID string.
        hr = pEndpoint->GetId(&pwszID);
        EXIT_ON_ERROR(hr);

        hr = pEndpoint->OpenPropertyStore(STGM_READ, &pProps);
        EXIT_ON_ERROR(hr);

        // Initialize container for property value.
        PropVariantInit(&varName);

        // Get the endpoint's friendly-name property.
        hr = pProps->GetValue(PKEY_Device_FriendlyName, &varName);
        EXIT_ON_ERROR(hr);

        // Print endpoint friendly name and endpoint ID.
        printf("%s endpoint %d: \"%S\" (%S)\n", (dataFlow == eCapture ? "Capture" : "Render"), i, varName.pwszVal, pwszID);

        CoTaskMemFree(pwszID);
        pwszID = NULL;
        PropVariantClear(&varName);
    }

Exit:
    CoTaskMemFree(pwszID);
    pwszID = NULL;
    PropVariantClear(&varName);
    SAFE_RELEASE(pEnumerator);
    SAFE_RELEASE(pCollection);
    SAFE_RELEASE(pEndpoint);
    SAFE_RELEASE(pProps);
}
// </cpp-enumerate-audio-device>
