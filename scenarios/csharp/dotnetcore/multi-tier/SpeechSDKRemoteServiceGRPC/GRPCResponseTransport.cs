//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using Google.Protobuf.WellKnownTypes;
using Grpc.Core;
using SpeechSDKSamples.Remote.Core;
using SpeechSDKSamples.Remote.Service.Core;

namespace SpeechSDKSamples.Remote.Service.GRPC.Services
{
    /// <summary>
    /// A GRPC transport implementation for sending responses back to the client.
    /// </summary>
    internal class GRPCResponseTransport : IResponseTransport
    {
        private IServerStreamWriter<SpeechSDKRemoteResponse> _responseStream;

        public GRPCResponseTransport(IServerStreamWriter<SpeechSDKRemoteResponse> responseStream)
        {
            _responseStream = responseStream;
        }

        /// <inheritDoc/>
        public Task AcknowledgeAudioAsync(TimeSpan offset)
        {
            var message = new SpeechSDKRemoteResponse()
            {
                AcknowledgedAudio = new audioAckMessage()
                {
                    AcknowledgedAudio = Duration.FromTimeSpan(offset)
                }
            };

            return _responseStream.WriteAsync(message);
        }

        /// <inheritdoc/>
        public Task StopRecognitionAsync(RecognitionStoppedReason reason, int errorCode, string errorMessage)
        {
            var message = new SpeechSDKRemoteResponse()
            {
                RecognitionStopped = new recognitionStopped()
                {
                    Reason = reason switch
                    {
                        RecognitionStoppedReason.Unknown => recognitionStoppedReason.Unknown,
                        RecognitionStoppedReason.EndOfStream => recognitionStoppedReason.EndOfStream,
                        RecognitionStoppedReason.Error => recognitionStoppedReason.Error,
                        RecognitionStoppedReason.StopRequested => recognitionStoppedReason.StoppRequested,
                        _ => throw new NotImplementedException()
                    },
                    ErrorCode = errorCode,
                    ErrorMessage = errorMessage
                }
            };

            return _responseStream.WriteAsync(message);
        }
    }
}
