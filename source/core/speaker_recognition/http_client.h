//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// http_client.h: A thin wrapper for chunked encoding vs sending all data at once.
//

#pragma once

#include "ispxinterfaces.h"
#include <ispxinterfaces.h>
#include "http_request.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class ISpxHttpDataClient : public ISpxInterfaceBaseFor<ISpxHttpDataClient>
{
public:
    virtual void ProcessAudio(const DataChunkPtr& audioChunk) = 0;
    virtual std::unique_ptr<HttpResponse> FlushAudio() = 0;
};

class CSpxHttpChunkedClient : public ISpxHttpDataClient
{
public:

    CSpxHttpChunkedClient() = default;
    virtual ~CSpxHttpChunkedClient() = default;

    // --- ISpxHttpDataClient
    virtual void ProcessAudio(const DataChunkPtr& audioChunk) override;
    virtual std::unique_ptr<HttpResponse> FlushAudio() override;

};

class CSpxHttpClient : public ISpxHttpDataClient
{
public:

    CSpxHttpClient(const HttpEndpointInfo& endPoint, const std::string& sub, const std::string& header) :
        m_endPoint(endPoint), m_subscriptionKey(sub), m_headerName(header)
    {
    }

    virtual ~CSpxHttpClient() = default;

    // --- ISpxHttpDataClient
    virtual void ProcessAudio(const DataChunkPtr& audioChunk) override;
    virtual std::unique_ptr<HttpResponse> FlushAudio() override;

private:
    DISABLE_COPY_AND_MOVE(CSpxHttpClient);

    std::vector<uint8_t> m_audioData;

    HttpEndpointInfo m_endPoint;
    std::string m_subscriptionKey;
    std::string m_headerName;
};

 }}}}
