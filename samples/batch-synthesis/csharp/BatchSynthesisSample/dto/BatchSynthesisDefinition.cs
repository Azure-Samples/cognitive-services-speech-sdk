//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

public class BatchSynthesisDefinition
{

    /// <summary>
    /// The synthesis text type, `SSML` or `PlainText`.
    /// </summary>
    public string InputKind { get; set; }

    /// <summary>
    /// The text inputs.
    /// </summary>
    public IEnumerable<BatchSynthesisInputDefinition> Inputs { get; set; }

    /// <summary>
    /// The synthesis config.
    /// Required and only effective when text type is plain text.
    /// </summary>
    public SynthesisConfig SynthesisConfig { get; set; }

    /// <summary>
    /// The custom voice map of voice name and deployment ID .
    /// Required if using one or more custom voices, e.g. {"My custom neural voice": "502ac834-6537-4bc3-9fd6-140114daa66d"}.
    /// </summary>
    public IDictionary<string, Guid> CustomVoices { get; set; }

    /// <summary>
    /// The additional configuration options when creating a new synthesis and additional metadata provided by the service.
    /// </summary>
    public BatchSynthesisPropertiesDefinition Properties { get; set; }
}

public class BatchSynthesisInputDefinition
{
    /// <summary>
    /// The input text string, it could be SSML or plain text.
    /// </summary>
    public string Content { get; set; }
}
