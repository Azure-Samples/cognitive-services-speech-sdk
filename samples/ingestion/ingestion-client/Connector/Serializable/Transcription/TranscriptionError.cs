// <copyright file="TranscriptionError.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector
{
    public class TranscriptionError
    {
        public TranscriptionError(string code, string message)
        {
            this.Code = code;
            this.Message = message;
        }

        public string Code { get; set; }

        public string Message { get; set; }
    }
}
