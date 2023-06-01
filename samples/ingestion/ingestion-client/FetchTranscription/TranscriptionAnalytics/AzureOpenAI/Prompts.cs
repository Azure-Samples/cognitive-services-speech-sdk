// <copyright file="Prompts.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace FetchTranscription
{
    public static class Prompts
    {
        public static string BuildGeneralCallCenterPrompt(string transcript)
        {
            return "You must extract the following information from the phone conversation below:  \\r\\n" +
                "1. Call reason (key: reason)\\r\\n" +
                "2. Names of all customers (Customer: name)\\r\\n" +
                "3. A short, yet detailed summary (key: summary)\\r\\n" +
                "4. Present the output as JSON\\r\\n" +
                "\\r\\n" +
                "Transcript:\\r\\n" +
                transcript;
        }

        public static string BuildNamedEntityRecognitionPrompt(string transcript)
        {
            return "From the text below, extract the following entities in the following format: \\r\\n" +
                "Companies: <comma-separated list of companies mentioned>\\r\\n" +
                "People & titles: <comma-separated list of people mentioned (with their titles or roles appended in parentheses)> \\r\\n" +
                "\\r\\n" +
                "Transcript:\\r\\n" +
                transcript;
        }
    }
}
