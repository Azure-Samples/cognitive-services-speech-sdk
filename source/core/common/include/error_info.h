//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license
// information.
//

#pragma once

// In the absence of an stdafx.h for the common includes
#ifndef __SPX_THROW_HR_IMPL
#include "exception.h"
#define __SPX_THROW_HR_IMPL(hr) Microsoft::CognitiveServices::Speech::Impl::ThrowWithCallstack(hr)
#endif

#include "ispxinterfaces.h"
#include "http_status_codes.h"
#include "i_web_socket_state.h"
#include "recognition_status.h"

using namespace Microsoft::CognitiveServices::Speech::USP;

namespace Microsoft { namespace CognitiveServices { namespace Speech { namespace Impl {

  class ErrorInfo
    : public ISpxErrorInformation {
  private:
    std::string error_details;
    const CancellationErrorCode cancellation_code;
    const CancellationReason cancellation_reason;
    const RetryMode retry_mode;
    const int category_code;
    const int status_code;

  public:
    ErrorInfo(
        std::string details,
        int categoryCode,
        int statusCode,
        CancellationErrorCode cancellationCode,
        CancellationReason reason,
        RetryMode retryMode)
        : error_details{ std::move(details) }, cancellation_code{ cancellationCode },
        cancellation_reason{ reason }, retry_mode{ retryMode }, category_code{ categoryCode },
        status_code{ statusCode }
    {
    }

    // Directly generates an error based on the provided cancellation code and error details,
    // needed when there's no other origination information to derive the error from.
    static std::shared_ptr<ISpxErrorInformation> FromExplicitError(
        CancellationErrorCode error,
        const std::string& errorDetails);

    // Generates an error based on a RecognitionStatus, as received from the SR engine adapter
    // layer.
    static std::shared_ptr<ISpxErrorInformation> FromRecognitionStatus(
        RecognitionStatus status,
        const std::string& errorDetails);

    // Generates an error based on a WebSocket error and/or disconnection, appending provided
    // details.
    static std::shared_ptr<ISpxErrorInformation> FromWebSocket(
        WebSocketError error,
        int status,
        const std::string& = "");
    static std::shared_ptr<ISpxErrorInformation> FromWebSocket(
        WebSocketError error,
        WebSocketDisconnectReason reason,
        const std::string& errorDetails = "")
    {
      return FromWebSocket(error, (int)reason, errorDetails);
    }

    // Generates an error based on the provided HTTP status code, prepending and appending (as
    // applicable) the provided message components.
    static std::shared_ptr<ISpxErrorInformation> FromHttpStatus(
        HttpStatusCode status,
        const std::string& messagePrefix = "",
        const std::string& messageSuffix = "");

    // A less desirable factory method, this creates a new error as a copy of an existing error
    // with additional data appended to the message. This should be moved into a single-origination
    // and/or nested error model with future improvements.
    static std::shared_ptr<ISpxErrorInformation> FromErrorWithAppendedDetails(
        const std::shared_ptr<ISpxErrorInformation>& previousError,
        const std::string& extraDetails);

    // A less desirable factory method, this creates an error with minimal, standard information
    // (reporting a runtime error) with only the details/message customized.
    static std::shared_ptr<ISpxErrorInformation> FromRuntimeMessage(const std::string& message);

  // ISpxErrorInformation implementation
  public:
    // Gets the full message text associated with this error.
    const std::string GetDetails() final override { return error_details; }

    // Gets the public-facing error code enumeration value associated with this error.
    CancellationErrorCode GetCancellationCode() final override { return cancellation_code; }

    // Gets the public-facing cancellation reason (usually "error") associated with the error.
    CancellationReason GetCancellationReason() final override { return cancellation_reason; }

    // Gets the primary code associated with the error, e.g. the WebSocket status code.
    int GetCategoryCode() final override { return category_code; }

    // When applicable, gets the secondary code associated with the error, e.g.the underlying
    // HTTP status code for a WebSocket error.
    int GetStatusCode() final override { return status_code; }

    // Gets the retry mode for this error, which defines how automatic retry attempts should be
    // treated when the error is encountered.
    RetryMode GetRetryMode() final override { return retry_mode; }
  };

}}}} // namespace Microsoft::CognitiveServices::Speech::Impl
