//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license
// information.
//

#include "error_info.h"
#include <sstream>

namespace Microsoft { namespace CognitiveServices { namespace Speech { namespace Impl {

  enum class ErrorSource
  {
    Unspecified,
    Explicit,
    RecognitionStatus,
    WebSocket,
    HttpStatus,
  };

  static constexpr int SKIP_HTTP_STATUS_VALIDATION = -32760;

  // This is intended to be "the one true place" that we generate all of the internal pieces for our
  // externally-facing errors.
  std::shared_ptr<ErrorInfo> create_error(
      ErrorSource source,
      const int categoryCode,
      const int statusCode,
      const std::string& messagePrefix,
      const std::string& messageBody,
      const std::string& messageSuffix)
  {
    std::stringstream msg;
    auto cancellationCode{ CancellationErrorCode::RuntimeError };
    auto cancellationReason{ CancellationReason::Error };
    auto retryMode{ ISpxErrorInformation::RetryMode::Allowed };

    switch (source)
    {
      case ErrorSource::Explicit:
        cancellationCode = (CancellationErrorCode)categoryCode;
        msg << messageBody;
        retryMode = ISpxErrorInformation::RetryMode::NotAllowed;
        break;
      case ErrorSource::RecognitionStatus:
        switch ((RecognitionStatus)categoryCode)
        {
          case RecognitionStatus::Error:
            msg << "The speech service encountered an internal error and could not continue.";
            cancellationCode = CancellationErrorCode::ServiceError;
            break;
          case RecognitionStatus::TooManyRequests:
            msg << "The number of parallel requests exceeded the number of allowed concurrent "
                   "transcriptions.";
            cancellationCode = CancellationErrorCode::TooManyRequests;
            break;
          case RecognitionStatus::BadRequest:
            msg << "Invalid parameter or unsupported audio format in the request.";
            cancellationCode = CancellationErrorCode::BadRequest;
            break;
          case RecognitionStatus::Forbidden:
            msg << "The recognizer is using a free subscription that ran out of quota.";
            cancellationCode = CancellationErrorCode::Forbidden;
            break;
          case RecognitionStatus::ServiceUnavailable:
            msg << "The service is currently unavailable.";
            cancellationCode = CancellationErrorCode::ServiceUnavailable;
            break;
          case RecognitionStatus::InvalidMessage:
            msg << "Invalid response.";
            cancellationCode = CancellationErrorCode::ServiceError;
            break;
          case RecognitionStatus::Success:
          case RecognitionStatus::EndOfDictation:
          case RecognitionStatus::InitialSilenceTimeout:
          case RecognitionStatus::InitialBabbleTimeout:
          case RecognitionStatus::NoMatch:
            msg << "Unexpected error callback for non-error recognition status.";
            cancellationCode = CancellationErrorCode::RuntimeError;
            break;
          default:
            msg << "Runtime Error: invalid recognition status: " << categoryCode << ".";
            cancellationCode = CancellationErrorCode::RuntimeError;
            break;
        }
        msg << " Response text: " << messageBody;
        break;
      case ErrorSource::WebSocket:
        // The default (and most common) thing we'll report for WebSocket errors is a "connection
        // failure"
        cancellationCode = CancellationErrorCode::ConnectionFailure;

        switch ((WebSocketError)categoryCode)
        {
          case WebSocketError::REMOTE_CLOSED:
            msg << "Connection was closed by the remote host. Error code: " << statusCode
                << ". Error details: " << messageBody;

            switch ((WebSocketDisconnectReason)statusCode)
            {
              case WebSocketDisconnectReason::ProtocolError:
              case WebSocketDisconnectReason::InvalidPayloadData:
                retryMode = ISpxErrorInformation::RetryMode::NotAllowed;
                break;
              default:
                // No special handling for other reasons
                break;
            }

            break;
          case WebSocketError::CONNECTION_FAILURE:
            msg << "Connection failed (no connection to the remote host). Internal error: "
                << statusCode << ". Error details: " << messageBody;
            break;
          case WebSocketError::WEBSOCKET_UPGRADE:
            // An HTTP 200 response is not a valid upgrade response from a web socket server. We should get back
            // HTTP 101. Set a flag to skip status code validation otherwise we'll return nullptr in this particular
            // case even though we have an error
            return create_error(
                ErrorSource::HttpStatus, statusCode, SKIP_HTTP_STATUS_VALIDATION, "WebSocket upgrade failed:", messageBody, "");
          case WebSocketError::WEBSOCKET_SEND_FRAME:
            msg << "Failure while sending a frame over the WebSocket connection. "
                << "Internal error: " << statusCode << ". Error details: " << messageBody;
            break;

          case WebSocketError::WEBSOCKET_ERROR:
            msg << "WebSocket operation failed. Internal error: " << statusCode << ". "
                << "Error details: " << messageBody;
            break;
          case WebSocketError::DNS_FAILURE:
            msg << "DNS connection failed (the remote host did not respond). Internal error: "
                << statusCode << ".";
            break;
          case WebSocketError::UNKNOWN:
            msg << "Unknown transport error.";
            break;
        }
        break;
      case ErrorSource::HttpStatus:
        if (statusCode != SKIP_HTTP_STATUS_VALIDATION && categoryCode >= 200 && categoryCode < 300)
        {
          // 2XX codes are success codes and do not represent an error.
          return nullptr;
        }

        if (!messagePrefix.empty())
        {
            msg << messagePrefix << " ";
        }

        switch ((HttpStatusCode)categoryCode)
        {
          case HttpStatusCode::BAD_REQUEST: // 400
            msg << "Bad request (" << categoryCode
                << "). Please verify the provided subscription details and language information.";
            cancellationCode = CancellationErrorCode::BadRequest;
            retryMode = ISpxErrorInformation::RetryMode::NotAllowed;
            break;
          case HttpStatusCode::UNSUPPORTED_MEDIA_TYPE:
            cancellationCode = CancellationErrorCode::BadRequest;
            msg << "Unsupported media type (" << categoryCode
                << "). Please verify the format of the request input.";
            retryMode = ISpxErrorInformation::RetryMode::NotAllowed;
            break;
          case HttpStatusCode::UNAUTHORIZED: // 401
          case HttpStatusCode::FORBIDDEN: // 403
            msg << "Authentication error (" << categoryCode
                << "). Please check subscription information and region name.";
            cancellationCode = CancellationErrorCode::AuthenticationFailure;
            retryMode = ISpxErrorInformation::RetryMode::NotAllowed;
            break;
          case HttpStatusCode::TOO_MANY_REQUESTS: // 429
            msg << "Too many requests (" << categoryCode
                << "). Please check subscription information and region name.";
            cancellationCode = CancellationErrorCode::TooManyRequests;
            break;
          case HttpStatusCode::MOVED_PERMANENTLY: // 301
          case HttpStatusCode::TEMP_REDIRECT: // 307
          case HttpStatusCode::PERM_REDIRECT: // 308
          {
            auto isTemporary = (HttpStatusCode)categoryCode == HttpStatusCode::TEMP_REDIRECT;
            auto durationDescription = isTemporary ? "temporary" : "permanent";

            cancellationCode = isTemporary ? CancellationErrorCode::ServiceRedirectTemporary
                : CancellationErrorCode::ServiceRedirectPermanent;

            // Special case for redirects: the origination body of the error, if provided, is an available
            // redirect URL. The expectation is that this error is consumed internally via a retry.
            if (!messageBody.empty())
            {
                retryMode = ISpxErrorInformation::RetryMode::Allowed;
                msg.str(messageBody);
            }
            else
            {
                retryMode = ISpxErrorInformation::RetryMode::NotAllowed;
                msg << "A " << durationDescription << " redirect (" << categoryCode
                    << "). Redirect location unknown.";
            }

            break;
          }
          case HttpStatusCode::SERVICE_UNAVAILABLE: // 503
            msg << "Service unavailable (" << categoryCode << "). Please try the request again.";
            cancellationCode = CancellationErrorCode::ServiceUnavailable;
            break;
          case HttpStatusCode::TIMEOUT: // 408
            msg << "Timeout (" << categoryCode << "). Please try the request again.";
            cancellationCode = CancellationErrorCode::ServiceTimeout;
            break;
          case HttpStatusCode::CLIENT_CLOSED_REQUEST: // 499 (nginx)
            msg << "Request closed by client (" << categoryCode << ").";
            retryMode = ISpxErrorInformation::RetryMode::NotAllowed;
            cancellationCode = CancellationErrorCode::NoError;
            cancellationReason = CancellationReason::CancelledByUser;
            break;
          case HttpStatusCode::NOT_FOUND: // 404
          case HttpStatusCode::CONFLICT: // 409
          case HttpStatusCode::INTERNAL_ERROR: // 500
          case HttpStatusCode::NOT_IMPLEMENTED: // 501
          case HttpStatusCode::BAD_GATEWAY: // 502
          case HttpStatusCode::HTTP_VERSION_NOT_SUPPORTED: // 505
          case HttpStatusCode::VARIANT_ALSO_NEGOTIATES: // 506
          case HttpStatusCode::INSUFFICIENT_STORAGE: // 507
          case HttpStatusCode::LOOP_DETECTED: // 508
          case HttpStatusCode::BANDWIDTH_EXCEEDED: // 509
          case HttpStatusCode::NOT_EXTENDED: // 510
            cancellationCode = CancellationErrorCode::ServiceError;
            msg << "Internal service error (" << categoryCode << "). Please check request details.";
            break;
          default:
            msg << "Unspecified connection error (" << categoryCode << ").";
            cancellationCode = CancellationErrorCode::ConnectionFailure;
            break;
        }
        if (!messageSuffix.empty())
        {
            msg << " " << messageSuffix;
        }
        break;
      default:
        msg << "Unspecified error (category: " << categoryCode << ", status: " << statusCode
            << ").";
        cancellationCode = CancellationErrorCode::RuntimeError;
        break;
    }

    return std::make_shared<ErrorInfo>(
        msg.str(), categoryCode, statusCode, cancellationCode, cancellationReason, retryMode);
  }

  const std::shared_ptr<ErrorInfo> create_error(
      ErrorSource source,
      int categoryCode,
      int statusCode,
      const std::string& errorDetails)
  {
    return create_error(source, categoryCode, statusCode, "", errorDetails, "");
  }

  std::shared_ptr<ISpxErrorInformation> ErrorInfo::FromExplicitError(
      const CancellationErrorCode error,
      const std::string& errorDetails)
  {
    return create_error(ErrorSource::Explicit, (int)error, 0, errorDetails);
  }

  std::shared_ptr<ISpxErrorInformation> ErrorInfo::FromRecognitionStatus(
      const RecognitionStatus status,
      const std::string& responseText)
  {
    return create_error(ErrorSource::RecognitionStatus, (int)status, 0, responseText);
  }

  std::shared_ptr<ISpxErrorInformation> ErrorInfo::FromWebSocket(
      WebSocketError error,
      int statusCode,
      const std::string& errorDetails)
  {
    return create_error(ErrorSource::WebSocket, (int)error, statusCode, errorDetails);
  }

  std::shared_ptr<ISpxErrorInformation> ErrorInfo::FromHttpStatus(
      HttpStatusCode status,
      const std::string& messagePrefix,
      const std::string& messageSuffix)
  {
    return create_error(ErrorSource::HttpStatus, (int)status, 0, messagePrefix, "", messageSuffix);
  }

  std::shared_ptr<ISpxErrorInformation> ErrorInfo::FromErrorWithAppendedDetails(
      const std::shared_ptr<ISpxErrorInformation>& previousError,
      const std::string& extraDetails)
  {
    return std::make_shared<ErrorInfo>(
        previousError->GetDetails() + " " + extraDetails,
        previousError->GetCategoryCode(),
        previousError->GetStatusCode(),
        previousError->GetCancellationCode(),
        previousError->GetCancellationReason(),
        previousError->GetRetryMode());
  }

  std::shared_ptr<ISpxErrorInformation> ErrorInfo::FromRuntimeMessage(const std::string& message)
  {
    return create_error(
        ErrorSource::Explicit, (int)CancellationErrorCode::RuntimeError, 0, message);
  }

}}}} // namespace Microsoft::CognitiveServices::Speech::Impl
