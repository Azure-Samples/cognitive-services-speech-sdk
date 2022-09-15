//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

public class BatchSynthesis : BatchSynthesisDefinition
{
    /// <summary>
    /// Gets or sets the identifier of this batch synthesis.
    /// </summary>
    public Guid Id { get; set; }

    /// <summary>
    /// Gets or sets the display name of the batch synthesis.
    /// </summary>
    public string? DisplayName { get; set; }

    /// <summary>
    /// Gets or sets the description of the batch synthesis.
    /// </summary>
    public string? Description { get; set; }

    /// <summary>
    /// Gets or sets the status of the batch synthesis.
    /// </summary>
    public string? Status { get; set; }

    /// <summary>
    /// Gets or sets the time-stamp when the object was created.
    /// The time stamp is encoded as ISO 8601 date and time format
    /// ("YYYY-MM-DDThh:mm:ssZ", see https://en.wikipedia.org/wiki/ISO_8601#Combined_date_and_time_representations).
    /// </summary>
    public DateTime CreatedDateTime { get; set; }

    /// <summary>
    /// Gets or sets the time-stamp when the current status was entered.
    /// The time stamp is encoded as ISO 8601 date and time format
    /// ("YYYY-MM-DDThh:mm:ssZ", see https://en.wikipedia.org/wiki/ISO_8601#Combined_date_and_time_representations).
    /// </summary>
    public DateTime LastActionDateTime { get; set; }

    /// <summary>
    /// Gets or sets additional configuration and additional metadata provided by the service.
    /// </summary>
    public new BatchSynthesisProperties Properties { get; set; }

    /// <summary>
    /// Gets or sets the synthesis task outputs.
    /// </summary>
    public BatchSynthesisOutputs Outputs { get; set; }
}
