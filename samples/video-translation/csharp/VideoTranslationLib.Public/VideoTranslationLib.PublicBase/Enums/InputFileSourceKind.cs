// <copyright file="VideoFileSourceKind.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// </copyright>

namespace Microsoft.SpeechServices.Common.Client;

using System;

public enum InputFileSourceKind
{
    [Obsolete("Do not use directly - used to discover serializer issues.")]
    None = 0,

    AzureStorageBlobManagedIdentity,
}
