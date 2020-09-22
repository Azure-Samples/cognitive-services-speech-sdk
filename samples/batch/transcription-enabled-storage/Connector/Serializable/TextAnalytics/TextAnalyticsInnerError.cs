// <copyright file="TextAnalyticsInnerError.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector.Serializable.TextAnalytics
{
    public class TextAnalyticsInnerError
    {
        public TextAnalyticsInnerError(string code, string message)
        {
            Code = code;
            Message = message;
        }

        public string Code { get; private set; }

        public string Message { get; private set; }
    }
}
