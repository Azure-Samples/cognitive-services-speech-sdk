// <copyright file="TextAnalyticsError.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector.Serializable.TextAnalytics
{
    public class TextAnalyticsError
    {
        public TextAnalyticsError(string code, string message, TextAnalyticsInnerError innerError)
        {
            this.Code = code;
            this.Message = message;
            this.InnerError = innerError;
        }

        public string Code { get; private set; }

        public string Message { get; private set; }

        public TextAnalyticsInnerError InnerError { get; private set; }
    }
}
