// <copyright file="TranscriptionProperties.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector
{
    public class TranscriptionProperties
    {
        public TranscriptionProperties(TranscriptionError error)
        {
            this.Error = error;
        }

        public TranscriptionError Error { get; set; }
    }
}
