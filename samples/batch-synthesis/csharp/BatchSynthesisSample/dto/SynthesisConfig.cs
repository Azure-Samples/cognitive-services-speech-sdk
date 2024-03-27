//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

public class SynthesisConfig
{
    /// <summary>
    /// The voice name.
    /// </summary>
    public string Voice { get; set; }

    /// <summary>
    /// The speaker profile ID of target personal voice.
    /// </summary>
    public string SpeakerProfileId { get; init; }

    /// <summary>
    /// The role name.
    /// </summary>
    string Role { get; set; }

    /// <summary>
    /// The style name.
    /// </summary>
    string Style { get; set; }

    /// <summary>
    /// The style degree value.
    /// </summary>
    string StyleDegree { get; set; }

    /// <summary>
    /// The rate value.
    /// </summary>
    string Rate { get; set; }

    /// <summary>
    /// The pitch value.
    /// </summary>
    string Pitch { get; set; }

    /// <summary>
    /// The volume value.
    /// </summary>
    string Volume { get; set; }

    /// <summary>
    /// The background audio configuration.
    /// </summary>
    public BackgroundAudioConfig BackgroundAudio { get; init; }
}

public class BackgroundAudioConfig
{
    public Uri Src { get; init; }

    public long? Fadein { get; init; }

    public long? Fadeout { get; init; }

    public double? Volume { get; init; }
}
