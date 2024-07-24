// <copyright file="CompletedMessage.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector
{
    public class CompletedMessage
    {
        public CompletedMessage(string audioFileName, string jsonResportLocation)
        {
            this.AudioFileName = audioFileName;
            this.JsonReportLocation = jsonResportLocation;
        }

        public string AudioFileName { get; set; }

        public string JsonReportLocation { get; set; }
    }
}