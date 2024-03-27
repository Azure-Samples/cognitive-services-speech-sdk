//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

public sealed class BatchSynthesisProperties : BatchSynthesisPropertiesDefinition
{
    /// <summary>
    /// The value of generated audio size in bytes.
    /// </summary>
    public long? SizeInBytes { get; set; }
    /// <summary>
    /// The number of succeeded audios.
    /// </summary>
    public int? SucceededAudioCount { get; set; }

    /// <summary>
    /// The number of failed audios.
    /// </summary>
    public int? FailedAudioCount { get; set; }

    /// <summary>
    /// The total duration of generated audios.
    /// </summary>
    public long? DurationInMilliseconds { get; set; }


    /// <summary>
    /// The details of billable characters by voice type in the input file.
    /// </summary>
    public IDictionary<string, long> BillingDetails { get; set; }

    /// <summary>
    /// The details of the error when the synthesis job is failed.
    /// </summary>
    public SynthesisError Error { get; set; }
}

public class SynthesisError
{
    /// <summary>
    /// The code of this error.
    /// </summary>
    public string Code { get; set; }

    /// <summary>
    /// The message for this error.
    /// </summary>
    public string Message { get; set; }
}
