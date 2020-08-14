//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace USP {

/// <summary>
/// Possible HTTP status codes returned by the service for upgrade requests
/// </summary>
enum class HttpStatusCode
{
    /// <summary>
    /// Success response
    /// </summary>
    OK = 200,
    /// <summary>
    /// This and all future requests should be directed to URL returned in the Location header in the response. HTTP method
    /// may change here.
    /// </summary>
    MOVED_PERMANENTLY = 301,
    /// <summary>
    /// This request should directed to the URL returned in the Location header in the response. Future requests may go to the
    /// original URL. HTTP method is not allowed to change here.
    /// </summary>
    TEMP_REDIRECT = 307,
    /// <summary>
    /// This request should directed to the URL returned in the Location header in the response. Future requests may go to the
    /// original URL. HTTP method is not allowed to change here.
    /// </summary>
    PERM_REDIRECT = 308,
    /// <summary>
    /// The request sent is invalid. Please double check you have all the required
    /// </summary>
    BAD_REQUEST = 400,
    /// <summary>
    /// The request is not authorized. Check the server response to see what authentication headers/query
    /// parameters are needed.
    /// </summary>
    UNAUTHORIZED = 401,
    /// <summary>
    /// The request is forbidden by the server. Please double check that you have the correct
    /// credentials
    /// </summary>
    FORBIDDEN = 403,
    /// <summary>
    /// The path used could not be found
    /// </summary>
    NOT_FOUND = 404,
    /// <summary>
    /// The request timed out.
    /// </summary>
    TIMEOUT = 408,
    /// <summary>
    /// The request could not be processed because of a conflict in the current state of the requested resource.
    /// </summary>
    CONFLICT = 409,
    /// <summary>
    /// The request specifies a media type that the service or requested resource does not support.
    /// </summary>
    UNSUPPORTED_MEDIA_TYPE = 415,
    /// <summary>
    /// You have been throttled by the service. You should wait some time and try again
    /// </summary>
    TOO_MANY_REQUESTS = 429,
    /// <summary>
    /// (nginx extension) The request was closed before the server could issue a response.
    /// </summary>
    CLIENT_CLOSED_REQUEST = 499,
    /// <summary>
    /// The server crashed while processing the upgrade request. You should retry again later
    /// </summary>
    INTERNAL_ERROR = 500,
    /// <summary>
    /// The requested operation has not yet been implemented on the service.
    /// </summary>
    NOT_IMPLEMENTED = 501,
    /// <summary>
    /// The server received an invalid response from an upstream service while acting as a gateway for the request.
    /// </summary>
    BAD_GATEWAY = 502,
    /// <summary>
    /// The service is down or unavailable. You should retry again
    /// </summary>
    SERVICE_UNAVAILABLE = 503,
    /// <summary>
    /// The server does not support the HTTP protocol version used in the request.
    /// </summary>
    HTTP_VERSION_NOT_SUPPORTED = 505,
    /// <summary>
    /// Transparent content negotiation for the request results in a circular reference.
    /// </summary>
    VARIANT_ALSO_NEGOTIATES = 506,
    /// <summary>
    /// The server was unable to store data needed to fulfill the request.
    /// </summary>
    INSUFFICIENT_STORAGE = 507,
    /// <summary>
    /// The server detected an infinite loop while processing the request.
    /// </summary>
    LOOP_DETECTED = 508,
    /// <summary>
    /// The server exceeded its allocated bandwidth while processing the request.
    /// </summary>
    BANDWIDTH_EXCEEDED = 509,
    /// <summary>
    /// Further extensions to the request are required for the server to fulfill it.
    /// </summary>
    NOT_EXTENDED = 510,
};

}}}}
