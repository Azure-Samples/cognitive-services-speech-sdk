//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace SpeechSDKSamples.Remote.Client.App
{
    /// <summary>
    /// An interface the defines the transport for an audio stream.
    /// </summary>
    internal interface IAudioStreamTransport
    {
        /// <summary>
        /// Sends the specified audio file to the specified endpoint using the specified transport.
        /// </summary>
        /// <param name="fileName">Demo file to send.</param>
        /// <param name="endpoint">Endpoint for the middle tier sample.</param>
        /// <returns></returns>
        public Task SendAudio(string fileName, Uri endpoint);
    }
}
