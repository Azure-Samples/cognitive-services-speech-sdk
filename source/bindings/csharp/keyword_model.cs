//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

using System;

namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// Represents keyword recognition model used w/ StartKeywordRecognitionAsync.
    /// </summary>
    public sealed class KeywordRecognitionModel : IDisposable
    { 
        /// <summary>
        /// Creates a keyword recognition model using the specified endpoint.
        /// </summary>
        /// <param name="fileName">A string that represents file name for the keyword recognition model.</param>
        /// <returns>The keyword recognition model being created.</returns>
        public static KeywordRecognitionModel FromFile(string fileName)
        {
            return new KeywordRecognitionModel(Microsoft.CognitiveServices.Speech.Internal.KeywordRecognitionModel.FromFile(fileName));
        }

        /// <summary>
        /// Dispose of associated resources.
        /// </summary>
        public void Dispose()
        {
            if (disposed)
            {
                return;
            }

            modelImpl.Dispose();
            disposed = true;
        }

        private bool disposed = false;

        internal KeywordRecognitionModel(Microsoft.CognitiveServices.Speech.Internal.KeywordRecognitionModel model)
        {
            modelImpl = model;
        }

        internal Microsoft.CognitiveServices.Speech.Internal.KeywordRecognitionModel modelImpl { get; }
    }
}
