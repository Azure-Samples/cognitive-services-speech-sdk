//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//


namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    using Microsoft.CognitiveServices.Speech.Tests.EndToEnd.Utils;

    static class TestData
    {
        public static string KwsDir { get; set; }

        public static class Kws
        {
            public static class Computer
            {
                public static string ModelFile => "kws/Computer/kws.table".GetRootRelativePath();
                public static string ModelKeyword => "computer";
            }
            public static class Computer2
            {
                public static string ModelFile => "kws/Computer/kws.table".GetRootRelativePath();
                public static string ModelKeyword => "computer";
            }

            public static class Secret
            {
                public static string ModelFile => "kws/Secret/kws.table".GetRootRelativePath();
                public static string ModelKeyword => "secret";
            }

            public static class Computer2Pass
            {
                public static string ModelFile => "kws/Computer2Pass/kws.table".GetRootRelativePath();
                public static string ModelKeyword => "computer";
            }
        }

        public static class DgiWreckANiceBeach
        {
            public static string DefaultRecoText => "Recognize speech.";
        }

        public static class ExpectedErrorDetails
        {
            public static string InvalidTargetLanaguageErrorMessage => @"Translation call failed: Response status code does not indicate success: 400 (Bad Request).";
            public static string InvalidVoiceNameErrorMessage => @"Synthesis service failed with code:  - Could not identify the voice 'InvalidVoice' for the text to speech service ";
        }

        public static class OfflineUnidec
        {
            // TODO: THIS IS WRONG! No Assumption should be made for this path, this must be fixed post haste.
            public static string LocalModelPathRoot => "unidec/Unidec.Model/model".GetRootRelativePath();
            public static string LocalModelLanguage => Language.EN;
        }
    }
}
