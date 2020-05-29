//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// http_client.cpp: Private implementation declarations for participant
//
#include "stdafx.h"

#include "http_client.h"

#include <json.h>
#include <http_request.h>
#include <http_response.h>
#include <http_headers.h>

using json = nlohmann::json;

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

void CSpxHttpClient::ProcessAudio(const DataChunkPtr& audioChunk)
{
    size_t bufferSize = (size_t)audioChunk->size;
    auto buffer = audioChunk->data.get();
    SPX_TRACE_INFO("copy %lu data into http client buffer", bufferSize);

    std::copy(buffer, buffer + bufferSize, back_inserter(m_audioData));
}

//do a POST BINARY data
std::unique_ptr<HttpResponse> CSpxHttpClient::FlushAudio()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    SPX_DBG_TRACE_INFO("end point url: %s", m_endPoint.EndpointUrl().c_str());

    HttpRequest request(m_endPoint);
    request.SetRequestHeader(m_headerName, m_subscriptionKey);
    request.SetRequestHeader(USP::headers::contentType, "audio/wav; codecs=audio/pcm");

    request.SetPath(m_endPoint.Path());

    // always clear the m_audioData even if there is exception.
    auto finish = std::shared_ptr<void>(nullptr, [&](void*) { m_audioData.clear();});

    std::unique_ptr<HttpResponse> response;
    try
    {
        response = request.SendRequest(HTTPAPI_REQUEST_POST, m_audioData.data(), m_audioData.size());
    }
    catch (const HttpException& e)
    {
        SPX_DBG_TRACE_ERROR("Error in a HTTP POST with audio data. '%s'", e.what());
    }
    catch (...)
    {
    }

    if (response)
    {
        SPX_TRACE_ERROR_IF(!response->IsSuccess(), "flush audio failed with HTTP %u [%s] url:'%s'",
            response->GetStatusCode(),
            response->ReadContentAsString().c_str(),
            m_endPoint.EndpointUrl().c_str());
    }

    return response;
}

}}}}
