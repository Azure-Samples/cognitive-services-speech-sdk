//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

public class BatchSynthesisPropertiesDefinition
{
    /// <summary>
    /// The requested audio output format.
    /// </summary>
    public string OutputFormat { get; set; }

    /// <summary>
    /// The value indicating whether synthesis result is concatenated.
    /// </summary>
    public bool? ConcatenateResult { get; set; }

    /// <summary>
    /// The requested destination container. It is an URL with Service adhoc SAS to a writeable container in Azure Blob storage.
    /// </summary>
    public Uri DestinationContainerUrl { get; set; }

    /// <summary>
    /// The destination path of the output in the destination container.
    /// </summary>
    public string DestinationPath { get; set; }

    /// <summary>
    /// The value indicating whether synthesis result is decopressed in target container.
    /// It only take effect when "destinationContainerUrl" is specified or "BYOS" is configured.
    /// </summary>
    public bool? DecompressOutputFiles { get; set; }

    /// <summary>
    /// The value indicating whether the word boundary output will be generated.
    /// </summary>
    public bool? WordBoundaryEnabled { get; set; }

    /// <summary>
    /// The value indicating whether the sentence boundary output will be generated.
    /// </summary>
    public bool? SentenceBoundaryEnabled { get; set; }

    /// <summary>
    /// How long the batch task will be kept in the system after it has completed. Once the
    /// batch task reaches the time to live after completion (succeeded or failed) it will be automatically
    /// deleted. The longest supported duration is 744 hours, the default vaule is 744.
    /// </summary>
    public int? TimeToLiveInHours { get; set; }
}
