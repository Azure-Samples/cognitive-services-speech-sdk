//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using Newtonsoft.Json;

public sealed class BatchSynthesisProperties : BatchSynthesisPropertiesDefinition
{
    /// <summary>
    /// The value of generated audio size in bytes.
    /// </summary>
    public long? AudioSize { get; set; }
    /// <summary>
    /// The number of succeeded audios.
    /// </summary>
    public int? SucceededAudioCount { get; set; }

    /// <summary>
    /// The number of failed audios.
    /// </summary>
    public int? FailedAudioCount { get; set; }

    /// <summary>
    /// The total audio duration in ticks.
    /// </summary>
    public long? DurationInTicks { get; set; }

    /// <summary>
    /// The duration of the transcription. The duration is encoded as ISO 8601 duration
    /// ("PnYnMnDTnHnMnS", see https://en.wikipedia.org/wiki/ISO_8601#Durations).
    /// </summary>
    [JsonConverter(typeof(TimeSpanConverter))]
    public TimeSpan Duration { get; set; }

    /// <summary>
    /// The details of billable characters by voice type in the input file.
    /// </summary>
    public BatchSynthesisBillingDetails BillingDetails { get; set; }

    /// <summary>
    /// The details of the error in case the entity is in a failed state.
    /// </summary>
    public EntityError Error { get; set; }
}
