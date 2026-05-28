//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

public class Program
{
    public static void Main(string[] args)
    {
        // Uncomment to run Professional Voice Sample
        // ProfessionalVoiceSample.ProfessionalVoiceTestAsync().Wait();

        // Uncomment to run Voice Design Sample (creates a personal voice from a text prompt)
        // VoiceDesignSample.VoiceDesignTestAsync().Wait();

        PersonalVoiceSample.PersonalVoiceTestAsync().Wait();
    }
}
