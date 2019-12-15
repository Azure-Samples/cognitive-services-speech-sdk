//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd.Utils
{
    public static class AssertOutput
    {
        public const string WrongErrorCount = "Wrong number of Error Events";
        public const string WrongFinalResultCount = "Wrong number of Recognized Events";
        public const string WrongSessionStartedCount = "Wrong number of Session Started Events";
        public const string WrongSessionStoppedCount = "Wrong number of Session Stopped Events";
        public const string WrongSpeechEndedCount = "Wrong number of Speech Ended Events";
        public const string WrongSpeechStartedCount = "Wrong number of Speech Started Events";
        public const string WrongConnectedEventCount = "Wrong number of Connected Events";
        public const string WrongDisconnectedEventCount = "Wrong number of Disconnected Events";
        public const string ConnectedEventCountMustNotBeZero = "The number of Connected events must be greater than 0.";
        public const string ConnectedDisconnectedEventUnmatch = "Connected events do not match Disconnected events";
        public const string TranslationShouldNotBeNull = "Translation final result should not be null";
        public const string WrongTranslatedUtterancesCount = "Wrong number of translated utterances";
        public const string WrongRecognizedUtterancesCount = "Wrong number of recognized utterances";
        public const string WrongSynthesizedUtterancesCount = "Wrong number of synthesized utterances";
    }
}
