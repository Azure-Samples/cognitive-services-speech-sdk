//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

public enum DatasetKind
{
    AudioAndScript = 1,
    LongAudio = 2,
    AudioOnly = 3
}

public enum DatasetProcessAs
{
    Segmented = 1,
    Contextual = 2
}

public class Dataset
{
    public DatasetKind Kind { get; set; }

    public AzureBlobContentSource Audios { get; set; }

    public AzureBlobContentSource Scripts { get; set; }

    public DatasetProcessAs? ProcessAs { get; set; }
}
