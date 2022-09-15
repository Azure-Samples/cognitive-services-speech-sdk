//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

public class BatchSynthesisPropertiesDefinition
{
    /// <summary>
    /// Gets or sets the requested audio output format.
    /// </summary>
    public string OutputFormat { get; set; }

    /// <summary>
    /// Gets or sets a value indicating whether synthesis result is concatenated.
    /// </summary>
    public bool ConcatenateResult { get; set; }

    /// <summary>
    /// Gets or sets the requested destination container. It is an URL with Service adhoc SAS to a writeable container in Azure Blob storage.
    /// </summary>
    public Uri DestinationContainerUrl { get; set; }

    /// <summary>
    /// Gets or sets a value indicating whether synthesis result is decopressed in target container.
    /// It only take effect when "destinationContainerUrl" is specified or "BYOS" is configured.
    /// </summary>
    public bool DecompressOutputFiles { get; set; }

    /// <summary>
    /// Gets or sets a value indicating whether the word boundary output will be generated.
    /// </summary>
    public bool WordBoundaryEnabled { get; set; }

    /// <summary>
    /// Gets or sets a value indicating whether the sentence boundary output will be generated.
    /// </summary>
    public bool SentenceBoundaryEnabled { get; set; }
}
