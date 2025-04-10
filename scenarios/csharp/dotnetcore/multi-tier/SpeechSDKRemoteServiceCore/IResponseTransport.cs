//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using SpeechSDKSamples.Remote.Core;
using System;
using System.Threading.Tasks;

namespace SpeechSDKSamples.Remote.Service.Core
{
    /// <summary>
    /// A transport agnostic interface for sending responses back to the client.
    /// </summary>
    public interface IResponseTransport
    {
        /// <summary>
        /// Indicates that the Speech SDK and Service have processed audio up to the point indicated.
        /// </summary>
        /// <param name="offset">The time offset from the begining of data that will no longer be needed.</param>
        /// <returns>A <see cref="Task"/> that is completed when the message has been sent.</returns>
        /// <remarks>
        /// This method is used by the remote client to know when the audio it has sent has been processed and it will no longer need to replay that audio in
        /// the event of a disconnection. Depending on what additional processing will be done with the results of the audio, you may want to hold transmission of this until that processing is done.
        /// </remarks>
        Task AcknowledgeAudioAsync(TimeSpan offset);

        /// <summary>
        /// Sends a message to the client that the recognition has stopped.
        /// </summary>
        /// <param name="reason">The <see cref="RecognitionStoppedReason"/> why recognition stopped.</param>
        /// <param name="errorCode">If applicable an error code indicating what error had occurred.</param>
        /// <param name="errorMessage">If applicable a message with additional data if an error occurred.</param>
        /// <returns></returns>
        Task StopRecognitionAsync(RecognitionStoppedReason reason, int errorCode, string errorMessage);
    }
}
